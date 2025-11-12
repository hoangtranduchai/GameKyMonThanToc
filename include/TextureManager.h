#pragma once

#include <string>
#include <map> // Chúng ta sẽ dùng map để lưu trữ texture
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class TextureManager {
private:
    // Cấm người khác tạo mới bằng constructor
    TextureManager() {

    }

    ~TextureManager() {
        
    }

    // Cấm sao chép
    TextureManager(const TextureManager&);
    TextureManager& operator=(const TextureManager&);
    
    // Biến map: lưu trữ "id" (string) và "con trỏ texture" (SDL_Texture*)
    std::map<std::string, SDL_Texture*> m_TextureMap;

    // Thể hiện (instance) tĩnh, duy nhất
    static TextureManager* s_pInstance;

public:
    // Hàm tĩnh (static) để truy cập thể hiện (instance) duy nhất
    static TextureManager* GetInstance() {
        // Tạo mới nếu chưa tồn tại
        if (s_pInstance == nullptr) {
            s_pInstance = new TextureManager();
        }
        return s_pInstance;
    }

    // Tải một file hình ảnh vào bộ nhớ
    // id: Tên định danh (vd: "player", "tileset")
    // filepath: Đường dẫn đến file (vd: "assets/player.png")
    bool Load(std::string id, std::string filepath, SDL_Renderer* renderer);

    // Dọn dẹp một texture cụ thể (nếu cần)
    void ClearFromTextureMap(std::string id);

    // Dọn dẹp TOÀN BỘ texture khi thoát game
    void Clean();

    // --- CÁC HÀM VẼ ---

    // 1. Vẽ một ảnh tĩnh (đơn giản)
    void Draw(std::string id, int x, int y, int w, int h, 
              SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // 2. Vẽ một frame từ một spritesheet (dùng cho Player animation)
    // row: Hàng thứ mấy trong spritesheet
    // frame: Cột (frame) thứ mấy trong spritesheet
    void DrawFrame(std::string id, int x, int y, int w, int h, 
                   int row, int frame, 
                   SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
};