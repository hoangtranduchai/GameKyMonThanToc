#include "GameEngine.h"
#include <string>

GameEngine::GameEngine() {
    // Khởi tạo mọi con trỏ về nullptr là "chuẩn mực"
    m_pWindow = nullptr;
    m_pRenderer = nullptr;
    m_pPlayerTexture = nullptr;
    m_bRunning = false; // Game chưa chạy
}

// Hàm hủy sẽ tự động gọi Quit khi đối tượng bị xóa
GameEngine::~GameEngine() {
    // Chúng ta có thể để trống hoặc gọi Quit()
    // Tốt hơn là gọi Quit() từ main.cpp để kiểm soát rõ ràng
}

// Khởi tạo tất cả hệ thống
bool GameEngine::Init(const char* title, int x, int y, int w, int h, bool fullscreen) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL không thể khởi tạo! Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_image
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cout << "SDL_image không thể khởi tạo! Lỗi: " << IMG_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf không thể khởi tạo! Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer không thể khởi tạo! Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tạo cửa sổ game
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }
    m_pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if (m_pWindow == nullptr) {
        std::cout << "Không thể tạo cửa sổ! Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tạo trình kết xuất (Bộ phận render đồ họa mạnh mẽ)
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer == nullptr) {
        std::cout << "Không thể tạo trình kết xuất! Lỗi SDL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_pWindow);     // Dọn dẹp cửa sổ đã tạo
        return false;
    }

    // Đặt màu vẽ mặc định cho trình kết xuất (Màu đen)
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);

    // Tải "Kết cấu" AAA đầu tiên
    // Xây dựng đường dẫn "tuyệt đối, không tì vết" bằng Macro từ CMake
    std::string imagePath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player.png";
    
    m_pPlayerTexture = IMG_LoadTexture(m_pRenderer, imagePath.c_str());
    if (m_pPlayerTexture == nullptr) {
        std::cout << "Không thể tải texture tại: '" << imagePath << "'! Lỗi: " << IMG_GetError() << std::endl;
        // Chúng ta vẫn có thể return true để chạy game dù không có hình ảnh
        // (hoặc return false nếu hình ảnh này là "tối quan trọng")
    }

    std::cout << "Game Engine khởi tạo thành công!" << std::endl;
    m_bRunning = true;
    return true;
}

// Dọn dẹp Tài nguyên
void GameEngine::Quit() {
    std::cout << "Game Engine đang dọn dẹp và kết thúc..." << std::endl;

    // Dọn dẹp "Kết cấu" (Texture)
    SDL_DestroyTexture(m_pPlayerTexture);
    m_pPlayerTexture = nullptr;

    // Dọn dẹp "Cây cọ" (Renderer) và "Bức toan" (Window)
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr; // "Chuẩn mực" là đặt lại con trỏ
    m_pRenderer = nullptr;

    // Dọn dẹp các thư viện
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    std::cout << "Game Engine đã dọn dẹp và kết thúc!" << std::endl;
}

// Xử lý sự kiện
void GameEngine::HandleEvents() {
    SDL_Event event;
    // Xử lý tất cả các sự kiện trong hàng đợi
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_bRunning = false;
        }
        // TODO: Xử lý Input chi tiết hơn (keyboard, mouse) sau
    }
}

// Cập nhật logic Game
void GameEngine::Update() {
    // TODO: Triển khai logic Update của IGameState hiện tại
}

// Render Đồ họa
void GameEngine::Render() {
    // Xóa sạch "bức toan" bằng màu đen
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);
    
    // ---- BẮT ĐẦU VẼ "ĐỒ HỌA AAA" ----
    
    // 1. Tạo một hình chữ nhật (Rect) để xác định vị trí và kích thước
    SDL_Rect destRect;
    destRect.x = 100; // Vị trí X (từ trái)
    destRect.y = 100; // Vị trí Y (từ trên)
    
    // 2. Lấy kích thước "chuẩn mực" của Texture
    SDL_QueryTexture(m_pPlayerTexture, NULL, NULL, &destRect.w, &destRect.h);
    
    // 3. Sao chép (vẽ) Texture lên Renderer
    SDL_RenderCopy(m_pRenderer, m_pPlayerTexture, NULL, &destRect);

    // ---- KẾT THÚC VẼ ----

    // Hiển thị mọi thứ đã vẽ ra màn hình
    SDL_RenderPresent(m_pRenderer);
}