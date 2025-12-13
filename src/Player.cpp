#include "Player.h"
#include "GameEngine.h"

Player::Player(const LoaderParams* pParams) : GameObject(pParams) {
    // Giải nén tham số từ LoaderParams vào biến thành viên
    m_x = pParams->m_x;
    m_y = pParams->m_y;

    // Lưu vị trí chính xác để tính toán số thực
    m_exactX = (float)m_x;
    m_exactY = (float)m_y;

    m_width = pParams->m_width;
    m_height = pParams->m_height;
    m_textureID = pParams->m_textureID;
    m_currentRow = 1;
    m_currentFrame = 0;
    m_flip = pParams->m_flip;

    // Thiết lập tốc độ
    m_moveSpeed = 200.0f; // Ví dụ: 200 pixel/giây
}

void Player::Draw() {
    // Sử dụng TextureManager để vẽ chính mình
    // TextureManager::GetInstance()->DrawFrame(m_textureID, m_x, m_y, m_width, m_height, m_currentRow, m_currentFrame, GameEngine::GetInstance()->GetRenderer(), m_flip);
    TextureManager::GetInstance()->Draw(m_textureID, m_x, m_y, m_width, m_height, GameEngine::GetInstance()->GetRenderer(), m_flip);
}

void Player::HandleInput() {
    // Cách xử lý Input "Mượt mà" hơn: Dùng Keystates trực tiếp thay vì Event Polling
    // Điều này giúp di chuyển trơn tru, không bị khựng
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    float moveX = 0;
    float moveY = 0;

    if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP]) {
        moveY = -1;
    }
    if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN]) {
        moveY = 1;
    }
    if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
        moveX = -1;
        m_flip = SDL_FLIP_HORIZONTAL; // Lật hình khi đi sang trái
    }
    if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
        moveX = 1;
        m_flip = SDL_FLIP_NONE;
    }

    // Chuẩn hóa vector (tránh đi chéo nhanh hơn)
    if (moveX != 0 || moveY != 0) {
        float length = sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;

        // Lấy DeltaTime từ Engine
        float dt = GameEngine::GetInstance()->GetDeltaTime();

        // Cập nhật vị trí chính xác
        m_exactX += moveX * m_moveSpeed * dt;
        m_exactY += moveY * m_moveSpeed * dt;
    }
}

void Player::Update() {
    HandleInput(); // Xử lý di chuyển

    // Cập nhật vị trí hiển thị (int) từ vị trí chính xác (float)
    m_x = (int)m_exactX;
    m_y = (int)m_exactY;

    // Giới hạn biên (Lấy kích thước cửa sổ từ Engine)
    int winW = GameEngine::GetInstance()->GetWindowWidth();
    int winH = GameEngine::GetInstance()->GetWindowHeight();

    if (m_x < 0) { m_x = 0; m_exactX = 0; }
    if (m_x + m_width > winW) { m_x = winW - m_width; m_exactX = (float)m_x; }
    if (m_y < 0) { m_y = 0; m_exactY = 0; }
    if (m_y + m_height > winH) { m_y = winH - m_height; m_exactY = (float)m_y; }
    
    // Animation đơn giản (nếu có sprite sheet)
    // m_currentFrame = int(((SDL_GetTicks() / 100) % 6)); 
}

void Player::Clean() {
    // Dọn dẹp riêng của Player nếu có (hiện tại chưa cần)
}