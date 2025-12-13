#include "Player.h"
#include "GameEngine.h"
#include "Map.h"
#include "Camera.h"

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

    if (moveX == 0 && moveY == 0) return;

    // Chuẩn hóa vector
    float length = sqrt(moveX * moveX + moveY * moveY);
    moveX /= length;
    moveY /= length;

    float dt = GameEngine::GetInstance()->GetDeltaTime();
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int tileSize = pMap->GetTileSize();

    // --- KỸ THUẬT CHECK VA CHẠM AAA (Sliding) ---
    // Kiểm tra trục X trước
    float nextX = m_exactX + moveX * m_moveSpeed * dt;
    // Kiểm tra 4 góc hoặc đơn giản là tâm + bán kính biên
    // Ở đây ta check tâm để đơn giản hóa nhưng tách trục
    int colX = (int)((nextX + m_width / 2) / tileSize);
    int rowCurr = (int)((m_exactY + m_height / 2) / tileSize);

    if (pMap->GetTileID(rowCurr, colX) != 1) { // 1 là Núi
        m_exactX = nextX; // Không va chạm X -> Di chuyển X
    }

    // Kiểm tra trục Y sau (độc lập với X)
    float nextY = m_exactY + moveY * m_moveSpeed * dt;
    int colCurr = (int)((m_exactX + m_width / 2) / tileSize);
    int rowY = (int)((nextY + m_height / 2) / tileSize);

    if (pMap->GetTileID(rowY, colCurr) != 1) { // 1 là Núi
        m_exactY = nextY; // Không va chạm Y -> Di chuyển Y
    }
}

void Player::Update() {
    HandleInput(); // Xử lý di chuyển & Va chạm Tile

    // Lấy kích thước Map thực tế (Pixel)
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int mapPixelW = pMap->GetMapPixelWidth();
    int mapPixelH = pMap->GetMapPixelHeight();

    // Kẹp tọa độ X
    if (m_exactX < 0) {
        m_exactX = 0;
    }
    else if (m_exactX + m_width > mapPixelW) {
        m_exactX = (float)(mapPixelW - m_width);
    }

    // Kẹp tọa độ Y
    if (m_exactY < 0) {
        m_exactY = 0;
    }
    else if (m_exactY + m_height > mapPixelH) {
        m_exactY = (float)(mapPixelH - m_height);
    }

    // Đồng bộ về tọa độ nguyên (int) để vẽ
    m_x = (int)m_exactX;
    m_y = (int)m_exactY;
    
    // Animation đơn giản (nếu có sprite sheet)
    // m_currentFrame = int(((SDL_GetTicks() / 100) % 6)); 
}

void Player::Clean() {
    // Dọn dẹp riêng của Player nếu có (hiện tại chưa cần)
}