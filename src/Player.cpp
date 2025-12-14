#include "Player.h"
#include "GameEngine.h"
#include "Map.h"
#include <cmath>

Player::Player(const LoaderParams* pParams) : GameObject(pParams) {
    // Giải nén tham số từ LoaderParams vào biến thành viên
    m_x = pParams->m_x;
    m_y = pParams->m_y;

    m_width = pParams->m_width;
    m_height = pParams->m_height;

    // Khởi tạo vị trí hiển thị (Visual) trùng với vị trí Logic ban đầu
    m_visualX = (float)m_x;
    m_visualY = (float)m_y;

    // Khởi tạo trạng thái mặc định
    m_currentState = STATE_IDLE;
    m_lastState = STATE_IDLE;
    m_currentDirection = DIR_DOWN;
    m_lastDirection = DIR_DOWN;

    m_textureID = "player_idle_down"; // ID mặc định
    
    // Tất cả đều có 8 frame
    m_numFrames = 8;
    m_currentFrame = 0;
    
    // Animation Speed: Chạy thì nhanh (100ms), Đứng thì chậm (200ms)
    m_animSpeed = 150;
    
    // Khởi tạo thời gian
    m_lastMoveTime = 0;

    // --- CẤU HÌNH TỶ LỆ AAA ---
    // Hãy thử các số: 1.5f (to rưỡi), 2.0f (to gấp đôi), 2.5f...
    // Tùy thuộc vào bạn muốn nhân vật chiếm bao nhiêu phần của ô đất.
    m_scale = 10.0f;
}

void Player::SetPosition(int x, int y) {
    m_x = x;
    m_y = y;
    
    // Khi teleport (Undo/Reset), visual cũng phải nhảy theo ngay lập tức
    m_visualX = (float)x;
    m_visualY = (float)y;
    
    // Reset thời gian để có thể đi tiếp ngay
    m_lastMoveTime = SDL_GetTicks() - MOVE_DELAY;
}

// Hàm cập nhật ID Texture dựa trên Trạng thái & Hướng
void Player::UpdateAnimationID() {
    // Nếu trạng thái hoặc hướng không đổi, không cần làm gì cả (Tối ưu AAA)
    if (m_currentState == m_lastState && m_currentDirection == m_lastDirection) {
        return;
    }

    std::string actionStr = "";
    std::string dirStr = "";
    
    // 1. Cấu hình Animation cho từng trạng thái
    switch (m_currentState) {
        case STATE_IDLE:
            actionStr = "idle";
            m_animSpeed = 200; // Đứng thở chậm rãi (200ms/frame)
            break;
        case STATE_RUN:
            actionStr = "run";
            m_animSpeed = 80;  // Chạy nhanh (80ms/frame) -> Mượt hơn
            break;
    }

    // 2. Xác định chuỗi Hướng
    switch (m_currentDirection) {
        case DIR_DOWN: dirStr = "down"; break;
        case DIR_LEFT: dirStr = "left"; break;
        case DIR_RIGHT: dirStr = "right"; break;
        case DIR_UP:   dirStr = "up"; break;
    }

    // 3. Tạo ID mới
    m_textureID = "player_" + actionStr + "_" + dirStr;

    // 4. Reset Frame về 0 để bắt đầu animation mới từ đầu
    m_currentFrame = 0;
    
    // 5. Lưu lại trạng thái cũ
    m_lastState = m_currentState;
    m_lastDirection = m_currentDirection;
}

void Player::HandleInput() {
    // 1. Kiểm tra Cooldown: Nếu chưa đủ thời gian nghỉ thì không làm gì cả
    Uint32 currentTime = SDL_GetTicks();
    
    // Nếu nhân vật đã đến đích (Visual đuổi kịp Logic) và không bấm phím -> Về IDLE
    if (abs(m_x - m_visualX) < 2.0f && abs(m_y - m_visualY) < 2.0f) {
        // Chỉ reset về IDLE nếu đang không bị cooldown di chuyển chặn
        if (currentTime - m_lastMoveTime > MOVE_DELAY) {
             m_currentState = STATE_IDLE;
        }
    }

    if (currentTime - m_lastMoveTime < MOVE_DELAY) return;

    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    // Lấy thông tin Map để tính toán
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int tileSize = pMap->GetTileSize();

    // Tính Grid
    int currentRow = (m_y + m_height / 2) / tileSize;
    int currentCol = (m_x + m_width / 2) / tileSize;

    // Reset dx, dy logic grid
    int nextRow = currentRow;
    int nextCol = currentCol;
    bool hasInput = false;

    // 2. Xử lý Phím bấm -> Xác định Hướng
    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        nextRow--;
        hasInput = true;
        m_currentDirection = DIR_UP;
    }
    else if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        nextRow++;
        hasInput = true;
        m_currentDirection = DIR_DOWN;
    }
    else if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        nextCol--;
        hasInput = true;
        m_currentDirection = DIR_LEFT;
    }
    else if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        nextCol++;
        hasInput = true;
        m_currentDirection = DIR_RIGHT;
    }

    // 3. Thực hiện Di chuyển Logic (Grid Jump)
    if (hasInput) {
        // Cập nhật trạng thái sang CHẠY
        m_currentState = STATE_RUN;

        // Kiểm tra va chạm (Map trả về 1 là Núi)
        int nextTileID = pMap->GetTileID(nextRow, nextCol);
        if (nextTileID != 1) { // Không phải Núi
            // --- LƯU TRẠNG THÁI UNDO ---
            GameEngine::GetInstance()->SaveState();

            // Cập nhật tọa độ
            m_x = nextCol * tileSize;
            m_y = nextRow * tileSize;
            m_lastMoveTime = currentTime;

            // Báo cáo Engine
            GameEngine::GetInstance()->OnPlayerMove();

            // Kiểm tra Trận Nhãn
            if (nextTileID == 2) {
                GameEngine::GetInstance()->OnShrineVisited(nextRow, nextCol);
            }
        }
    }

    // Luôn cập nhật Animation ID ở cuối Input để đảm bảo ID đúng
    UpdateAnimationID();
}

void Player::Update() {
    HandleInput(); // Xử lý logic nhảy ô (thay đổi m_x, m_y)

    // 1. Smooth Movement (Lerp)
    float dt = GameEngine::GetInstance()->GetDeltaTime();
    
    // Nội suy Visual về phía Logic
    // Dùng Epsilon 1.0f để snap vị trí khi đã rất gần, tránh rung
    if (abs(m_x - m_visualX) < 1.0f) m_visualX = (float)m_x;
    else m_visualX += (m_x - m_visualX) * SMOOTH_SPEED * dt;

    if (abs(m_y - m_visualY) < 1.0f) m_visualY = (float)m_y;
    else m_visualY += (m_y - m_visualY) * SMOOTH_SPEED * dt;

    // 2. Animation Frame Update (Độc lập với logic di chuyển)
    // Sử dụng SDL_GetTicks để tính frame dựa trên thời gian thực
    // Chia cho m_animSpeed để điều chỉnh tốc độ nhanh/chậm
    m_currentFrame = int(((SDL_GetTicks() / m_animSpeed) % m_numFrames));
}

void Player::Draw() {
    // --- LOGIC CĂN GIỮA (CENTERING) ---
    // Để nhân vật đứng giữa ô, ta cần dịch chuyển lùi lại một chút
    // Công thức: Offset = (Kích thước Tile - Kích thước Nhân vật sau khi Scale) / 2
    
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int tileSize = pMap->GetTileSize();
    
    // Kích thước hiển thị thực tế của Player
    int drawW = (int)(m_width * m_scale);
    int drawH = (int)(m_height * m_scale);
    
    // Tính toán tọa độ vẽ để căn giữa Tile
    // m_visualX là góc trên-trái của ô Tile logic
    int drawX = (int)(m_visualX + (tileSize - drawW) / 2);
    
    // Căn đáy: Để chân nhân vật chạm đáy ô (thường đẹp hơn căn giữa theo chiều dọc)
    // Hoặc căn giữa hoàn toàn: (tileSize - drawH) / 2
    int drawY = (int)(m_visualY + (tileSize - drawH) / 2); 

    TextureManager::GetInstance()->DrawFrame(
        m_textureID, 
        drawX,    // Vẽ tại vị trí đã căn chỉnh
        drawY, 
        m_width,  // Kích thước gốc (để cắt ảnh)
        m_height, 
        1, 
        m_currentFrame, 
        GameEngine::GetInstance()->GetRenderer(), 
        m_scale,  // <--- TRUYỀN TỶ LỆ PHÓNG TO
        SDL_FLIP_NONE
    );
}

void Player::Clean() {}