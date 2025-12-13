#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include "TextureManager.h"

// Forward Declaration để tránh lỗi vòng lặp include
class IGameState;
class Player;

class GameEngine {
public:
    // Mẫu thiết kế Singleton: Trả về thể hiện duy nhất
    static GameEngine* GetInstance();
    
    // Hàm hủy Instance
    static void DestroyInstance();

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

    SDL_Renderer* GetRenderer() const {
        return m_pRenderer;
    
    }
    float GetDeltaTime() const {
        return m_deltaTime;
    }

    int GetWindowWidth() const {
        return m_windowWidth;
    }

    int GetWindowHeight() const {
        return m_windowHeight;
    }

    // Xóa bỏ Copy Constructor và Assignment Operator để đảm bảo tính ĐỘC NHẤT
    GameEngine(const GameEngine&) = delete;
    void operator=(const GameEngine&) = delete;

private:
    // Private constructor cho Singleton
    GameEngine();
    ~GameEngine();

    // Biến tĩnh lưu trữ thể hiện duy nhất
    static GameEngine* s_Instance;

    // Thời gian giữa các khung hình
    float m_deltaTime;

    // Lưu trữ các trạng thái game
    std::vector<IGameState*> m_States;
    bool m_bRunning;
    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;
    
    // Kích thước cửa sổ để giới hạn di chuyển
    int m_windowWidth;
    int m_windowHeight;

    // Con trỏ đến Player
    Player* m_pPlayer;
};