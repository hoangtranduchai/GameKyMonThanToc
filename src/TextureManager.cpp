#include "TextureManager.h"
#include <iostream> // Dùng để in log lỗi

// Phải khởi tạo con trỏ tĩnh (static) ở ngoài class
TextureManager* TextureManager::s_pInstance = nullptr;

// --- Hàm Tải (Load) ---
bool TextureManager::Load(std::string id, std::string filepath, SDL_Renderer* renderer) {
    // 1. Tải file ảnh về dưới dạng "Surface"
    SDL_Surface* pTempSurface = IMG_Load(filepath.c_str());
    if (pTempSurface == nullptr) {
        std::cout << "Failed to load image: " << filepath << ". Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // 2. Chuyển "Surface" (trên CPU) thành "Texture" (tối ưu cho GPU)
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pTempSurface);

    // 3. Dọn dẹp Surface (không cần nữa)
    SDL_FreeSurface(pTempSurface);

    if (pTexture == nullptr) {
        std::cout << "Failed to create texture from surface. Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 4. Lưu vào map
    m_TextureMap[id] = pTexture;
    std::cout << "Loaded texture '" << id << "' from " << filepath << std::endl;
    return true;
}

// --- Hàm Dọn dẹp (Clean) ---
void TextureManager::Clean() {
    std::cout << "Cleaning TextureManager..." << std::endl;
    // Lặp qua toàn bộ map và hủy từng texture
    for (auto const& [id, pTexture] : m_TextureMap) {
        SDL_DestroyTexture(pTexture);
    }
    m_TextureMap.clear(); // Xóa sạch map
}

// --- Hàm Vẽ Đơn giản (Draw) ---
void TextureManager::Draw(std::string id, int x, int y, int w, int h, SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect;  // Hình chữ nhật NGUỒN (lấy phần nào của ảnh)
    SDL_Rect destRect; // Hình chữ nhật ĐÍCH (vẽ vào đâu trên màn hình)

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = w; // Lấy toàn bộ chiều rộng...
    srcRect.h = h; // ... và chiều cao của ảnh

    destRect.x = x;
    destRect.y = y;
    destRect.w = w;
    destRect.h = h;

    // Vẽ lên renderer
    SDL_RenderCopyEx(renderer, m_TextureMap[id], &srcRect, &destRect, 0, 0, flip);
}

// --- Hàm Vẽ Frame (DrawFrame) ---
void TextureManager::DrawFrame(std::string id, int x, int y, int w, int h, int row, int frame, SDL_Renderer* renderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Tính toán NGUỒN (srcRect)
    // Đây là logic quan trọng của spritesheet
    srcRect.x = w * frame;      // Cột (frame) thứ mấy
    srcRect.y = h * (row - 1);  // Hàng (row) thứ mấy (giả sử hàng 1 là index 0)
    srcRect.w = w;              // Chiều rộng 1 frame
    srcRect.h = h;              // Chiều cao 1 frame

    destRect.x = x;
    destRect.y = y;
    destRect.w = w;
    destRect.h = h;

    // Vẽ frame đã chọn
    SDL_RenderCopyEx(renderer, m_TextureMap[id], &srcRect, &destRect, 0, 0, flip);
}