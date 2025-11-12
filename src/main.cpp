// -------------------------------------------------------------------------
// src/main.cpp
// Điểm khởi đầu CỰC KỲ GỌN GÀNG
// -------------------------------------------------------------------------

#include "GameEngine.h" // Chỉ cần biết về GameEngine
#include <iostream>

int main(int argc, char* args[]) {
    
    std::cout << "Starting 'Ky Mon Than Toc'..." << std::endl;

    // 1. Tạo đối tượng GameEngine
    GameEngine* game = new GameEngine();

    // 2. Khởi tạo
    // (Chúng ta sẽ dùng 1024x768, không full screen)
    if (game->Init("Ky Mon Than Toc - PBL2", 
                   SDL_WINDOWPOS_CENTERED, // Căn giữa cửa sổ
                   SDL_WINDOWPOS_CENTERED, 
                   1024, 
                   768, 
                   false)) {
        
        // 3. Chạy vòng lặp game
        game->Run();
    }

    // 4. Dọn dẹp (khi Run() kết thúc)
    // (Hàm Run() sẽ tự gọi CleanUp() bên trong nó)
    delete game; // Xóa đối tượng game
    game = NULL;

    std::cout << "Exiting 'Ky Mon Than Toc'..." << std::endl;
    return 0;
}