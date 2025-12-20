#include "Core/GameEngine.h"
#include "Core/Config.h"
#include <iostream>
#include <stdexcept>
#include <windows.h>

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

int main(int argc, char* argv[]) {
    // Cấu hình Console Tiếng Việt (UTF-8) để debug
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "==================================================" << std::endl;
    std::cout << "[HỆ THỐNG] Đang khởi động Game KY MON THAN TOC..." << std::endl;
    std::cout << "[HỆ THỐNG] Tốc độ khung hình mục tiêu: " << TARGET_FPS << " FPS" << std::endl;
    std::cout << "==================================================" << std::endl;

    Uint32 frameStartTimestamp;
    int frameDuration;

    GameEngine* pGameEngine = GameEngine::GetInstance();

    try {
        if (!pGameEngine->Init("Ky Mon Than Toc", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, true)) {
            throw std::runtime_error("Khởi tạo Game thất bại!");
        }

        std::cout << "[HỆ THỐNG] Khởi tạo Game thành công. Đang bắt đầu vòng lặp game..." << std::endl;

        while (pGameEngine->IsRunning()) {
            frameStartTimestamp = SDL_GetTicks();

            pGameEngine->HandleEvents();
            pGameEngine->Update();
            pGameEngine->Render();

            frameDuration = SDL_GetTicks() - frameStartTimestamp;
            if (FRAME_DELAY > frameDuration) {
                SDL_Delay(FRAME_DELAY - frameDuration);
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Ngoại lệ chưa được xử lý: " << e.what() << std::endl;
        GameEngine::DestroyInstance();
        return -1;
    } catch (...) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Đã xảy ra ngoại lệ không xác định!" << std::endl;
        GameEngine::DestroyInstance();
        return -1;
    }

    std::cout << "[HỆ THỐNG] Đang tắt chương trình..." << std::endl;
    GameEngine::DestroyInstance();
    std::cout << "[HỆ THỐNG] Tạm biệt!" << std::endl;

    return 0;
}