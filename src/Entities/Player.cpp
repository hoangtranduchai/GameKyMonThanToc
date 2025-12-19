#include "Entities/Player.h"
#include "Core/GameEngine.h"
#include "Graphics/TextureManager.h"
#include "Audio/SoundManager.h"
#include "Entities/Map.h"
#include <cmath> // Dùng cho các phép toán vị trí

// HÀM KHỜI TẠO & HỦY (CONSTRUCTOR & DESTRUCTOR)

Player::Player(const LoaderParams* pParams) 
    : GameObject(pParams),
      m_velX(0.0f),
      m_velY(0.0f),
      m_currentDir(PlayerDirection::DOWN),
      m_lastDir(PlayerDirection::DOWN),
      m_isMoving(false),
      m_isFalling(false),
      m_fallTimer(0.0f),
      m_fallStartY(0.0f)
{
}

Player::~Player() { Clean(); }

void Player::Update() {
    // Lấy khoảng thời gian Delta Time từ Engine
    float dt = GameEngine::GetInstance()->GetDeltaTime();

    // TRƯỜNG HỢP 1: ĐANG RƠI XUỐNG VỰC (FALLING STATE)
    if (m_isFalling) {
        m_fallTimer += dt;

        // 1. Hiệu ứng mờ dần
        // Tính Alpha dựa trên % thời gian đã trôi qua
        float progress = m_fallTimer / FALL_DURATION; // 0.0 -> 1.0
        if (progress > 1.0f) progress = 1.0f;
        
        m_alpha = (int)(255 * (1.0f - progress)); // Giảm từ 255 về 0
        if (m_alpha < 0) m_alpha = 0;

        // 2. Hiệu ứng tụt xuống
        // Rơi xuống khoảng 1 ô (64px) trong suốt quá trình
        m_y = m_fallStartY + (progress * 50.0f); 

        // 3. Kiểm tra kết thúc
        if (m_fallTimer >= FALL_DURATION) {
            // Chuyển sang màn hình thua
            GameEngine::GetInstance()->SwitchState(STATE_RESULT_LOSE);
        }
        
        // Khi đang rơi, không xử lý Input hay Animation nữa
        return; 
    }

    // TRƯỜNG HỢP 2: TRẠNG THÁI BÌNH THƯỜNG (NORMAL STATE)
    HandleInput();          // 1. Đọc bàn phím
    HandleMovement(dt);     // 2. Di chuyển vật lý & Tương tác ô
    UpdateAnimation();      // 3. Cập nhật hình ảnh (Idle/Run)
}

// XỬ LÝ ĐẦU VÀO (INPUT HANDLING)
void Player::HandleInput() {
    // Đặt lại vận tốc mỗi khung hình để nhân vật dừng lại ngay khi thả phím
    m_velX = 0;
    m_velY = 0;
    m_isMoving = false;

    // LÁY TRẠNG THÁI BÀN PHÍM HIỆN TẠI
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    // XỬ LÝ LOGIC DI CHUYỂN (MOVEMENT LOGIC)

    if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) {
        m_velY = -Config::PLAYER_SPEED;
        m_currentDir = PlayerDirection::UP;
        m_isMoving = true;
    }
    else if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) {
        m_velY = Config::PLAYER_SPEED;
        m_currentDir = PlayerDirection::DOWN;
        m_isMoving = true;
    }
    else if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
        m_velX = -Config::PLAYER_SPEED;
        m_currentDir = PlayerDirection::LEFT;
        m_isMoving = true;
    }
    else if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
        m_velX = Config::PLAYER_SPEED;
        m_currentDir = PlayerDirection::RIGHT;
        m_isMoving = true;
    }

    // Nếu đang di chuyển, cập nhật hướng nhìn cuối cùng
    if (m_isMoving) {
        m_lastDir = m_currentDir;
    }
}

// XỬ LÝ DI CHUYỂN VẬT LÝ
void Player::HandleMovement(float dt) {
    // 1. Tính toán vị trí tiếp theo dựa trên vận tốc và Delta Time
    m_x += m_velX * dt;
    m_y += m_velY * dt;

    // 2. Lấy kích thước Map hiện tại
    int mapWidth = GameEngine::GetInstance()->GetMap()->GetCols() * Config::TILE_SIZE;
    int mapHeight = GameEngine::GetInstance()->GetMap()->GetRows() * Config::TILE_SIZE;

    // 3. KIỂM TRA BIÊN BẢN ĐỒ (MAP BOUNDARIES CHECK)
    // Nếu bất kỳ phần nào của nhân vật đi ra khỏi giới hạn Map -> Rơi xuống vực
    bool isOutLeft   = (m_x < 0);
    bool isOutTop    = (m_y < 0);
    bool isOutRight  = (m_x + m_width > mapWidth);
    bool isOutBottom = (m_y + m_height > mapHeight);

    if (isOutLeft || isOutTop || isOutRight || isOutBottom) {
        // Kích hoạt hiệu ứng rơi ngay lập tức
        Falling();
        return;
    }

    // 4. Nếu vẫn ở trong map -> Xử lý tương tác với ô lưới (Tile Interaction)
    ProcessTileInteraction();
}

void Player::ProcessTileInteraction() {
    // Tính toán TÂM (Center) của nhân vật
    // Dùng tâm giúp cảm giác điều khiển chính xác hơn (không bị va chạm khi chỉ mới chạm rìa)
    int centerX = (int)(m_x + m_width / 2);
    int centerY = (int)(m_y + m_height / 2);

    // Quy đổi ra tọa độ lưới (Grid Coordinates)
    int gridCol = centerX / Config::TILE_SIZE;
    int gridRow = centerY / Config::TILE_SIZE;

    // Lấy thông tin bản đồ
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int tileID = pMap->GetTileID(gridRow, gridCol);

    // XỬ LÝ LOGIC TƯƠNG TÁC VỚI Ô LƯỚI (TILE INTERACTION LOGIC)
    
    // TRƯỜNG HỢP 1: TRẬN NHÃN (SHRINE - ID 2)
    if (tileID == Config::TileID::SHRINE) {
        // Gọi Engine xử lý: Cộng điểm, Phát âm thanh
        GameEngine::GetInstance()->OnShrineVisited(gridRow, gridCol);
    }
    // TRƯỜNG HỢP 2: KHÔNG KHÍ (AIR) Đi vào ô này sẽ RƠI
    else if (tileID == Config::TileID::AIR) {
        // Kích hoạt trạng thái rơi ngay lập tức
        Falling();
    }
}

void Player::Falling() {
    if (m_isFalling) return; // Đã rơi rồi thì thôi

    m_isFalling = true;
    m_fallTimer = 0.0f;
    m_fallStartY = m_y; // Ghi nhớ độ cao để làm hiệu ứng tụt xuống
    
    // Phát âm thanh thất bại
    SoundManager::GetInstance()->PlaySFX("sfx_lose");
}

// HỆ THỐNG ANIMATION (VISUALS)
void Player::UpdateAnimation() {
    // 1. Xác định hành động (Action Prefix)
    std::string action = m_isMoving ? "run_" : "idle_";

    // 2. Xác định hướng (Direction Suffix)
    std::string direction = "down"; // Mặc định
    
    // Nếu đang chạy, dùng hướng hiện tại. Nếu đứng yên, dùng hướng cuối cùng (LastDir).
    PlayerDirection dirToUse = m_isMoving ? m_currentDir : m_lastDir;

    switch (dirToUse) {
        case PlayerDirection::UP:    direction = "up"; break;
        case PlayerDirection::DOWN:  direction = "down"; break;
        case PlayerDirection::LEFT:  direction = "left"; break;
        case PlayerDirection::RIGHT: direction = "right"; break;
        default: break;
    }

    // 3. Ghép chuỗi để tạo Texture ID
    // Ví dụ: "run_" + "left" = "run_left"
    std::string newTextureID = action + direction;

    // Cập nhật vào GameObject
    SetTexture(newTextureID);

    // 4. Tính toán Frame Animation
    // Công thức: (Thời gian hệ thống / Tốc độ) % Tổng số frame
    // Giúp animation chạy đều bất kể FPS.
    m_currentFrame = int(((SDL_GetTicks() / m_animSpeed) % m_numFrames));
}

// VẼ HÌNH (RENDERING)
void Player::Draw() {
    // Lấy Renderer
    SDL_Renderer* pRenderer = GameEngine::GetInstance()->GetRenderer();

    // [HIỆU ỨNG RƠI] Cập nhật độ trong suốt (Alpha Mod)
    // Nếu m_alpha < 255, nhân vật sẽ mờ đi.
    // Lấy texture hiện tại từ Manager để set Alpha
    SDL_Texture* tex = TextureManager::GetInstance()->GetTexture(m_textureID); 
    if (tex) SDL_SetTextureAlphaMod(tex, (Uint8)m_alpha);

    // Gọi hàm vẽ Frame chuẩn (Cắt từ dải 8 frame)
    TextureManager::GetInstance()->DrawFrame(
        m_textureID, 
        (int)m_x, (int)m_y, 
        m_width, m_height, 
        m_currentRow, 
        m_currentFrame, 
        pRenderer
    );

    // Khôi phục độ trong suốt về 255 sau khi vẽ
    if (tex) SDL_SetTextureAlphaMod(tex, 255);
}

// Thiết lập lại vị trí nhân vật
void Player::SetPosition(float x, float y) {
    m_x = x;
    m_y = y;
    
    // Khôi phục toàn bộ trạng thái khi được dịch chuyển (Teleport)
    m_isFalling = false;
    m_alpha = 255;
    m_fallTimer = 0.0f;
    m_velX = 0;
    m_velY = 0;
    
    // Khôi phục animation về trạng thái đứng yên hướng xuống
    m_currentDir = PlayerDirection::DOWN;
    m_lastDir = PlayerDirection::DOWN;
    m_isMoving = false;
}

void Player::Clean() {}