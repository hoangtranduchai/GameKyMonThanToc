#include "GameEngine.h"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {

    SetConsoleOutputCP(CP_UTF8); // Thiết lập mã hóa UTF-8 cho console
    SetConsoleCP(CP_UTF8); // Thiết lập mã hóa UTF-8 cho console

    GameEngine* game = new GameEngine();
    
    // Khởi tạo cửa sổ 1280 x 720
    if (game->Init("Kỳ Môn Thần Tốc", 100, 100, 1280, 720, false)) { 

        // Vòng lặp chính của game
        while (game->IsRunning()) {
            game->HandleEvents(); // 1. Xử lý Input
            game->Update();       // 2. Cập nhật Logic
            game->Render();       // 3. Vẽ Đồ họa
        }

    }

    // Dọn dẹp tài nguyên và kết thúc game
    game->Quit();
    delete game;
    game = nullptr;
    
    return 0;
}