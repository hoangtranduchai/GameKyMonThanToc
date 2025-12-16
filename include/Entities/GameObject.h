#pragma once
#include <SDL.h>
#include <string>
#include "TextureManager.h"

// Struct này giúp việc truyền tham số khởi tạo trở nên gọn gàng
struct LoaderParams {
    LoaderParams(int x, int y, int width, int height, std::string textureID, SDL_RendererFlip flip = SDL_FLIP_NONE)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_textureID(textureID), m_flip(flip) {}

    int m_x;
    int m_y;
    int m_width;
    int m_height;
    std::string m_textureID;
    SDL_RendererFlip m_flip;
};

// Lớp trừu tượng (Abstract Class) cho mọi đối tượng game
class GameObject {
public:
    // Constructor nhận các thông số cơ bản thông qua LoaderParams
    GameObject(const LoaderParams* pParams) {}
    virtual ~GameObject() {}

    // Các hàm thuần ảo (Pure Virtual) bắt buộc lớp con phải hiện thực
    virtual void Draw() = 0;
    virtual void Update() = 0;
    virtual void Clean() = 0;

    int GetX() const {
        return m_x;
    }

    int GetY() const {
        return m_y;
    }

    int GetWidth() const {
        return m_width;
    }

    int GetHeight() const {
        return m_height;
    }

protected:
    // Mọi đối tượng đều cần vị trí và kích thước
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    
    // ID của texture và dòng/cột hiện tại (cho animation)
    std::string m_textureID; 
    int m_currentRow;
    int m_currentFrame;
    
    // Biến lật hình (trái/phải)
    SDL_RendererFlip m_flip;
};