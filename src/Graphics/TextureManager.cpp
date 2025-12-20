#include "Graphics/TextureManager.h"
#include <SDL_image.h>
#include <iostream>

// Khởi tạo con trỏ Singleton là null ban đầu
TextureManager* TextureManager::s_Instance = nullptr;

TextureManager* TextureManager::GetInstance() {
    // Chỉ tạo instance khi được gọi lần đầu tiên
    if (s_Instance == nullptr) s_Instance = new TextureManager();
    return s_Instance;
}

void TextureManager::DestroyInstance() {
    if (s_Instance != nullptr) {
        s_Instance->Clean(); // Dọn dẹp tài nguyên trước khi hủy đối tượng
        delete s_Instance;
        s_Instance = nullptr;
    }
}

TextureManager::TextureManager() {}

TextureManager::~TextureManager() { Clean(); }

void TextureManager::Clean() {
    // 1. Giải phóng toàn bộ Texture
    // Duyệt qua từng phần tử trong Map và hủy Texture
    for (auto const& [id, texture] : m_textureMap) SDL_DestroyTexture(texture);
    m_textureMap.clear(); // Xóa sạch danh sách định danh

    // 2. Giải phóng toàn bộ Font
    for (auto const& [id, font] : m_fontMap) TTF_CloseFont(font);
    m_fontMap.clear();

    std::cout << "[ĐỒ HỌA] Bộ nhớ TextureManager đã được dọn dẹp." << std::endl;
}

bool TextureManager::Load(const std::string& fileName, const std::string& id, SDL_Renderer* pRenderer) {
    // Kiểm tra xem texture đã được tải chưa
    if (m_textureMap.find(id) != m_textureMap.end()) return true;

    SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());
    if (pTempSurface == nullptr) {
        std::cerr << "[LỖI] TextureManager::Load - Không thể tải hình ảnh: " << fileName 
                  << " | Lỗi SDL_Image: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
    SDL_FreeSurface(pTempSurface);

    if (pTexture == nullptr) {
        std::cerr << "[LỖI] TextureManager::Load - Không thể tạo texture từ surface: " << fileName 
                  << " | Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    m_textureMap[id] = pTexture;
    std::cout << "[TÀI NGUYÊN] Đã tải Texture: " << id << " (" << fileName << ")" << std::endl;
    
    return true;
}

void TextureManager::ClearTexture(const std::string& id) {
    // Tìm iterator của texture cần xóa
    auto it = m_textureMap.find(id);
    
    // Nếu tìm thấy
    if (it != m_textureMap.end()) {
        SDL_DestroyTexture(it->second); // Hủy texture
        m_textureMap.erase(it);         // Xóa khỏi danh sách quản lý
        std::cout << "[TÀI NGUYÊN] Đã xóa Texture: " << id << std::endl;
    }
}

// 3. QUẢN LÝ PHÔNG CHỮ (FONT MANAGEMENT)

bool TextureManager::LoadFont(const std::string& fileName, const std::string& id, int fontSize) {
    if (m_fontMap.find(id) != m_fontMap.end()) return true;

    // Mở file phông chữ với kích thước cụ thể
    TTF_Font* pFont = TTF_OpenFont(fileName.c_str(), fontSize);
    if (pFont == nullptr) {
        std::cerr << "[LỖI] TextureManager::LoadFont - Không thể tải phông chữ: " << fileName << " | Lỗi TTF: " << TTF_GetError() << std::endl;
        return false;
    }

    m_fontMap[id] = pFont;
    std::cout << "[TÀI NGUYÊN] Đã tải Font: " << id << " (" << fontSize << "pt)" << std::endl;
    return true;
}

// 4. KẾT XUẤT ĐỒ HỌA (RENDERING)

void TextureManager::Draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* pRenderer) {
    // Nếu texture chưa tải, thoát để tránh lỗi crash
    if (m_textureMap.find(id) == m_textureMap.end()) {
        std::cerr << "[CẢNH BÁO] Không tìm thấy ID Texture: " << id << std::endl;
        return;
    }

    SDL_Rect srcRect;  // Vùng lấy ảnh gốc
    SDL_Rect destRect; // Vùng vẽ trên màn hình

    // Lấy toàn bộ kích thước ảnh gốc
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = destRect.w = width;
    srcRect.h = destRect.h = height;

    // Vị trí đích
    destRect.x = x;
    destRect.y = y;

    // Gọi lệnh vẽ của SDL
    SDL_RenderCopy(pRenderer, m_textureMap[id], &srcRect, &destRect);
}

void TextureManager::DrawFrameScaled(const std::string& id, int x, int y, int srcWidth, int srcHeight, float scale, int currentRow, int currentFrame, SDL_Renderer* pRenderer) {
    if (m_textureMap.find(id) == m_textureMap.end()) return;

    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = srcWidth * currentFrame;
    srcRect.y = srcHeight * currentRow;
    srcRect.w = srcWidth;
    srcRect.h = srcHeight;

    destRect.x = x;
    destRect.y = y;
    destRect.w = static_cast<int>(srcWidth * scale);
    destRect.h = static_cast<int>(srcHeight * scale);

    SDL_RenderCopy(pRenderer, m_textureMap[id], &srcRect, &destRect);
}

void TextureManager::DrawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer) {
    // Nếu texture chưa load, thoát để tránh Crash
    if (m_textureMap.find(id) == m_textureMap.end()) return;

    SDL_Rect srcRect;
    SDL_Rect destRect;

    // XỬ LÝ CẮT SPRITESHEET (SPRITESHEET CLIPPING LOGIC)
    
    // Dịch khung nhìn sang phải theo số thứ tự frame
    srcRect.x = width * currentFrame;

    // Dịch khung nhìn xuống dưới (ảnh là 1 hàng nên row = 0)
    srcRect.y = height * currentRow;

    // Kích thước ô cắt
    srcRect.w = width;
    srcRect.h = height;

    // Vị trí vẽ
    destRect.x = x;
    destRect.y = y;
    destRect.w = width;  // Vẽ đúng bằng kích thước 1 frame
    destRect.h = height;

    SDL_RenderCopy(pRenderer, m_textureMap[id], &srcRect, &destRect);
}