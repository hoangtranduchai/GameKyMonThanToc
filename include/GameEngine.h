#pragma once // Đảm bảo tệp header chỉ được thêm một lần
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
    SDL_Texture* m_pPlayerTexture; // "Kết cấu" cho đồ họa đầu tiên

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