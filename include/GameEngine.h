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
    STATE_WIN       // Màn hình thắng: Nơi vinh quang tỏa sáng
};

// Cấu trúc Snapshot: Lưu giữ khoảnh khắc lịch sử
// Được tối ưu hóa bộ nhớ, chỉ lưu những gì cần thiết
struct GameStateMoment {
    int playerGridRow;       // Vị trí dòng của Player
    int playerGridCol;       // Vị trí cột của Player
    int currentSteps;        // Số bước chân
    int shrinesCollected;    // Số lượng đã thu thập
    
    // Danh sách tọa độ các Trận Nhãn ĐÃ MỞ tại thời điểm này
    // Dùng vector nhỏ (N <= 15) nên copy rất nhanh, an toàn hơn con trỏ
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

    // Xóa bỏ Copy Constructor và Assignment Operator để đảm bảo tính ĐỘC NHẤT
    GameEngine(const GameEngine&) = delete;
    void operator=(const GameEngine&) = delete;

    // --- CÁC HÀM GAMEPLAY LOGIC ---
    void OnPlayerMove(); // Gọi khi nhân vật di chuyển
    void OnShrineVisited(int row, int col); // Gọi khi nhân vật đến Trận Nhãn

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
    void SaveState(); // Chụp ảnh trạng thái hiện tại
    void Undo();      // Quay ngược thời gian

    // Hàm Reset game để chơi lại từ đầu
    void ResetGame();

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

    // Con trỏ đến Map
    Map* m_pMap;

    // Số bước tối ưu (Từ Thiên Cơ) để hiển thị trên UI
    int m_optimalSteps;

    // Biến Gameplay
    int m_currentSteps;      // Số bước người chơi đã đi
    int m_shrinesCollected;  // Số Trận Nhãn đã mở
    int m_totalShrines;      // Tổng số Trận Nhãn cần mở
    
    // Danh sách các vị trí Trận Nhãn đã mở (để không tính điểm 2 lần)
    std::vector<std::pair<int, int>> m_visitedShrinesList;

    // Ngăn xếp lịch sử (Stack)
    std::stack<GameStateMoment> m_historyStack;

    // Biến lưu trạng thái hiện tại
    GameState m_currentState;
    
    // Biến hiệu ứng nhấp nháy cho text "Press Enter"
    float m_blinkTimer;

    // --- HỆ THỐNG PARALLAX SKY AAA ---
    float m_cloudScrollX_1; // Lớp mây xa (chậm)
    float m_cloudScrollX_2; // Lớp mây gần (nhanh)
    
    // Tốc độ trôi
    const float CLOUD_SPEED_1 = 20.0f; 
    const float CLOUD_SPEED_2 = 50.0f;
};