#include "GameEngine.h"
#include <string>

// Theo dõi thời gian "chuẩn mực"
static Uint32 s_lastTick = 0;
static float s_deltaTime = 0.0f;

GameEngine::GameEngine() {
    // Khởi tạo mọi con trỏ về nullptr là "chuẩn mực"
    m_pWindow = nullptr;
    m_pRenderer = nullptr;
    m_pPlayerTexture = nullptr;
    m_bRunning = false; // Game chưa chạy

    // Khởi tạo tốc độ và trạng thái input "chuẩn mực"
    m_playerSpeed = 200.0f; // Ví dụ: 200 pixel/giây
    m_movingLeft = false;
    m_movingRight = false;
    m_movingUp = false;
    m_movingDown = false;
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
    // Lưu kích thước cửa sổ để giới hạn di chuyển
    m_windowWidth = w;
    m_windowHeight = h;

    // Tạo trình kết xuất render đồ họa
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer == nullptr) {
        std::cout << "Không thể tạo trình kết xuất! Lỗi SDL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_pWindow);     // Dọn dẹp cửa sổ đã tạo
        return false;
    }

    // Đặt màu vẽ mặc định cho trình kết xuất (Màu đen)
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);

    // Xây dựng đường dẫn tuyệt đối bằng Macro từ CMake
    std::string imagePath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player.png";
    
    m_pPlayerTexture = IMG_LoadTexture(m_pRenderer, imagePath.c_str());
    if (m_pPlayerTexture == nullptr) {
        std::cout << "Không thể tải texture tại: '" << imagePath << "'! Lỗi: " << IMG_GetError() << std::endl;
        // Chúng ta vẫn có thể return true để chạy game dù không có hình ảnh
        // (hoặc return false nếu hình ảnh này là "tối quan trọng")
    }

    // Khởi tạo vị trí và kích thước ban đầu cho nhân vật
    m_playerDestRect.x = 100;
    m_playerDestRect.y = 100;
    // Lấy kích thước từ texture
    SDL_QueryTexture(m_pPlayerTexture, NULL, NULL, &m_playerDestRect.w, &m_playerDestRect.h);

    // Scale (0.2 = giảm còn 20%)
    const float playerScale = 0.2f;
    m_playerDestRect.w = static_cast<int>(m_playerDestRect.w * playerScale);
    m_playerDestRect.h = static_cast<int>(m_playerDestRect.h * playerScale);

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
    m_pWindow = nullptr;
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
        
        // LOGIC XỬ LÝ INPUT
        if (event.type == SDL_KEYDOWN) { // Nếu một phím được nhấn
            switch (event.key.keysym.sym) { // Kiểm tra phím đó là gì
                case SDLK_w : case SDLK_UP: // Phím W, Phím lên
                    m_movingUp = true; // Di chuyển lên
                    break;
                case SDLK_s: case SDLK_DOWN: // Phím S, Phím xuống
                    m_movingDown = true; // Di chuyển xuống
                    break;
                case SDLK_a: case SDLK_LEFT: // Phím A, Phím trái
                    m_movingLeft = true; // Di chuyển sang trái
                    break;
                case SDLK_d: case SDLK_RIGHT: // Phím D, Phím phải
                    m_movingRight = true; // Di chuyển sang phải
                    break;
            }
        }
        else if (event.type == SDL_KEYUP) { // Nếu một phím được thả ra
            switch (event.key.keysym.sym) { // Kiểm tra phím đó là gì
                case SDLK_w : case SDLK_UP: // Phím W, Phím lên
                    m_movingUp = false; // Ngừng di chuyển lên
                    break;
                case SDLK_s: case SDLK_DOWN: // Phím S, Phím xuống
                    m_movingDown = false; // Ngừng di chuyển xuống
                    break;
                case SDLK_a: case SDLK_LEFT: // Phím A, Phím trái
                    m_movingLeft = false; // Ngừng di chuyển sang trái
                    break;
                case SDLK_d: case SDLK_RIGHT: // Phím D, Phím phải
                    m_movingRight = false; // Ngừng di chuyển sang phải
                    break;
            }
        }
    }
}

// Cập nhật logic Game
void GameEngine::Update() {
    // TÍNH TOÁN "TIME-STEP" CHO HIỆU SUẤT TỐI ƯU
    Uint32 currentTick = SDL_GetTicks(); // Lấy thời gian hiện tại (miligiây)
    s_deltaTime = (currentTick - s_lastTick) / 1000.0f; // Thời gian trôi qua (giây)
    s_lastTick = currentTick; // Cập nhật thời gian cho khung hình tiếp theo

    // Đặt giới hạn DeltaTime để tránh lỗi khi pause/debug quá lâu
    if (s_deltaTime > 0.05f) { // Ví dụ: Giới hạn 50ms/frame (20 FPS tối thiểu)
        s_deltaTime = 0.05f;
    }

    // LOGIC DI CHUYỂN
    float moveX = 0;
    float moveY = 0;

    if (m_movingLeft)  moveX -= m_playerSpeed;
    if (m_movingRight) moveX += m_playerSpeed;
    if (m_movingUp)    moveY -= m_playerSpeed;
    if (m_movingDown)  moveY += m_playerSpeed;

    // Chuẩn hóa Vector di chuyển để tốc độ chéo không nhanh hơn (Nếu đang di chuyển chéo)
    if (moveX != 0 && moveY != 0) {
        // return; // Không cho di chuyển chéo
        float length = sqrt(moveX * moveX + moveY * moveY);
        moveX = (moveX / length) * m_playerSpeed;
        moveY = (moveY / length) * m_playerSpeed;
    }

    // Cập nhật vị trí
    m_playerDestRect.x += (int)(moveX * s_deltaTime);
    m_playerDestRect.y += (int)(moveY * s_deltaTime);

    // GIỚI HẠN DI CHUYỂN TRONG CỬA SỔ
    // Giới hạn bên trái
    if (m_playerDestRect.x < 0) {
        m_playerDestRect.x = 0;
    }
    // Giới hạn bên phải
    if (m_playerDestRect.x + m_playerDestRect.w > m_windowWidth) {
        m_playerDestRect.x = m_windowWidth - m_playerDestRect.w;
    }
    // Giới hạn phía trên
    if (m_playerDestRect.y < 0) {
        m_playerDestRect.y = 0;
    }
    // Giới hạn phía dưới
    if (m_playerDestRect.y + m_playerDestRect.h > m_windowHeight) {
        m_playerDestRect.y = m_windowHeight - m_playerDestRect.h;
    }
}

// Render Đồ họa
void GameEngine::Render() {
    // Xóa sạch "bức toan" bằng màu đen
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);
    
    // VẼ "KIỆT TÁC"
    // Vẽ Texture của người chơi tại vị trí m_playerDestRect
    SDL_RenderCopy(m_pRenderer, m_pPlayerTexture, NULL, &m_playerDestRect);
    
    // Hiển thị mọi thứ đã vẽ ra màn hình
    SDL_RenderPresent(m_pRenderer);
}