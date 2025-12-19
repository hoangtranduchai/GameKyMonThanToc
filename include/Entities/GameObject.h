#pragma once

// THƯ VIỆN HỆ THỐNG
#include <string>
#include <iostream>
#include <SDL.h>

// MẪU THIẾT KẾ ĐỐI TƯỢNG THAM SỐ (PARAMETER OBJECT PATTERN)

// STRUCT: LoaderParams (THAM SỐ KHỞI TẠO)
// Giúp việc truyền tham số khởi tạo trở nên gọn gàng và dễ bảo trì
struct LoaderParams {
public:
    LoaderParams(int x, int y, int width, int height, const std::string& textureID, 
                 int numFrames = 8, int animSpeed = 100)
        : m_x(x), m_y(y), m_width(width), m_height(height), 
          m_textureID(textureID), m_numFrames(numFrames), m_animSpeed(animSpeed)
    {}

    // Các hàm truy xuất (Getters)
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    std::string GetTextureID() const { return m_textureID; }
    int GetNumFrames() const { return m_numFrames; }
    int GetAnimSpeed() const { return m_animSpeed; }

private:
    int m_x, m_y;
    int m_width, m_height;
    std::string m_textureID;
    int m_numFrames;
    int m_animSpeed;
};

// CLASS: GameObject (LỚP TRỪU TƯỢNG)
class GameObject {
public:
    // Constructor nhận các thông số cơ bản thông qua LoaderParams
    GameObject(const LoaderParams* pParams) 
        : m_x((float)pParams->GetX()), // Ép kiểu sang float để di chuyển mượt
          m_y((float)pParams->GetY()),
          m_width(pParams->GetWidth()), 
          m_height(pParams->GetHeight()),
          m_textureID(pParams->GetTextureID()),
          m_numFrames(pParams->GetNumFrames()),
          m_animSpeed(pParams->GetAnimSpeed()),
          m_currentRow(0),    // Asset dải ngang -> Luôn là hàng 0
          m_currentFrame(0),   // Frame bắt đầu
          m_alpha(255)
    {}
    
    // Hàm hủy ảo (Virtual Destructor)
    virtual ~GameObject() {}

    // Các hàm thuần ảo (Pure Virtual) bắt buộc lớp con phải định nghĩa lại
    virtual void Draw() = 0;
    virtual void Update() = 0;
    virtual void Clean() = 0;

    // GETTERS & SETTERS

    // Vị trí thực tế
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    // Thiết lập vị trí
    void SetPosition(float x, float y) { m_x = x; m_y = y; }

    // Kích thước
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // Hàm đổi Texture
    // Dùng để đổi từ "idle_down" sang "run_up" khi di chuyển
    void SetTexture(const std::string& id) { m_textureID = id; }

protected:
    // Vị trí
    float m_x, m_y;
    
    // Kích thước hiển thị
    int m_width, m_height;
    
    // ID của ảnh trong TextureManager (VD: "idle_down", "run_right")
    std::string m_textureID;

    // Biến Animation
    int m_currentRow;       // Luôn là 0 vì asset dải ngang
    int m_currentFrame;     // Chạy từ 0 đến 7 (8 frames)
    int m_numFrames;        // Tổng số frame (8)
    int m_animSpeed;        // Tốc độ update frame (ms)

    // Độ mờ (Alpha)
    int m_alpha;
};