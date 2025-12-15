#include "TextureManager.h"

// Khởi tạo instance duy nhất
TextureManager* TextureManager::s_Instance = 0;

// Hàm tải ảnh
bool TextureManager::Load(const std::string& fileName, const std::string& id, SDL_Renderer* pRenderer) {
    // Tạo surface tạm thời
    SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());
    if (pTempSurface == 0) {
        std::cout << "[Lỗi] Khong the tai anh: " << fileName << " - Lỗi: " << IMG_GetError() << std::endl;
        return false;
    }

    // Chuyển surface thành texture (tối ưu cho GPU)
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
    SDL_FreeSurface(pTempSurface); // Xóa surface ngay để giải phóng RAM

    if (pTexture != 0) {
        // Lưu texture vào kho chứa với ID định danh
        m_textureMap.emplace(id, pTexture);
        // Cache dimensions immediately to avoid repeated SDL_QueryTexture
        int w = 0, h = 0;
        SDL_QueryTexture(pTexture, NULL, NULL, &w, &h);
        m_textureSizeCache.emplace(id, std::make_pair(w, h));
        return true;
    }

    std::cout << "[Lỗi] Khong the tao Texture tu Surface! Lỗi: " << SDL_GetError() << std::endl;
    return false;
}

// Hàm vẽ cơ bản (cho Background, UI)
void TextureManager::Draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip) noexcept {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Nguồn: Lấy toàn bộ ảnh
    srcRect.x = 0;
    srcRect.y = 0;
    
    // Tránh operator[] gây tạo phần tử; kiểm tra tồn tại
    auto it = m_textureMap.find(id);
    if (it == m_textureMap.end()) return;
    // Use cached dimensions instead of repeated SDL_QueryTexture call
    auto sizeIt = m_textureSizeCache.find(id);
    if (sizeIt != m_textureSizeCache.end()) {
        srcRect.w = sizeIt->second.first;
        srcRect.h = sizeIt->second.second;
    } else {
        SDL_QueryTexture(it->second, NULL, NULL, &srcRect.w, &srcRect.h);
    }

    // Đích: Vị trí và kích thước muốn vẽ lên màn hình
    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;

    // Vẽ lên màn hình
    SDL_RenderCopyEx(pRenderer, it->second, &srcRect, &destRect, 0, 0, flip);
}

// Hàm vẽ Frame (cho Player, Animation)
void TextureManager::DrawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, double scale, SDL_RendererFlip flip) noexcept {
    // An toàn: Không vẽ nếu chưa load
    auto it = m_textureMap.find(id);
    if (it == m_textureMap.end()) return;
    
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // 1. Tính toán phần ảnh CẮT từ Sprite Sheet (Kích thước gốc)
    // Đây là kích thước thật trong file ảnh
    srcRect.x = width * currentFrame;
    srcRect.y = height * (currentRow - 1);
    srcRect.w = width;
    srcRect.h = height;
    
    // 2. Tính toán phần ảnh VẼ ra màn hình (Scale nhân vật)
    // TÍNH TOÁN THEO SCALE: Kích thước đích = Kích thước gốc * Tỷ lệ
    destRect.w = (int)(width * scale);
    destRect.h = (int)(height * scale);

    // 3. Vị trí vẽ
    destRect.x = x;
    destRect.y = y;

    SDL_RenderCopyEx(pRenderer, it->second, &srcRect, &destRect, 0, 0, flip);
}

// Xóa texture cụ thể
void TextureManager::Drop(const std::string& id) noexcept {
    auto it = m_textureMap.find(id);
    if (it != m_textureMap.end()) {
        SDL_DestroyTexture(it->second);
        m_textureMap.erase(it);
        m_textureSizeCache.erase(id);
    }
}

// Dọn dẹp toàn bộ khi thoát game
void TextureManager::Clean() noexcept {
    for (auto& kv : m_textureMap) {
        SDL_DestroyTexture(kv.second);
    }
    m_textureMap.clear();
    m_textureSizeCache.clear();
    
    // Xóa Font
    for (auto& kv : m_fontMap) {
        TTF_CloseFont(kv.second);
    }
    m_fontMap.clear();
    
    std::cout << "[He thong] Da don dep Texture va Font Manager!" << std::endl;
}

// Hàm lấy kích thước ảnh
void TextureManager::GetTextureSize(const std::string& id, int* w, int* h) const noexcept {
    // Use cached size if available (faster than SDL_QueryTexture)
    auto sizeIt = m_textureSizeCache.find(id);
    if (sizeIt != m_textureSizeCache.end()) {
        *w = sizeIt->second.first;
        *h = sizeIt->second.second;
    } else {
        *w = 0;
        *h = 0;
    }
}

// Hàm vẽ Tile chuyên dụng (dùng cho bản đồ)
void TextureManager::DrawTile(const std::string& id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer) noexcept {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Tính toán vị trí tile trong ảnh tileset
        // Tính toán vị trí tile trong ảnh tileset (fast path when margin/spacing == 0 and row == 1)
        if (margin == 0 && spacing == 0 && currentRow == 1) {
            srcRect.x = width * currentFrame;
            srcRect.y = 0;
        } else {
            srcRect.x = margin + (spacing + width) * currentFrame;
            srcRect.y = margin + (spacing + height) * (currentRow - 1);
        }
    srcRect.w = width;
    srcRect.h = height;

    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;
    auto it = m_textureMap.find(id);
    if (it == m_textureMap.end()) return;
    // No rotation/flip needed for tiles; use faster RenderCopy
    SDL_RenderCopy(pRenderer, it->second, &srcRect, &destRect);
}

bool TextureManager::LoadFont(const std::string& fileName, const std::string& id, int size) {
    TTF_Font* pFont = TTF_OpenFont(fileName.c_str(), size);
    if (pFont == nullptr) {
        std::cout << "[Lỗi AAA] Khong the tai font: " << fileName << " - Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }
    m_fontMap.emplace(id, pFont);
    return true;
}

void TextureManager::DrawText(const std::string& fontId, const std::string& text, int x, int y, SDL_Color color, SDL_Renderer* pRenderer) noexcept {
    auto fIt = m_fontMap.find(fontId);
    if (fIt == m_fontMap.end() || fIt->second == nullptr) return;

    // Tạo surface từ text
    SDL_Surface* pSurface = TTF_RenderText_Solid(fIt->second, text.c_str(), color);
    if (pSurface == nullptr) return;

    // Tạo texture từ surface
    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    
    // Thiết lập khung hình chữ nhật chứa text
    SDL_Rect srcRect = {0, 0, pSurface->w, pSurface->h};
    SDL_Rect destRect = {x, y, pSurface->w, pSurface->h};

    // Vẽ lên màn hình
    SDL_RenderCopy(pRenderer, pTexture, &srcRect, &destRect);

    // Dọn dẹp bộ nhớ ngay lập tức
    SDL_FreeSurface(pSurface);
    SDL_DestroyTexture(pTexture);
}

bool TextureManager::CreateTextTexture(const std::string& fontId, const std::string& text, const std::string& id, SDL_Color color, SDL_Renderer* pRenderer) {
    auto fIt = m_fontMap.find(fontId);
    if (fIt == m_fontMap.end() || fIt->second == nullptr) return false;

    SDL_Surface* pSurface = TTF_RenderText_Solid(fIt->second, text.c_str(), color);
    if (pSurface == nullptr) return false;

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    if (pTexture == nullptr) {
        SDL_FreeSurface(pSurface);
        return false;
    }

    m_textCache.emplace(id, pTexture);
    m_textSizeCache.emplace(id, std::make_pair(pSurface->w, pSurface->h));

    SDL_FreeSurface(pSurface);
    return true;
}

void TextureManager::DrawTextCached(const std::string& id, int x, int y, SDL_Renderer* pRenderer) const noexcept {
    auto it = m_textCache.find(id);
    if (it == m_textCache.end() || it->second == nullptr) return;

    auto sizeIt = m_textSizeCache.find(id);
    if (sizeIt == m_textSizeCache.end()) return;

    SDL_Rect srcRect = {0, 0, sizeIt->second.first, sizeIt->second.second};
    SDL_Rect destRect = {x, y, sizeIt->second.first, sizeIt->second.second};
    SDL_RenderCopy(pRenderer, it->second, &srcRect, &destRect);
}

void TextureManager::DropText(const std::string& id) noexcept {
    auto it = m_textCache.find(id);
    if (it != m_textCache.end()) {
        SDL_DestroyTexture(it->second);
        m_textCache.erase(it);
    }
    m_textSizeCache.erase(id);
}