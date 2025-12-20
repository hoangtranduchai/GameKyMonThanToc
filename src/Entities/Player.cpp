#include "Entities/Player.h"
#include "Core/GameEngine.h"
#include "Graphics/TextureManager.h"
#include "Audio/SoundManager.h"
#include "Entities/Map.h"
#include <cmath> // Dùng cho các phép toán vị trí
#include <algorithm> // std::min

// HÀM KHỜI TẠO & HỦY (CONSTRUCTOR & DESTRUCTOR)

Player::Player(const LoaderParams* pParams) 
    : GameObject(pParams),
      m_velX(0.0f),
      m_velY(0.0f),
      m_currentDir(PlayerDirection::DOWN),
      m_lastDir(PlayerDirection::DOWN),
      m_isMoving(false),
      m_isMovingToTile(false),
      m_targetX(0.0f),
      m_targetY(0.0f),
      m_isFalling(false),
      m_fallTimer(0.0f),
      m_fallStartY(0.0f)
{}

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
    // Nếu đang di chuyển tới một ô, bỏ qua input mới để đảm bảo mượt
    if (m_isMovingToTile) {
        m_isMoving = true;
        return;
    }

    // Mặc định: không di chuyển cho tới khi nhận phím
    m_isMoving = false;

    // LÁY TRẠNG THÁI BÀN PHÍM HIỆN TẠI
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    // XỬ LÝ LOGIC DI CHUYỂN (MOVEMENT LOGIC)
    if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) {
        m_currentDir = PlayerDirection::UP;
        m_isMoving = true;
        m_isMovingToTile = true;
        m_targetX = m_x;
        m_targetY = m_y - Config::TILE_SIZE;
    }
    else if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) {
        m_currentDir = PlayerDirection::DOWN;
        m_isMoving = true;
        m_isMovingToTile = true;
        m_targetX = m_x;
        m_targetY = m_y + Config::TILE_SIZE;
    }
    else if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
        m_currentDir = PlayerDirection::LEFT;
        m_isMoving = true;
        m_isMovingToTile = true;
        m_targetX = m_x - Config::TILE_SIZE;
        m_targetY = m_y;
    }
    else if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
        m_currentDir = PlayerDirection::RIGHT;
        m_isMoving = true;
        m_isMovingToTile = true;
        m_targetX = m_x + Config::TILE_SIZE;
        m_targetY = m_y;
    }

    // Nếu đang di chuyển, cập nhật hướng nhìn cuối cùng
    if (m_isMoving) {
        m_lastDir = m_currentDir;
    }
}

// XỬ LÝ DI CHUYỂN VẬT LÝ
void Player::HandleMovement(float dt) {
    // Di chuyển mượt tới ô mục tiêu nếu có
    if (m_isMovingToTile) {
        float step = Config::PLAYER_SPEED * dt;
        float dx = m_targetX - m_x;
        float dy = m_targetY - m_y;

        if (std::fabs(dx) > 0.0f) {
            float moveX = (dx > 0 ? 1.0f : -1.0f) * std::min(step, std::fabs(dx));
            m_x += moveX;
        }
        if (std::fabs(dy) > 0.0f) {
            float moveY = (dy > 0 ? 1.0f : -1.0f) * std::min(step, std::fabs(dy));
            m_y += moveY;
        }

        // Đã tới mục tiêu -> Snap chính xác vào tâm ô và ghi nhận bước đi
        if (std::fabs(m_targetX - m_x) < 0.001f && std::fabs(m_targetY - m_y) < 0.001f) {
            m_x = m_targetX;
            m_y = m_targetY;
            m_isMovingToTile = false;
            m_isMoving = false;
            GameEngine::GetInstance()->OnPlayerMove();
            // Sau khi hoàn thành bước, xử lý tương tác ô
            ProcessTileInteraction();
            // Lưu trạng thái sau khi tương tác ô để Undo khôi phục đúng các ô đã thu thập
            GameEngine::GetInstance()->SaveState();
        }
    }

    // Lấy kích thước Map hiện tại và offset
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int mapWidth = pMap->GetCols() * Config::TILE_SIZE;
    int mapHeight = pMap->GetRows() * Config::TILE_SIZE;
    int offsetX = pMap->GetOffsetX();
    int offsetY = pMap->GetOffsetY();

    // KIỂM TRA BIÊN BẢN ĐỒ (MAP BOUNDARIES CHECK)
    // So sánh với vị trí của bản đồ (có tính offset)
    bool isOutLeft   = (m_x < offsetX);
    bool isOutTop    = (m_y < offsetY);
    bool isOutRight  = (m_x + Config::TILE_SIZE > offsetX + mapWidth);
    bool isOutBottom = (m_y + Config::TILE_SIZE > offsetY + mapHeight);

    if (isOutLeft || isOutTop || isOutRight || isOutBottom) {
        // Kích hoạt hiệu ứng rơi
        Falling();
        return;
    }
}

void Player::ProcessTileInteraction() {
    // Tính toán TÂM (Center) của nhân vật
    int centerX = (int)(m_x + Config::TILE_SIZE / 2);
    int centerY = (int)(m_y + Config::TILE_SIZE / 2);

    // Lấy thông tin bản đồ và offset
    Map* pMap = GameEngine::GetInstance()->GetMap();
    int offsetX = pMap->GetOffsetX();
    int offsetY = pMap->GetOffsetY();

    // Quy đổi ra tọa độ lưới (Grid Coordinates) - Trừ offset
    int gridCol = (centerX - offsetX) / Config::TILE_SIZE;
    int gridRow = (centerY - offsetY) / Config::TILE_SIZE;

    // Lấy thông tin ô trên bản đồ
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
    if (m_isFalling) return;

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
    Map* pMap = GameEngine::GetInstance()->GetMap();

    // [HIỆU ỨNG RƠI] Cập nhật độ trong suốt (Alpha Mod)
    // Nếu m_alpha < 255, nhân vật sẽ mờ đi.
    // Lấy texture hiện tại từ Manager để set Alpha
    SDL_Texture* tex = TextureManager::GetInstance()->GetTexture(m_textureID); 
    if (tex) SDL_SetTextureAlphaMod(tex, (Uint8)m_alpha);

    const float S = pMap ? pMap->GetRenderScale() : 1.0f; // Scale theo bản đồ

    // Tính offset căn giữa map (khớp với Map::DrawMap, có chừa margin đáy)
    int mapCols = pMap ? pMap->GetCols() : 0;
    int mapRows = pMap ? pMap->GetRows() : 0;
    int mapWidth = mapCols * Config::TILE_SIZE;
    int mapHeight = mapRows * Config::TILE_SIZE;
    int centerOffsetX = static_cast<int>((Config::SCREEN_WIDTH - mapWidth * S) / 2.0f);
    int centerOffsetY = static_cast<int>((Config::SCREEN_HEIGHT - Config::MAP_BOTTOM_MARGIN - mapHeight * S) / 2.0f);
    if (centerOffsetY < 0) centerOffsetY = 0;

    // Lấy tâm ô theo tọa độ thế giới (m_x, m_y đang là góc trên trái ô + offset map)
    float worldTileCenterX = m_x + Config::TILE_SIZE * 0.5f - (pMap ? pMap->GetOffsetX() : 0);
    float worldTileCenterY = m_y + Config::TILE_SIZE * 0.5f - (pMap ? pMap->GetOffsetY() : 0);

    // Quy đổi sang tọa độ màn hình đã scale và căn giữa
    int screenCenterX = static_cast<int>(worldTileCenterX * S) + centerOffsetX;
    int screenCenterY = static_cast<int>(worldTileCenterY * S) + centerOffsetY;

    // Kích thước sprite sau khi scale theo map
    int scaledWidth = static_cast<int>(m_width * S);
    int scaledHeight = static_cast<int>(m_height * S);

    // Vẽ sao cho nhân vật luôn nằm giữa ô
    TextureManager::GetInstance()->DrawFrameScaled(
        m_textureID,
        screenCenterX - scaledWidth / 2,
        screenCenterY - scaledHeight / 2,
        m_width,
        m_height,
        S,
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
    m_targetX = x;
    m_targetY = y;
    m_isMovingToTile = false;
    
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