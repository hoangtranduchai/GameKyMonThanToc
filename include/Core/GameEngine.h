#pragma once

// THƯ VIỆN CHUẨN & SDL
#include <SDL.h>
#include <vector>
#include <stack>
#include <string>

// CÁC MODULE HỆ THỐNG
#include "Graphics/TextureManager.h"

// Khai báo tiền (Forward Declaration) để tránh vòng lặp include
class Player;
class ThienCoEngine;
class Map;

// TRẠNG THÁI GAME (GAME STATES)
enum GameState {
    STATE_INTRO,            // Màn hình giới thiệu
    STATE_STORY_NAME,       // Màn hình hiển thị tên màn chơi
    STATE_STORY_SENSEI,     // Màn hình hiển thị lời dạy của thầy
    STATE_PLAY,             // Trạng thái chơi game
    STATE_RESULT_S,         // Màn hình kết quả thắng rank S
    STATE_RESULT_A,         // Màn hình kết quả thắng rank A
    STATE_RESULT_LOSE,      // Màn hình kết quả thua
    STATE_CONTINUE          // Màn hình tiếp tục sau vòng 3 (chuyển sang vòng 4+)
};

// SNAPSHOT (DÙNG CHO TÍNH NĂNG HOÀN TÁC - UNDO)
// Lưu lại trạng thái của nhân vật và bản đồ tại một thời điểm
struct GameStateMoment {
    int playerGridRow;       // Vị trí dòng của nhân vật trên lưới (đơn vị: ô lưới)
    int playerGridCol;       // Vị trí cột của nhân vật trên lười (đơn vị: ô lười)
    int currentSteps;        // Số bước đi hiện tại
    int shrinesCollected;    // Số trận nhãn đã thu thập
    int currentLevelIdx;     // Chỉ số level hiện tại (để khôi phục đúng level khi undo)
    
    // Danh sách tọa độ các trận nhãn đã được kích hoạt tại thời điểm này
    std::vector<std::pair<int, int>> visitedShrinesSnapshot;
};

// CLASS: GameEngine (MẪU THIẾT KẾ SINGLETON)
class GameEngine {
public:
    // Đảm bảo chỉ có duy nhất một thể hiện của GameEngine tồn tại trong toàn chương trình
    static GameEngine* GetInstance();
    static void DestroyInstance();

    // VÒNG ĐỜI ỨNG DỤNG (APPLICATION LIFECYCLE)
    
    // Khởi tạo toàn bộ hệ thống (SDL, Cửa sổ, Bộ kết xuất, Tài nguyên)
    // Trả về true nếu thành công, false nếu thất bại
    bool Init(const char* title, int xPos, int yPos, int width, int height, bool fullscreen);

    // Xử lý sự kiện đầu vào (Bàn phím, Sự kiện cửa sổ)
    void HandleEvents();

    // Cập nhật logic game (AI, Vật lý, Hoạt ảnh, Chuyển đổi trạng thái)
    void Update();

    // Vẽ toàn bộ khung hình hiện tại lên màn hình
    void Render();

    // Dọn dẹp tài nguyên trước khi thoát (Giải phóng bộ nhớ)
    void Clean();

    // Kiểm tra game còn chạy không (để duy trì vòng lặp chính)
    bool IsRunning() const { return m_bRunning; }

    // HỆ THỐNG TRUY CẬP (GETTERS)
    
    SDL_Renderer* GetRenderer() const { return m_pRenderer; }

    // Thời gian giữa 2 khung hình (Delta Time) - Dùng cho hoạt ảnh
    float GetDeltaTime() const { return m_deltaTime; }

    // Truy cập các đối tượng game
    Map* GetMap() const { return m_pMap; }
    Player* GetPlayer() const { return m_pPlayer; }

    // ĐIỀU KHIỂN LOGIC TRÒ CHƠI (GAMEPLAY LOGIC CONTROL)

    // Chuyển đổi trạng thái game (có lưu lịch sử để hoàn tác, có hiệu ứng chuyển cảnh)
    void SwitchState(GameState newState);

    // Hệ thống quản lý cấp độ (Level Management)
    void LoadLevel(int levelIndex); // Tải màn chơi theo chỉ số
    void NextLevel();               // Chuyển sang màn kế tiếp

    // Các tính năng trò chơi (Gameplay Features)
    void SaveState();   // Lưu trạng thái hiện tại vào ngăn xếp (Stack)
    void Undo();        // Xử lý hoàn tác tổng hợp
    void ToggleAudio(); // Bật/tắt âm thanh

    // Xử lý sự kiện gameplay
    void OnPlayerMove();                    // Gọi khi nhân vật di chuyển
    void OnShrineVisited(int row, int col); // Gọi khi nhân vật thu thập trận nhãn

    // QUẢN LÝ THÔNG SỐ TRÒ CHƠI (GAMEPLAY STATISTICS)
    int GetCurrentSteps() const { return m_currentSteps; }
    int GetOptimalSteps() const { return m_optimalSteps; }
    int GetShrinesCollected() const { return m_shrinesCollected; }
    int GetTotalShrines() const { return m_totalShrines; }

private:
    // Hàm khởi tạo riêng tư (Mẫu thiết kế Singleton)
    GameEngine();
    ~GameEngine();

    // Biến tĩnh lưu trữ thể hiện duy nhất
    static GameEngine* s_Instance;

    // CÁC THÀNH PHẦN CỐT LÕI (CORE COMPONENTS)
    bool m_bRunning;                // Cờ kiểm soát vòng lặp game
    SDL_Window* m_pWindow;          // Cửa sổ ứng dụng
    SDL_Renderer* m_pRenderer;      // Bộ kết xuất đồ họa
    
    int m_windowWidth;              // Chiều rộng cửa sổ
    int m_windowHeight;             // Chiều cao cửa sổ

    float m_deltaTime;              // Thời gian trôi qua giữa khung hình trước và khung hình hiện tại (giây)
    Uint64 m_lastTime;              // Mốc thời gian của khung hình trước
    Uint64 m_performanceFrequency;  // Tần số bộ đếm hiệu suất CPU

    // CÁC ĐỐI TƯỢNG GAME (GAME OBJECTS)
    Player* m_pPlayer;              // Nhân vật chính
    Map* m_pMap;                    // Bản đồ (Tilemap)

    // QUẢN LÝ CẤP ĐỘ (LEVEL MANAGEMENT)
    int m_currentLevelIdx;                  // Chỉ số cấp độ hiện tại (bắt đầu từ 1, 2, 3...)
    std::vector<std::string> m_levelFiles;  // Danh sách đường dẫn file bản đồ

    int m_currentSteps;             // Số bước người chơi đã đi
    int m_optimalSteps;             // Số bước tối ưu (tính bằng thuật toán AI ThienCoEngine)
    int m_shrinesCollected;         // Số trận nhãn đã thu thập
    int m_totalShrines;             // Tổng số trận nhãn trong bản đồ
    int m_resultLevelIdx;           // Lưu level dùng để hiển thị màn hình kết quả
    std::vector<std::pair<int, int>> m_visitedShrinesList; // Danh sách tọa độ trận nhãn đã thu thập

    // Ngăn xếp hoàn tác cho gameplay (Lưu các bước đi)
    std::stack<GameStateMoment> m_historyStack;
    
    // Ngăn xếp hoàn tác cho trạng thái (Lưu các màn hình đã qua)
    std::stack<GameState> m_stateHistory;

    // Lưu trạng thái game trước khi rời khỏi STATE_PLAY
    // Dùng để quay lại game khi nhấn U từ màn hình INTRO/RESULT
    bool m_hasPreviousGameState = false;
    GameStateMoment m_previousGameState;

    // QUẢN LÝ TRẠNG THÁI (STATE MANAGEMENT)
    GameState m_currentState; // Trạng thái hiện tại
    GameState m_nextState;  // Trạng thái chờ chuyển đến
    
    // HIỆU ỨNG HÌNH ẢNH (VISUAL EFFECTS)

    // Hiệu ứng chuyển cảnh (Transition - mờ dần)
    float m_fadeAlpha;      // Độ mờ của lớp phủ màu đen (0.0 -> 255.0)
    bool m_isFadingIn;      // Đang sáng dần lên?
    bool m_isFadingOut;     // Đang tối dần đi?
    const float FADE_SPEED = 2.5f; // Tốc độ hiệu ứng chuyển cảnh

    // Chuỗi hiệu ứng chiến thắng (Victory Sequence)
    bool m_isWinningSequence; 
    float m_winDelayTimer;

    // Trạng thái âm thanh
    bool m_isMuted = true;
    
    // Biến lưu ID hình ảnh ngẫu nhiên cho các vòng sau vòng 3
    std::string m_randomRankImageID;

    // Cờ chờ tăng level (dùng để tránh hiển thị sai ảnh trong quá trình fade)
    bool m_isPendingNextLevel = false;

    // CÁC HÀM TRỢ GIÚP (HELPER FUNCTIONS)
    // Các hàm vẽ nội bộ
    void DrawBackground();  // Vẽ nền
    std::string GetCurrentStoryImageID(const std::string& suffix) const; // Hàm lấy ID hình ảnh theo cấp độ
    void HandleStoryInput(SDL_Event& event); // Xử lý phím ENTER chung cho các màn hình cốt truyện
};