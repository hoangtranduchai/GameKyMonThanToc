#include "GameEngine.h"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {

    SetConsoleOutputCP(CP_UTF8); // Thiết lập mã hóa UTF-8 cho console
    SetConsoleCP(CP_UTF8); // Thiết lập mã hóa UTF-8 cho console

    // TRIỆU HỒI SINGLETON INSTANCE
    // Không dùng "new GameEngine()", mà dùng "GameEngine::GetInstance()"
    // Điều này đảm bảo tính duy nhất và toàn vẹn của hệ thống.

    GameEngine* engine = GameEngine::GetInstance();
    
    // Khởi tạo cửa sổ 1280 x 720
    if (engine->Init("Kỳ Môn Thần Tốc", 100, 100, 1280, 720, false)) { 

        // Vòng lặp chính của game
        while (engine->IsRunning()) {
            engine->HandleEvents(); // 1. Xử lý Input
            engine->Update();       // 2. Cập nhật Logic
            engine->Render();       // 3. Vẽ Đồ họa
        }

    } else {
        std::cout << "Khoi tao Game Engine that bai!" << std::endl;
        return -1;
    }

    // Dọn dẹp tài nguyên và kết thúc game
    engine->Quit();

    // Hủy Instance của GameEngine
    GameEngine::DestroyInstance();
    
    return 0;
}