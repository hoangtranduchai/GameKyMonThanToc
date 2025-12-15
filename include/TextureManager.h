#pragma once
#include <SDL_ttf.h>
#include "GameEngine.h"
#include <string>
#include <unordered_map>

// Class quản lý texture, sử dụng Singleton Pattern
class TextureManager {
public:
    // Tải ảnh từ file và gán cho nó một ID (tên định danh)
    bool Load(const std::string& fileName, const std::string& id, SDL_Renderer* pRenderer);

    // Vẽ ảnh lên màn hình (Draw tĩnh - dùng cho Background)
    void Draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE) noexcept;

    // Vẽ một phần ảnh (DrawFrame - dùng cho nhân vật có Animation sau này)
    // Hiện tại chúng ta dùng nó để vẽ Player mượt mà hơn
    void DrawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, double scale = 1.0, SDL_RendererFlip flip = SDL_FLIP_NONE) noexcept;
    
    // Xóa một texture cụ thể khỏi bộ nhớ
    void Drop(const std::string& id) noexcept;
    
    // Dọn dẹp toàn bộ texture (dùng khi tắt game)
    void Clean() noexcept;

    // Lấy con trỏ texture gốc (để chỉnh Alpha/Color Mod)
    SDL_Texture* GetTexture(const std::string& id) const noexcept {
        auto it = m_textureMap.find(id);
        return it != m_textureMap.end() ? it->second : nullptr;
    }
    
    // Lấy kích thước ảnh (để tính toán va chạm hoặc scale)
    void GetTextureSize(const std::string& id, int* w, int* h) const noexcept;

    // Hàm vẽ Tile chuyên dụng (dùng cho bản đồ)
    void DrawTile(const std::string& id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer) noexcept;
    
    // Singleton Instance
    static TextureManager* GetInstance() {
        if (s_Instance == 0) {
            s_Instance = new TextureManager();
        }
        return s_Instance;
    }

    // --- THÊM MỚI CHO HỆ THỐNG FONT AAA ---
    // Tải font với kích thước cụ thể
    bool LoadFont(const std::string& fileName, const std::string& id, int size);
    
    // Vẽ chữ lên màn hình
    void DrawText(const std::string& fontId, const std::string& text, int x, int y, SDL_Color color, SDL_Renderer* pRenderer) noexcept;

    // --- Text caching API ---
    // Tạo và cache texture chữ với ID, để vẽ nhiều lần không cần tạo lại
    bool CreateTextTexture(const std::string& fontId, const std::string& text, const std::string& id, SDL_Color color, SDL_Renderer* pRenderer);
    // Vẽ lại chữ đã cache theo ID
    void DrawTextCached(const std::string& id, int x, int y, SDL_Renderer* pRenderer) const noexcept;
    // Xóa chữ cache
    void DropText(const std::string& id) noexcept;

private:
    TextureManager() {}
    ~TextureManager() {}

    // Kho chứa "kho báu" texture: Map ánh xạ từ ID (string) sang SDL_Texture*
    std::unordered_map<std::string, SDL_Texture*> m_textureMap;
    // Cache texture dimensions to avoid repeated SDL_QueryTexture calls
    std::unordered_map<std::string, std::pair<int, int>> m_textureSizeCache;

    static TextureManager* s_Instance;

    // Kho chứa Font
    std::unordered_map<std::string, TTF_Font*> m_fontMap;

    // Cache cho text (texture tạo từ font)
    std::unordered_map<std::string, SDL_Texture*> m_textCache;
    std::unordered_map<std::string, std::pair<int, int>> m_textSizeCache;
};