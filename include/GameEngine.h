#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include "TextureManager.h"
#include "Map.h"
#include <stack>
#include <utility>

// Forward Declaration để tránh lỗi vòng lặp include
class IGameState;
class Player;

// Định nghĩa các giai đoạn của một kiệt tác
enum GameState {
    STATE_MENU,     // Màn hình chờ: Nơi cảm xúc bắt đầu
    STATE_PLAY,     // Màn hình chơi: Nơi trí tuệ thăng hoa
    STATE_PAUSE,    // Màn hình tạm dừng: Nơi tĩnh lặng
    STATE_WIN       // Màn hình thắng: Nơi vinh quang tỏa sáng
};

// Cấu trúc Snapshot: Lưu giữ khoảnh khắc lịch sử
struct GameStateMoment {
    int playerGridRow;       // Vị trí dòng của Player
    int playerGridCol;       // Vị trí cột của Player
    int currentSteps;        // Số bước chân
    int shrinesCollected;    // Số lượng đã thu thập
    std::vector<std::pair<int, int>> visitedShrinesSnapshot;
};

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

    // Player có thể truy cập Map
    Map* GetMap() const {
        return m_pMap;
    }

    // Getter cho UI sau này
    int GetOptimalSteps() const {
        return m_optimalSteps;
    }

    // Xóa bỏ Copy Constructor và Assignment Operator
    GameEngine(const GameEngine&) = delete;
    void operator=(const GameEngine&) = delete;

    // --- CÁC HÀM GAMEPLAY LOGIC ---
    void OnPlayerMove(); 
    void OnShrineVisited(int row, int col); 

    int GetCurrentSteps() const {
        return m_currentSteps;
    }

    int GetShrinesCollected() const {
        return m_shrinesCollected;
    }

    int GetTotalShrines() const {
        return m_totalShrines;
    }

    // --- HỆ THỐNG HỒI TƯỞNG (UNDO) ---
    void SaveState(); 
    void Undo();      

    // Hàm Reset game để chơi lại từ đầu
    void ResetGame();

    // Hàm chuyển đổi trạng thái mượt mà
    void SwitchState(GameState newState);

    // --- HỆ THỐNG LEVEL (KHAI BÁO 1 LẦN DUY NHẤT Ở ĐÂY) ---
    void LoadLevel(int levelIndex);
    void NextLevel();

    // Toggle Admin Mode
    void ToggleAdminMode();

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
    
    // Kích thước cửa sổ
    int m_windowWidth;
    int m_windowHeight;

    // Con trỏ đến Player
    Player* m_pPlayer;

    // Con trỏ đến Map
    Map* m_pMap;

    // Số bước tối ưu
    int m_optimalSteps;

    // Biến Gameplay
    int m_currentSteps;      
    int m_shrinesCollected;  
    int m_totalShrines;      
    
    std::vector<std::pair<int, int>> m_visitedShrinesList;

    // Ngăn xếp lịch sử (Stack)
    std::stack<GameStateMoment> m_historyStack;

    // Biến lưu trạng thái hiện tại
    GameState m_currentState;
    
    // Biến hiệu ứng nhấp nháy
    float m_blinkTimer;

    // --- HỆ THỐNG PARALLAX SKY AAA ---
    float m_cloudScrollX_1; 
    float m_cloudScrollX_2; 
    
    const float CLOUD_SPEED_1 = 15.0f; 
    const float CLOUD_SPEED_2 = 35.0f; 

    // Hàm vẽ HUD
    void DrawHUD();

    // Biến quản lý Fade
    float m_fadeAlpha; 
    bool m_isFadingIn; 
    bool m_isFadingOut; 

    // --- BIẾN QUẢN LÝ CHUYỂN CẢNH AAA ---
    GameState m_nextState; 
    const float FADE_SPEED = 2.0f;

    // --- BIẾN CHO VICTORY FLOW (AAA POLISH) ---
    bool m_isWinningSequence; 
    float m_winDelayTimer;    

    // --- HỆ THỐNG UI AAA ---
    void DrawStylishBox(int x, int y, int w, int h, std::string title = "");
    void DrawButton(std::string text, int x, int y, int w, int h, bool isSelected);

    int m_menuSelection; 
    bool m_isAdminMode;

    // --- HỆ THỐNG LEVEL (BIẾN DỮ LIỆU) ---
    std::vector<std::string> m_levelFiles; 
    int m_currentLevelIdx;                 
};