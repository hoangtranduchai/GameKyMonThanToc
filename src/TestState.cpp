#include "TestState.h"
#include "GameEngine.h"
#include <iostream>
#include <SDL2/SDL.h>

void TestState::Init(GameEngine* engine) {
    std::cout << "TestState Init." << std::endl;
}

void TestState::CleanUp() {
    std::cout << "TestState CleanUp." << std::endl;
}

void TestState::Pause() {

}

void TestState::Resume() {

}

void TestState::HandleEvents(GameEngine* engine) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            engine->Quit(); // Gọi hàm Quit của Engine
        }
        // Thoát bằng phím ESC
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            engine->Quit();
        }
    }
}

void TestState::Update(GameEngine* engine) {
    // Không làm gì
}

void TestState::Render(GameEngine* engine) {
    // Chỉ cần vẽ một màu xanh lá để biết nó hoạt động
    SDL_SetRenderDrawColor(engine->GetRenderer(), 0, 100, 0, 255); // Xanh lá
    SDL_RenderClear(engine->GetRenderer());
}