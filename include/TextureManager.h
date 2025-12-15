#pragma once
#include <SDL_ttf.h>
#include "GameEngine.h"
#include <string>
#include <map>

// Class quản lý texture, sử dụng Singleton Pattern
class TextureManager {
public:
    // Tải ảnh từ file và gán cho nó một ID (tên định danh)
    bool Load(std::string fileName, std::string id, SDL_Renderer* pRenderer);

    // Vẽ ảnh lên màn hình (Draw tĩnh - dùng cho Background)
    void Draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Vẽ một phần ảnh (DrawFrame - dùng cho nhân vật có Animation sau này)
    // Hiện tại chúng ta dùng nó để vẽ Player mượt mà hơn
    void DrawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, double scale = 1.0, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Xóa một texture cụ thể khỏi bộ nhớ
    void Drop(std::string id);
    
    // Dọn dẹp toàn bộ texture (dùng khi tắt game)
    void Clean();

    // Lấy con trỏ texture gốc (để chỉnh Alpha/Color Mod)
    SDL_Texture* GetTexture(std::string id) {
        return m_textureMap[id];
    }
    
    // Lấy kích thước ảnh (để tính toán va chạm hoặc scale)
    void GetTextureSize(std::string id, int* w, int* h);

    // Hàm vẽ Tile chuyên dụng (dùng cho bản đồ)
    void DrawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer);
    
    // Singleton Instance
    static TextureManager* GetInstance() {
        if (s_Instance == 0) {
            s_Instance = new TextureManager();
        }
        return s_Instance;
    }

    // --- THÊM MỚI CHO HỆ THỐNG FONT AAA ---
    // Tải font với kích thước cụ thể
    bool LoadFont(std::string fileName, std::string id, int size);
    
    // Vẽ chữ lên màn hình
    void DrawText(std::string fontId, std::string text, int x, int y, SDL_Color color, SDL_Renderer* pRenderer);

private:
    TextureManager() {}
    ~TextureManager() {}

    // Kho chứa "kho báu" texture: Map ánh xạ từ ID (string) sang SDL_Texture*
    std::map<std::string, SDL_Texture*> m_textureMap;

    static TextureManager* s_Instance;

    // Kho chứa Font
    std::map<std::string, TTF_Font*> m_fontMap;
};