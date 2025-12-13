#pragma once
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
    void DrawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Xóa một texture cụ thể khỏi bộ nhớ
    void Drop(std::string id);
    
    // Dọn dẹp toàn bộ texture (dùng khi tắt game)
    void Clean();
    
    // Lấy kích thước ảnh (để tính toán va chạm hoặc scale)
    void GetTextureSize(std::string id, int* w, int* h);
    
    // Singleton Instance
    static TextureManager* GetInstance() {
        if (s_Instance == 0) {
            s_Instance = new TextureManager();
        }
        return s_Instance;
    }

private:
    TextureManager() {}
    ~TextureManager() {}

    // Kho chứa "kho báu" texture: Map ánh xạ từ ID (string) sang SDL_Texture*
    std::map<std::string, SDL_Texture*> m_textureMap;

    static TextureManager* s_Instance;
};