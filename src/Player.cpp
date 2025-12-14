#include "Player.h"
#include "GameEngine.h"
#include "Map.h"

Player::Player(const LoaderParams* pParams) : GameObject(pParams) {
    // Giải nén tham số từ LoaderParams vào biến thành viên
    m_x = pParams->m_x;
    m_y = pParams->m_y;

    m_width = pParams->m_width;
    m_height = pParams->m_height;

    m_textureID = pParams->m_textureID;
    m_currentRow = 1;
    m_currentFrame = 0;
    m_flip = pParams->m_flip;

    // Khởi tạo thời gian
    m_lastMoveTime = 0;
}

void Player::Draw() {
    // Sử dụng TextureManager để vẽ chính mình
    // TextureManager::GetInstance()->DrawFrame(m_textureID, m_x, m_y, m_width, m_height, m_currentRow, m_currentFrame, GameEngine::GetInstance()->GetRenderer(), m_flip);
    TextureManager::GetInstance()->Draw(m_textureID, m_x, m_y, m_width, m_height, GameEngine::GetInstance()->GetRenderer(), m_flip);
}

void Player::HandleInput() {
    // 1. Kiểm tra Cooldown: Nếu chưa đủ thời gian nghỉ thì không làm gì cả
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - m_lastMoveTime < MOVE_DELAY) {
        return;
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    // Lấy thông tin Map để tính toán
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int tileSize = pMap->GetTileSize();

    // Tính toán ô hiện tại (làm tròn)
    int currentRow = (m_y + m_height / 2) / tileSize;
    int currentCol = (m_x + m_width / 2) / tileSize;

    int nextRow = currentRow;
    int nextCol = currentCol;

    bool hasInput = false;

    // 2. Xác định ô đích dựa trên phím bấm
    // Dùng else if để chỉ cho phép đi 1 hướng tại 1 thời điểm (không đi chéo)
    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        nextRow--;
        hasInput = true;
    }
    else if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        nextRow++;
        hasInput = true;
    }
    else if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        nextCol--;
        m_flip = SDL_FLIP_HORIZONTAL;
        hasInput = true;
    }
    else if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        nextCol++;
        m_flip = SDL_FLIP_NONE;
        hasInput = true;
    }

    // 3. Xử lý di chuyển
    if (hasInput) {
        // Kiểm tra vật cản (Chỉ chặn nếu là NÚI - ID 1)
        int nextTileID = pMap->GetTileID(nextRow, nextCol);
        
        if (nextTileID != 1) { 
            // --- LƯU TRẠNG THÁI TRƯỚC KHI ĐI ---
            GameEngine::GetInstance()->SaveState();

            // Cập nhật tọa độ
            m_x = nextCol * tileSize;
            m_y = nextRow * tileSize;
            m_lastMoveTime = currentTime;

            // --- BÁO CÁO CHO ENGINE (AAA LOGIC) ---
            // 1. Báo cáo đã di chuyển
            GameEngine::GetInstance()->OnPlayerMove();

            // 2. Báo cáo nếu gặp Trận Nhãn
            // Kiểm tra Trận Nhãn
            // Lưu ý: Lúc này Tile vẫn là 2. Sau khi gọi OnShrineVisited nó mới biến thành 0.
            if (nextTileID == 2) {
                GameEngine::GetInstance()->OnShrineVisited(nextRow, nextCol);
                // Sau này sẽ thêm code phát âm thanh hoặc hiệu ứng tại đây
            }
        }
    }
}

void Player::Update() {
    HandleInput(); // Xử lý di chuyển & Va chạm Tile
    
    // Animation đơn giản (nếu có sprite sheet)
    // m_currentFrame = int(((SDL_GetTicks() / 100) % 6)); 
}

void Player::Clean() {
    // Dọn dẹp riêng của Player nếu có (hiện tại chưa cần)
}