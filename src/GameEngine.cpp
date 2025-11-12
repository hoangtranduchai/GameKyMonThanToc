// -------------------------------------------------------------------------
// src/GameEngine.cpp
// Hiện thực hóa file header GameEngine.h
// -------------------------------------------------------------------------

#include "GameEngine.h"       // Quan trọng! Phải include header tương ứng
#include "GameStateManager.h" // Cần biết GameStateManager để tạo nó
#include "TextureManager.h"   // Cần biết TextureManager để load texture
#include "PlayState.h"        // Cần biết PlayState để khởi tạo trạng thái ban đầu
#include <iostream>

// ----- 1. Hàm khởi tạo (Constructor) -----
GameEngine::GameEngine() {
    // Đặt tất cả các con trỏ về NULL
    // Đây là thói quen code "sạch" để tránh lỗi "con trỏ hoang"
    m_pWindow = NULL;
    m_pRenderer = NULL;
    m_pGameStateManager = NULL;
    m_bIsRunning = false;
    std::cout << "GameEngine constructor called." << std::endl;
}

// ----- 2. Hàm hủy (Destructor) -----
GameEngine::~GameEngine() {
    // Hàm hủy sẽ gọi CleanUp để đảm bảo mọi thứ được dọn dẹp
    // (Mặc dù CleanUp() thường được gọi riêng ở cuối hàm Run())
    std::cout << "GameEngine destructor called." << std::endl;
    // CleanUp(); // Chúng ta sẽ gọi CleanUp thủ công
}

// ----- 3. Hàm Khởi tạo (Init) -----
bool GameEngine::Init(const char* title, int x, int y, int width, int height, bool fullscreen) {
    // Đặt cờ m_bIsRunning = true
    m_bIsRunning = true;

    // --- 1. Khởi tạo SDL ---
    // Yêu cầu khởi tạo Video, Âm thanh và Timer
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cout << "SDL Init failed! ERROR: " << SDL_GetError() << std::endl;
        m_bIsRunning = false;
        return false;
    }

    // --- 2. Tạo Cửa sổ (Window) ---
    int windowFlags = SDL_WINDOW_SHOWN; // Cờ mặc định
    if (fullscreen) {
        windowFlags = SDL_WINDOW_FULLSCREEN;
    }

    m_pWindow = SDL_CreateWindow(title, x, y, width, height, windowFlags);
    if (m_pWindow == NULL) {
        std::cout << "Window creation failed! ERROR: " << SDL_GetError() << std::endl;
        m_bIsRunning = false;
        return false;
    }

    // --- 3. Tạo Bộ kết xuất (Renderer) ---
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer == NULL) {
        std::cout << "Renderer creation failed! ERROR: " << SDL_GetError() << std::endl;
        m_bIsRunning = false;
        return false;
    }
    // Đặt màu vẽ mặc định là màu đen
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);

    // --- 4. Khởi tạo các thư viện phụ (Image, TTF, Mixer) ---
    // (Phần này sẽ được thêm vào sau, nhưng Init của SDL_image là cần thiết ngay)
    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "SDL_image Init failed! ERROR: " << IMG_GetError() << std::endl;
        m_bIsRunning = false;
        return false;
    }

    // --- 5. Khởi tạo Game State Manager ---
    m_pGameStateManager = new GameStateManager();
    m_pGameStateManager->Init(this); // "this" là con trỏ trỏ tới chính GameEngine này

    // --- 6. Đặt Trạng thái ban đầu (PlayState) ---
    // Đây là bước quan trọng để cỗ máy bắt đầu chạy
    m_pGameStateManager->ChangeState(new PlayState());

    std::cout << "GameEngine Init successful." << std::endl;
    return true; // Khởi tạo thành công!
}

// ----- 4. Hàm Chạy Vòng lặp (Run) -----
void GameEngine::Run() {
    const int FPS = 60; // 60 khung hình/giây
    const int FRAME_DELAY = 1000 / FPS; // Thời gian cho mỗi khung hình (khoảng 16.6ms)

    Uint32 frameStart;
    int frameTime;

    std::cout << "GameEngine is now running..." << std::endl;
    while (m_bIsRunning) {
        frameStart = SDL_GetTicks(); // Lấy thời gian bắt đầu frame

        // 1. Xử lý Input
        HandleEvents();
        
        // 2. Cập nhật Logic
        Update();
        
        // 3. Vẽ lên màn hình
        Render();

        frameTime = SDL_GetTicks() - frameStart; // Thời gian thực thi frame

        // Giới hạn FPS
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime); // Chờ cho đủ 16.6ms
        }
    }

    // Khi vòng lặp kết thúc, dọn dẹp
    CleanUp();
}

// ----- 5. Dọn dẹp (CleanUp) -----
void GameEngine::CleanUp() {
    std::cout << "Cleaning up GameEngine..." << std::endl;
    
    // Dọn dẹp theo thứ tự ngược lại với lúc tạo
    m_pGameStateManager->CleanUp(); // Dọn dẹp tất cả các trạng thái
    delete m_pGameStateManager;     // Xóa StateManager
    m_pGameStateManager = NULL;

    TextureManager::GetInstance()->Clean(); // Dọn dẹp toàn bộ texture

    IMG_Quit(); // Tắt hệ thống SDL_image
    
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = NULL;
    m_pRenderer = NULL;

    SDL_Quit(); // Tắt toàn bộ SDL
    std::cout << "GameEngine cleanup complete." << std::endl;
}

// ----- 6. Các hàm vòng lặp riêng -----
// Các hàm này giờ đây sẽ "ủy quyền" (delegate)
// công việc cho GameStateManager

void GameEngine::HandleEvents() {
    m_pGameStateManager->HandleEvents();
}

void GameEngine::Update() {
    m_pGameStateManager->Update();
}

void GameEngine::Render() {
    // Xóa màn hình về màu đen
    // (Chúng ta có thể chuyển cái này vào PlayState sau)
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);

    // Yêu cầu StateManager vẽ trạng thái hiện tại
    m_pGameStateManager->Render();

    // Hiển thị ra màn hình
    SDL_RenderPresent(m_pRenderer);
}