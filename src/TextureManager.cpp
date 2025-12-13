#include "TextureManager.h"

// Khởi tạo instance duy nhất
TextureManager* TextureManager::s_Instance = 0;

// Hàm tải ảnh
bool TextureManager::Load(std::string fileName, std::string id, SDL_Renderer* pRenderer) {
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
        m_textureMap[id] = pTexture;
        return true;
    }

    std::cout << "[Lỗi] Khong the tao Texture tu Surface! Lỗi: " << SDL_GetError() << std::endl;
    return false;
}

// Hàm vẽ cơ bản (cho Background, UI)
void TextureManager::Draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Nguồn: Lấy toàn bộ ảnh
    srcRect.x = 0;
    srcRect.y = 0;
    
    // Query kích thước thật của ảnh để vẽ đúng tỉ lệ nếu cần (nhưng ở đây ta lấy toàn bộ)
    SDL_QueryTexture(m_textureMap[id], NULL, NULL, &srcRect.w, &srcRect.h);

    // Đích: Vị trí và kích thước muốn vẽ lên màn hình
    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;

    // Vẽ lên màn hình
    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
}

// Hàm vẽ Frame (cho Player, Animation)
void TextureManager::DrawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Tính toán phần ảnh cần cắt (sprite sheet logic)
    // Hiện tại Player là ảnh tĩnh, nên ta có thể để mặc định
    srcRect.x = width * currentFrame;
    srcRect.y = height * (currentRow - 1);
    srcRect.w = width;
    srcRect.h = height;
    
    // Nếu là ảnh đơn (không phải sprite sheet), query lại kích thước thật
    if (width == 0 || height == 0) {
       SDL_QueryTexture(m_textureMap[id], NULL, NULL, &srcRect.w, &srcRect.h);
       srcRect.x = 0; srcRect.y = 0;
    }

    destRect.x = x;
    destRect.y = y;
    destRect.w = (width > 0) ? width : srcRect.w; // Nếu width truyền vào là 0 thì dùng kích thước gốc
    destRect.h = (height > 0) ? height : srcRect.h;

    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
}

// Xóa texture cụ thể
void TextureManager::Drop(std::string id) {
    SDL_DestroyTexture(m_textureMap[id]);
    m_textureMap.erase(id);
}

// Dọn dẹp toàn bộ khi thoát game
void TextureManager::Clean() {
    std::map<std::string, SDL_Texture*>::iterator textureIter;
    for (textureIter = m_textureMap.begin(); textureIter != m_textureMap.end(); textureIter++) {
        SDL_DestroyTexture(textureIter->second);
    }
    m_textureMap.clear();
    
    // Xóa Font
    std::map<std::string, TTF_Font*>::iterator fontIter;
    for (fontIter = m_fontMap.begin(); fontIter != m_fontMap.end(); fontIter++) {
        TTF_CloseFont(fontIter->second);
    }
    m_fontMap.clear();
    
    std::cout << "[He thong] Da don dep Texture va Font Manager!" << std::endl;
}

// Hàm lấy kích thước ảnh
void TextureManager::GetTextureSize(std::string id, int* w, int* h) {
    if (m_textureMap[id] != nullptr) {
        SDL_QueryTexture(m_textureMap[id], NULL, NULL, w, h);
    } else {
        *w = 0;
        *h = 0;
    }
}

// Hàm vẽ Tile chuyên dụng (dùng cho bản đồ)
void TextureManager::DrawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Tính toán vị trí tile trong ảnh tileset
    srcRect.x = margin + (spacing + width) * currentFrame;
    srcRect.y = margin + (spacing + height) * (currentRow - 1);
    srcRect.w = width;
    srcRect.h = height;

    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;

    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
}

bool TextureManager::LoadFont(std::string fileName, std::string id, int size) {
    TTF_Font* pFont = TTF_OpenFont(fileName.c_str(), size);
    if (pFont == nullptr) {
        std::cout << "[Lỗi AAA] Khong the tai font: " << fileName << " - Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }
    m_fontMap[id] = pFont;
    return true;
}

void TextureManager::DrawText(std::string fontId, std::string text, int x, int y, SDL_Color color, SDL_Renderer* pRenderer) {
    if (m_fontMap[fontId] == nullptr) return;

    // Tạo surface từ text
    SDL_Surface* pSurface = TTF_RenderText_Solid(m_fontMap[fontId], text.c_str(), color);
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