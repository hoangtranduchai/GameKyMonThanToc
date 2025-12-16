/**
 * ============================================================================
 * PROJECT: KY MON THAN TOC (THE MYSTIC LABYRINTH)
 * MODULE:  Main Entry Point
 * AUTHOR:  Kien Truc Su Truong & Gemini AI
 * TYPE:    AAA Game Core Implementation
 * ============================================================================
 * * Mô tả:
 * Đây là điểm khởi chạy chính của ứng dụng (Entry Point).
 * Nhiệm vụ:
 * 1. Thiết lập môi trường hệ thống (Console UTF-8).
 * 2. Khởi tạo Singleton Game Engine.
 * 3. Quản lý Vòng lặp Game (Game Loop) với cơ chế khóa FPS (Frame Capping).
 * 4. Dọn dẹp tài nguyên hệ thống khi thoát.
 */

#include "GameEngine.h"
#include <iostream>
#include <windows.h> // Thư viện Windows API để xử lý Console Output

// --- CẤU HÌNH KHỞI TẠO (INITIAL CONFIGURATION) ---

// Độ phân giải thiết kế (Logical Resolution) - Chuẩn HD 720p
// Game sẽ tự động Scale (phóng to) lên 1080p hoặc 4K dựa trên màn hình người chơi.
const int DESIGN_WIDTH = 1280;
const int DESIGN_HEIGHT = 720;

// Mục tiêu tốc độ khung hình (Target FPS)
// 60 FPS là chuẩn vàng cho trải nghiệm mượt mà của game 2D.
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS; // ~16ms mỗi khung hình

// Tiêu đề cửa sổ ứng dụng
const char* GAME_TITLE = "Ky Mon Than Toc - Thien Co Engine [AAA Edition]";

int main(int argc, char* argv[]) {
    // -------------------------------------------------------------------------
    // BƯỚC 1: THIẾT LẬP MÔI TRƯỜNG DEBUG (CONSOLE SETUP)
    // -------------------------------------------------------------------------
    // Cấu hình Console để hiển thị tiếng Việt có dấu (UTF-8) phục vụ Debug log.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "==================================================" << std::endl;
    std::cout << ">>> KHOI DONG ENGINE: " << GAME_TITLE << " <<<" << std::endl;
    std::cout << "==================================================" << std::endl;

    // -------------------------------------------------------------------------
    // BƯỚC 2: KHỞI TẠO ENGINE (ENGINE INITIALIZATION)
    // -------------------------------------------------------------------------
    // Sử dụng Pattern Singleton để lấy thể hiện duy nhất của GameEngine.
    GameEngine* pGameEngine = GameEngine::GetInstance();

    // Gọi hàm Init để nạp SDL2, Texture, Audio và cấu hình Window/Renderer.
    // Tham số cuối cùng 'true' kích hoạt chế độ Fullscreen Borderless (Chuẩn AAA).
    bool initResult = pGameEngine->Init(
        GAME_TITLE,                 // Tên cửa sổ
        SDL_WINDOWPOS_CENTERED,     // Vị trí X (Giữa màn hình)
        SDL_WINDOWPOS_CENTERED,     // Vị trí Y (Giữa màn hình)
        DESIGN_WIDTH,               // Chiều rộng thiết kế
        DESIGN_HEIGHT,              // Chiều cao thiết kế
        true                        // Fullscreen Mode: BẬT
    );

    if (!initResult) {
        std::cerr << "[CRITICAL ERROR] Khong the khoi tao Game Engine!" << std::endl;
        std::cerr << "Vui long kiem tra log va thu muc assets." << std::endl;
        return -1; // Trả về mã lỗi cho hệ điều hành
    }

    std::cout << "[INFO] Engine khoi tao thanh cong. Bat dau Game Loop..." << std::endl;

    // -------------------------------------------------------------------------
    // BƯỚC 3: VÒNG LẶP TRÒ CHƠI (THE GAME LOOP)
    // -------------------------------------------------------------------------
    // Đây là trái tim của game, chạy liên tục 60 lần/giây.
    
    Uint32 frameStart; // Thời điểm bắt đầu khung hình (ms)
    int frameTime;     // Thời gian xử lý khung hình (ms)

    while (pGameEngine->IsRunning()) {
        // A. Bắt đầu tính giờ khung hình hiện tại
        frameStart = SDL_GetTicks();

        // B. Xử lý Sự kiện đầu vào (Input Handling)
        // Nhận phím bấm, chuột, sự kiện cửa sổ...
        pGameEngine->HandleEvents();

        // C. Cập nhật Logic Game (Update Physics/AI)
        // Di chuyển nhân vật, tính toán va chạm, cập nhật trạng thái UI...
        pGameEngine->Update();

        // D. Vẽ Đồ họa (Rendering)
        // Xóa màn hình -> Vẽ Map -> Vẽ Player -> Vẽ UI -> Hiển thị (Swap Buffer)
        pGameEngine->Render();

        // E. Ổn định Khung hình (Frame Capping / Throttling)
        // Tính thời gian CPU đã tiêu tốn cho khung hình này
        frameTime = SDL_GetTicks() - frameStart;

        // Nếu xử lý quá nhanh (< 16ms), cho CPU ngủ để tiết kiệm pin và ổn định FPS
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // -------------------------------------------------------------------------
    // BƯỚC 4: DỌN DẸP TÀI NGUYÊN (CLEANUP & SHUTDOWN)
    // -------------------------------------------------------------------------
    std::cout << "[INFO] Dang dong Engine..." << std::endl;
    
    // Hủy instance, giải phóng bộ nhớ texture, âm thanh, SDL context.
    GameEngine::DestroyInstance();

    std::cout << ">>> GAME KET THUC AN TOAN (EXIT SUCCESS) <<<" << std::endl;
    
    return 0;
}