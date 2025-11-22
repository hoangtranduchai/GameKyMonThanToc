#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>

// Forward Declaration để tránh lỗi vòng lặp include
class IGameState;

class GameEngine {
private:
    // Lưu trữ các trạng thái game
    std::vector<IGameState*> m_States;

    bool m_bRunning;
    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;
    SDL_Texture* m_pPlayerTexture; // "Kết cấu" của nhân vật
    SDL_Rect m_playerDestRect; // "Trạng thái" của nhân vật (Vị trí và Kích thước)

    // "Tốc độ" của nhân vật (Pixel/giây)
    float m_playerSpeed; 

    // "Trạng thái" Input của người chơi (Di chuyển)
    bool m_movingUp;
    bool m_movingDown;
    bool m_movingLeft;
    bool m_movingRight;

    // Kích thước cửa sổ để giới hạn di chuyển
    int m_windowWidth;
    int m_windowHeight;

public:
    GameEngine();
    ~GameEngine();

    // Cập nhật hàm Init để nhận thông tin cửa sổ
    bool Init(const char* title, int x, int y, int w, int h, bool fullscreen);
    
    void Quit();

    // "Nhịp đập" của Kiệt tác (Vòng lặp Game)
    void HandleEvents(); // Xử lý Input
    void Update();       // Cập nhật Logic
    void Render();       // Vẽ Đồ họa

    // Hàm kiểm tra trạng thái
    bool IsRunning() {
        return m_bRunning;
    }
};