#pragma once
#include <SDL.h>
#include <algorithm>

class Camera {
public:
    // Trả về singleton instance
    static Camera* GetInstance() {
        static Camera instance;
        return &instance;
    }

    // Khởi tạo Camera với kích thước màn hình
    void Init(int screenW, int screenH) {
        m_screenWidth = screenW;
        m_screenHeight = screenH;
        m_zoom = 1.0f;
        m_viewBox = {0, 0, screenW, screenH};
    }

    // Cập nhật vị trí Camera dựa trên nhân vật và kích thước Map
    void Update(int targetX, int targetY, int mapWidth, int mapHeight) {
        // 1. Tính toán vị trí mong muốn (Nhân vật ở giữa màn hình)
        // Công thức: CameraX = PlayerX - (ScreenW / 2) / Zoom
        m_viewBox.x = targetX - (m_screenWidth / 2) / m_zoom;
        m_viewBox.y = targetY - (m_screenHeight / 2) / m_zoom;

        // 2. Kẹp (Clamp) Camera không cho ra ngoài mép bản đồ
        if (m_viewBox.x < 0) m_viewBox.x = 0;
        if (m_viewBox.y < 0) m_viewBox.y = 0;
        
        // Tính kích thước Map thực tế sau khi zoom
        // Nếu map nhỏ hơn màn hình thì không cần clamp phía bên phải/dưới
        if (mapWidth > m_screenWidth / m_zoom) {
            if (m_viewBox.x > mapWidth - m_screenWidth / m_zoom) {
                m_viewBox.x = mapWidth - m_screenWidth / m_zoom;
            }
        }
        
        if (mapHeight > m_screenHeight / m_zoom) {
             if (m_viewBox.y > mapHeight - m_screenHeight / m_zoom) {
                m_viewBox.y = mapHeight - m_screenHeight / m_zoom;
            }
        }
    }

    // Tính toán Zoom tự động cho Map nhỏ (Auto-Fit)
    void AutoFit(int mapW, int mapH) {
        // Nếu Map nhỏ hơn màn hình, Zoom lên cho vừa khít
        float scaleX = (float)m_screenWidth / mapW;
        float scaleY = (float)m_screenHeight / mapH;
        
        // Chọn tỉ lệ nhỏ hơn để đảm bảo map nằm trọn trong màn hình (hoặc lớn hơn tùy ý đồ)
        // Ở đây ta chọn scale nhỏ nhất để map vừa khít, nhưng tối thiểu là 1.0 (không thu nhỏ map lớn)
        m_zoom = std::min(scaleX, scaleY);
        
        // Giới hạn zoom tối thiểu là 1.0f để không bị quá nhỏ
        if (m_zoom < 1.0f) m_zoom = 1.0f;
        
        // Giới hạn zoom tối đa (ví dụ 4.0f) để không bị vỡ hạt quá mức
        if (m_zoom > 4.0f) m_zoom = 4.0f;
    }

    // Chức năng Zoom thủ công
    void AddZoom(float amount) {
        m_zoom += amount;
        if (m_zoom < 0.5f) m_zoom = 0.5f; // Không cho zoom quá nhỏ
        if (m_zoom > 5.0f) m_zoom = 5.0f; // Không cho zoom quá to
    }

    SDL_Rect GetViewBox() const { return m_viewBox; }
    float GetZoom() const { return m_zoom; }

private:
    Camera() : m_zoom(1.0f), m_screenWidth(0), m_screenHeight(0) {}
    SDL_Rect m_viewBox;
    float m_zoom;
    int m_screenWidth;
    int m_screenHeight;
};