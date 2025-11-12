// -------------------------------------------------------------------------
// src/GameStateManager.cpp
// Hiện thực hóa file header GameStateManager.h
// -------------------------------------------------------------------------

#include "GameStateManager.h"
#include "IGameState.h"
#include "GameEngine.h"
#include <iostream>

// ----- 1. Hàm khởi tạo -----
GameStateManager::GameStateManager() {
    m_pEngine = NULL;
    // Không cần làm gì với m_States (vector tự khởi tạo)
    std::cout << "GameStateManager constructor called." << std::endl;
}

// ----- 2. Hàm hủy -----
GameStateManager::~GameStateManager() {
    std::cout << "GameStateManager destructor called." << std::endl;
    // CleanUp(); // Sẽ được gọi bởi GameEngine
}

// ----- 3. Hàm Init -----
void GameStateManager::Init(GameEngine* engine) {
    // Lưu lại con trỏ trỏ tới GameEngine
    // để các State có thể gọi các hàm của Engine (vd: engine->Quit())
    m_pEngine = engine;
    std::cout << "GameStateManager Init." << std::endl;
}

// ----- 4. Hàm CleanUp (Rất quan trọng) -----
void GameStateManager::CleanUp() {
    std::cout << "Cleaning up GameStateManager..." << std::endl;
    
    // Dọn dẹp TẤT CẢ các trạng thái còn trong stack
    // Lặp qua vector (stack) từ dưới lên và dọn dẹp
    while (!m_States.empty()) {
        IGameState* state = m_States.back(); // Lấy trạng thái trên cùng
        state->CleanUp();    // 1. Gọi CleanUp() của trạng thái đó
        delete state;        // 2. Xóa con trỏ
        state = NULL;
        m_States.pop_back(); // 3. Xóa nó khỏi stack
    }
    std::cout << "GameStateManager cleanup complete." << std::endl;
}

// ----- 5. Các hàm ủy quyền (Delegate) -----
// Các hàm này chỉ đơn giản là gọi hàm tương ứng của 
// trạng thái đang hoạt động (trạng thái ở trên cùng của stack)

void GameStateManager::HandleEvents() {
    if (!m_States.empty()) {
        m_States.back()->HandleEvents(m_pEngine);
    }
}

void GameStateManager::Update() {
    if (!m_States.empty()) {
        m_States.back()->Update(m_pEngine);
    }
}

void GameStateManager::Render() {
    if (!m_States.empty()) {
        m_States.back()->Render(m_pEngine);
    }
}

// ----- 6. Các hàm quản lý Stack (Cốt lõi) -----

void GameStateManager::ChangeState(IGameState* state) {
    std::cout << "Changing state..." << std::endl;
    // 1. Dọn dẹp tất cả các trạng thái cũ (nếu có)
    CleanUp(); // Xóa sạch stack
    
    // 2. Đẩy trạng thái mới vào
    PushState(state); // Đẩy trạng thái mới vào stack rỗng
}

void GameStateManager::PushState(IGameState* state) {
    std::cout << "Pushing new state..." << std::endl;
    // 1. "Tạm dừng" (Pause) trạng thái hiện tại (nếu có)
    if (!m_States.empty()) {
        m_States.back()->Pause();
    }

    // 2. Đẩy trạng thái mới vào stack
    m_States.push_back(state);
    
    // 3. Khởi tạo (Init) trạng thái mới này
    m_States.back()->Init(m_pEngine);
}

void GameStateManager::PopState() {
    std::cout << "Popping state..." << std::endl;
    if (m_States.empty()) {
        return; // Không có gì để pop
    }

    // 1. Dọn dẹp trạng thái hiện tại (trên cùng)
    IGameState* state = m_States.back();
    state->CleanUp();
    delete state;
    state = NULL;
    m_States.pop_back(); // Xóa khỏi stack

    // 2. "Quay lại" (Resume) trạng thái bên dưới (nếu có)
    if (!m_States.empty()) {
        m_States.back()->Resume();
    }
}