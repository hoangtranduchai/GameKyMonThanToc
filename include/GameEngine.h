#pragma once // Đảm bảo file này chỉ được include 1 lần

// Chúng ta cần các thư viện SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Đây là "Forward Declaration" (Khai báo trước)
// Nó báo cho GameEngine.h biết "Hey, sẽ có một lớp tên là GameStateManager"
// mà không cần phải #include "GameStateManager.h".
// Kỹ thuật này giúp TRÁNH LỖI BIÊN DỊCH VÒNG TRÒN (circular dependency)
class GameStateManager;

class GameEngine {
private:
    // Các hàm riêng tư, chỉ dùng bên trong GameEngine
    void HandleEvents(); // Xử lý input
    void Update();       // Cập nhật logic
    void Render();       // Vẽ lên màn hình

    // CÁC BIẾN THÀNH VIÊN (Member Variables)
    // Tiền tố 'm_p' = member pointer (biến thành viên là con trỏ)
    // Tiền tố 'm_b' = member boolean (biến thành viên là boolean)
    // Đây là một cách đặt tên code rất "sạch"

    SDL_Window* m_pWindow;     // Cửa sổ game
    SDL_Renderer* m_pRenderer; // Bộ kết xuất đồ họa
    
    GameStateManager* m_pGameStateManager; // Con trỏ tới bộ quản lý trạng thái

    bool m_bIsRunning; // Cờ kiểm soát vòng lặp game

public:
    // Hàm khởi tạo (Constructor)
    GameEngine();
    // Hàm hủy (Destructor)
    ~GameEngine();

    // 1. Khởi tạo toàn bộ hệ thống
    //    Thay vì code cứng 800x600, chúng ta truyền nó vào như tham số
    bool Init(const char* title, int x, int y, int width, int height, bool fullscreen);

    // 2. Chạy vòng lặp game chính
    void Run();

    // 3. Dọn dẹp tài nguyên
    void CleanUp();

    // 4. Các hàm trợ giúp (Getter)
    // Các phần khác (như PlayState) sẽ cần truy cập vào Renderer để vẽ
    SDL_Renderer* GetRenderer() {
        return m_pRenderer;
    }

    bool IsRunning() {
        return m_bIsRunning;
    }
    
    // Hàm để thoát game
    void Quit() {
        m_bIsRunning = false;
    }
};