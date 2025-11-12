#include "PlayState.h"
#include "GameEngine.h" // Cần để gọi engine->Quit()
#include "TextureManager.h" // Cần để tải assets
#include "Map.h" // Cần để tạo bản đồ
#include <iostream>
#include <SDL2/SDL.h>

// Chúng ta sẽ #include "Map.h" sau khi tạo nó

void PlayState::Init(GameEngine* engine) {
    std::cout << "PlayState Init.\n";
    m_pEngine = engine; // Lưu lại con trỏ engine
    
    // --- TẢI TÀI NGUYÊN (ASSETS) ---
    // Chúng ta cần một tileset (bộ gạch) để vẽ bản đồ
    // Giả sử chúng ta có 1 file "tileset.png" trong thư mục "assets/"
    // Và mỗi ô gạch (tile) có kích thước 32x32 pixels
    
    // SỬ DỤNG TEXTURE MANAGER:
    // Tải file ảnh và đặt ID cho nó là "tileset"
    if (!TextureManager::GetInstance()->Load("tileset", "assets/tileset.png", m_pEngine->GetRenderer())) {
        std::cout << "ERROR: Khong the tai file assets/tileset.png" << std::endl;
        // Xử lý lỗi (ví dụ: thoát game)
    }

    // --- KHỞI TẠO BẢN ĐỒ ---
    m_pMap = new Map("tileset", 128); // 128 là tileSize
    // Đọc file map ví dụ 3x6
    m_pMap->Load("assets/maps/level1.txt", 6, 3); // 6 cột, 3 hàng
}

void PlayState::CleanUp() {
    std::cout << "PlayState CleanUp.\n";
    // Dọn dẹp bản đồ
    delete m_pMap;
    m_pMap = nullptr;
    
    // Dọn dẹp texture (đã được GameEngine::CleanUp() gọi)
}

void PlayState::Pause() {
    std::cout << "PlayState Paused.\n";
}

void PlayState::Resume() {
    std::cout << "PlayState Resumed.\n";
}

void PlayState::HandleEvents(GameEngine* engine) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            engine->Quit();
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                engine->Quit(); // Tạm thời thoát game, sau này sẽ là PushState(PauseState)
            }
        }
    }
}

void PlayState::Update(GameEngine* engine) {
    // Cập nhật logic của bản đồ (nếu có)
    // Cập nhật logic của Player (sau này)
}

void PlayState::Render(GameEngine* engine) {
    // Xóa màn hình về màu đen (màu nền chung)
    SDL_SetRenderDrawColor(engine->GetRenderer(), 20, 20, 20, 255);
    SDL_RenderClear(engine->GetRenderer());

    // Yêu cầu bản đồ tự vẽ (Render)
    if (m_pMap != nullptr) {
        m_pMap->Render(engine->GetRenderer());
    }
    
    // Yêu cầu Player tự vẽ (sau này)
}