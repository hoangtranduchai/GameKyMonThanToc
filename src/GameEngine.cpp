#include "GameEngine.h"
#include "Player.h"
#include "Map.h"
#include "TextureManager.h"
#include "ThienCoEngine.h"
#include <string>
#include "SoundManager.h"
#include "ParticleSystem.h"
#include "Config.h"

// Khởi tạo biến static instance
GameEngine* GameEngine::s_Instance = nullptr;

// Singleton Pattern: Trả về thể hiện duy nhất
GameEngine* GameEngine::GetInstance() {
    if (s_Instance == nullptr) {
        s_Instance = new GameEngine();
    }
    return s_Instance;
}

// Hàm hủy Instance
void GameEngine::DestroyInstance() {
    if (s_Instance != nullptr) {
        delete s_Instance;
        s_Instance = nullptr;
    }
}

GameEngine::GameEngine() {
    m_pWindow = nullptr;
    m_pRenderer = nullptr;
    m_bRunning = false; // Game chưa chạy
    m_pPlayer = nullptr; // Khởi tạo con trỏ Player
    m_pMap = nullptr;    // Khởi tạo con trỏ Map
    m_deltaTime = 0.0f;
    m_optimalSteps = 0;
    m_currentSteps = 0;
    m_shrinesCollected = 0;
    m_totalShrines = 0;
    m_currentState = STATE_MENU; // Bắt đầu ở Menu
    m_blinkTimer = 0.0f;
}

GameEngine::~GameEngine() {
    Quit();
}

// Khởi tạo tất cả hệ thống
bool GameEngine::Init(const char* title, int x, int y, int w, int h, bool fullscreen) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL không thể khởi tạo! Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_image
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cout << "SDL_image không thể khởi tạo! Lỗi: " << IMG_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf không thể khởi tạo! Lỗi: " << TTF_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer không thể khởi tạo! Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tạo cửa sổ game
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    m_pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if (m_pWindow == nullptr) {
        std::cout << "Không thể tạo cửa sổ! Lỗi SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    // Lưu kích thước cửa sổ để giới hạn di chuyển
    m_windowWidth = w;
    m_windowHeight = h;

    // Tạo trình kết xuất render đồ họa
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer == nullptr) {
        std::cout << "Không thể tạo trình kết xuất! Lỗi SDL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_pWindow);     // Dọn dẹp cửa sổ đã tạo
        return false;
    }

    // Đặt màu vẽ mặc định cho trình kết xuất (Màu đen)
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);

    m_windowWidth = w;
    m_windowHeight = h;

    // Load Background
    std::string bgPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/background.png";
    if (!TextureManager::GetInstance()->Load(bgPath, "background", m_pRenderer)) return false;

    // Load ảnh mây (Seamless)
    std::string cloudPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/clouds.png";
    // Chúng ta dùng cùng 1 ảnh nhưng vẽ 2 lần với tốc độ/độ trong suốt khác nhau
    if (!TextureManager::GetInstance()->Load(cloudPath, "clouds", m_pRenderer)) return false;
    
    // Đặt alpha mod cho mây (để nó hơi trong suốt, hòa trộn với nền trời)
    SDL_Texture* cloudTex = TextureManager::GetInstance()->GetTexture("clouds");
    if (cloudTex) {
        // Đặt alpha mây 1 lần khi khởi tạo thay vì mỗi frame
        SDL_SetTextureAlphaMod(cloudTex, 80);
    }
    
    m_cloudScrollX_1 = 0.0f;
    m_cloudScrollX_2 = 0.0f;

    // Load Tileset Image
    std::string tilePath = std::string(PROJECT_ROOT_PATH) + "/assets/images/tiles.png";
    if (!TextureManager::GetInstance()->Load(tilePath, "tiles", m_pRenderer)) return false;

    // --- KHỞI TẠO HỆ THỐNG LEVEL ---
    m_levelFiles.clear();
    int CountLevels = 3; // Cập nhật số lượng level ở đây
    m_levelFiles.reserve(CountLevels);
    // Đăng ký các màn chơi vào hệ thống
    for (int i = 1; i <= CountLevels; ++i) {
        std::string levelFile = std::string(PROJECT_ROOT_PATH) + "/assets/levels/level" + std::to_string(i) + ".txt";
        m_levelFiles.push_back(levelFile);
    }

    m_currentLevelIdx = 0; // Bắt đầu từ Level 1
    m_isAdminMode = false; // Mặc định tắt Admin

    // Load Level đầu tiên
    LoadLevel(m_currentLevelIdx);

    // --- AUTO FIT LOGIC ---
    // Lấy kích thước thực tế của map vừa load
    int mapRealW = m_pMap->GetMapPixelWidth();
    int mapRealH = m_pMap->GetMapPixelHeight();

    // Đặt chế độ scale "Letterbox" để giữ nguyên tỉ lệ khung hình
    // (Nếu map quá dài hoặc quá cao, nó sẽ thêm viền đen thay vì kéo dãn hình ảnh gây méo)
    SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "letterbox");

    // Đặt chế độ scale chất lượng cao "nearest" để giữ độ sắc nét cho pixel art
    // (Nếu muốn mượt mà, đổi "nearest" thành "linear")
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); 

    // LỆNH QUAN TRỌNG NHẤT: Thiết lập kích thước logic
    // SDL sẽ tự động tính toán tỷ lệ zoom để mapRealW x mapRealH vừa khít vào cửa sổ 1280x720
    if (SDL_RenderSetLogicalSize(m_pRenderer, mapRealW, mapRealH) < 0) {
        std::cout << "[Canh bao] Khong the dat Logical Size! SDL Error: " << SDL_GetError() << std::endl;
        // Vẫn tiếp tục, nhưng có thể sẽ không scale đúng
    } else {
        std::cout << "[He thong] Da kich hoat Auto-Scale cho map: " << mapRealW << "x" << mapRealH << std::endl;
    }

    // --- KHỞI TẠO ÂM THANH AAA ---
    // Load Music
    std::string bgmPath = std::string(PROJECT_ROOT_PATH) + "/assets/audio/bgm.mp3";
    SoundManager::GetInstance()->LoadMusic(bgmPath, "bgm");
    SoundManager::GetInstance()->SetMusicVolume(64);  // 50% volume

    // Load SFX
    std::string stepPath = std::string(PROJECT_ROOT_PATH) + "/assets/audio/step.wav";
    SoundManager::GetInstance()->LoadSFX(stepPath, "step");

    std::string collectPath = std::string(PROJECT_ROOT_PATH) + "/assets/audio/collect.wav";
    SoundManager::GetInstance()->LoadSFX(collectPath, "collect");

    std::string winSoundPath = std::string(PROJECT_ROOT_PATH) + "/assets/audio/win.wav";
    SoundManager::GetInstance()->LoadSFX(winSoundPath, "win_sound");

    // Phát nhạc nền ngay lập tức (Lặp vô tận)
    SoundManager::GetInstance()->PlayMusic("bgm");

    // --- KÍCH HOẠT THIÊN CƠ (AI) ---
    // Tính toán ngay lập tức ma trận khoảng cách và Thiên Mệnh

    // 1. Tính Insight
    std::vector<std::vector<int>> insightMatrix = ThienCoEngine::GetInstance()->CalculateInsight(m_pMap);

    // 2. Tính Destiny (Thiên Mệnh)
    DestinyResult destiny = ThienCoEngine::GetInstance()->CalculateDestiny(insightMatrix);
    m_optimalSteps = destiny.totalSteps; // Lưu kết quả

    // Cập nhật tổng số Shrine từ Map
    m_totalShrines = m_pMap->GetShrines().size();
    // Preallocate visited shrines list to expected size
    m_visitedShrinesList.clear();
    if (m_totalShrines > 0) m_visitedShrinesList.reserve(m_totalShrines);

    // --- LOAD FONT CHO HUD ---
    // Vì Logical Height có thể lên tới 2000-3000px, Font size phải to tương ứng
    // Công thức: Size = LogicalHeight / 20 (Ví dụ)
    int fontSize = m_pMap->GetMapPixelHeight() / 15; 
    std::string fontPath = std::string(PROJECT_ROOT_PATH) + "/assets/fonts/Roboto-Regular.ttf"; 
    
    // Nếu chưa có file font, hãy tạo folder assets/fonts và copy file ttf vào!
    // Ở đây tôi giả định bạn sẽ làm việc đó.
    TextureManager::GetInstance()->LoadFont(fontPath, "gui_font", fontSize);

    // --- Cache static HUD/menu texts ---
    {
        SDL_Color gold = {255, 215, 0, 255};
        SDL_Color outline = {200, 100, 0, 255};
        SDL_Color shadow = {0, 0, 0, 200};
        // Title variants
        TextureManager::GetInstance()->CreateTextTexture("gui_font", "KY MON THAN TOC", "title_main", gold, m_pRenderer);
        TextureManager::GetInstance()->CreateTextTexture("gui_font", "KY MON THAN TOC", "title_outline", outline, m_pRenderer);
        TextureManager::GetInstance()->CreateTextTexture("gui_font", "KY MON THAN TOC", "title_shadow", shadow, m_pRenderer);
        // Help label (gold)
        TextureManager::GetInstance()->CreateTextTexture("gui_font", "[U] HOI TUONG   [ESC] MENU", "help_label", gold, m_pRenderer);
    }

    // --- LOAD MENU ART (AAA) ---
    // Đảm bảo file player.png nằm trong assets/images/
    std::string menuBgPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player.png"; 
    if (!TextureManager::GetInstance()->Load(menuBgPath, "menu_background", m_pRenderer)) {
        std::cout << "[Loi Critical] Khong tim thay Menu Art: " << menuBgPath << std::endl;
    }

    // Khởi tạo biến Menu
    m_menuSelection = 0; // Mặc định chọn nút đầu tiên

    // --- [AAA SYSTEM] NẠP SPRITE SHEET PLAYER ---
    // Định nghĩa các hành động và hướng tương ứng với tên file ảnh
    std::string actions[] = {"idle", "run"};
    std::string directions[] = {"down", "left", "right", "up"};
    
    // Vòng lặp tự động nạp tất cả ảnh
    for (const auto& act : actions) {
        for (const auto& dir : directions) {
            // Xây dựng đường dẫn file: assets/images/player/[action]_[dir].png
            // Ví dụ path: assets/images/player/idle_down.png
            std::string fileName = act + "_" + dir + ".png";
            
            // Đường dẫn phẳng (Flat path) như bạn đã upload
            std::string fullPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player/" + fileName;
            
            // ID trong TextureManager: "player_idle_down", "player_run_left"...
            std::string textureID = "player_" + act + "_" + dir;
            
            if (!TextureManager::GetInstance()->Load(fullPath, textureID, m_pRenderer)) {
                std::cout << "[Canh bao] Khong tim thay: " << fullPath << std::endl;
            }
        }
    }

    // --- KHỞI TẠO PLAYER ---
    // Lưu ý: TextureID ban đầu là "player_idle_down"
    int frameW = 96;
    int frameH = 80;

    MapPoint startPos = m_pMap->GetStartPoint();
    int tileSize = m_pMap->GetTileSize();

    // Tạo Player tại vị trí bắt đầu
    m_pPlayer = new Player(new LoaderParams(startPos.col * tileSize, startPos.row * tileSize, frameW, frameH, "player_idle_down"));

    m_currentState = STATE_MENU;
    m_nextState = STATE_MENU;
    m_fadeAlpha = 1.0f; // Bắt đầu là màn hình đen
    m_isFadingIn = true; // Bắt đầu game bằng hiệu ứng sáng dần
    m_isFadingOut = false;

    // Khởi tạo biến Flow
    m_isWinningSequence = false;
    m_winDelayTimer = 0.0f;

    std::cout << "Game Engine & Player Objects khoi tao thanh cong!" << std::endl;
    m_bRunning = true;
    return true;
}

// --- 1. HÀM LƯU TRẠNG THÁI (GỌI TRƯỚC KHI DI CHUYỂN) ---
void GameEngine::SaveState() {
    GameStateMoment moment;
    
    // Lưu tọa độ Grid của Player (Chính xác tuyệt đối)
    int tileSize = m_pMap->GetTileSize();
    moment.playerGridRow = m_pPlayer->GetY() / tileSize;
    moment.playerGridCol = m_pPlayer->GetX() / tileSize;
    
    // Lưu các chỉ số Gameplay
    moment.currentSteps = m_currentSteps;
    moment.shrinesCollected = m_shrinesCollected;
    
    // Lưu bản sao danh sách các Trận Nhãn đã ăn
    moment.visitedShrinesSnapshot = m_visitedShrinesList;

    // Đẩy vào ngăn xếp
    m_historyStack.push(moment);
    
    // TỐI ƯU HÓA BỘ NHỚ AAA:
    // Giới hạn lịch sử Undo là 100 bước để tránh tốn RAM vô ích
    // (Mặc dù stack mặc định không có hàm xóa đáy, nhưng với struct nhỏ này 
    // thì 1000 bước cũng chỉ tốn vài chục KB, nên ta có thể bỏ qua việc xóa đáy 
    // để giữ hiệu năng cao nhất, không cần dùng Deque).
}

// --- 2. HÀM UNDO (QUAY NGƯỢC THỜI GIAN) ---
void GameEngine::Undo() {
    // 1. Kiểm tra an toàn
    if (m_historyStack.empty()) {
        std::cout << "[Thong bao] Khong the Hoi Tuong ve qua khu xa hon!" << std::endl;
        return;
    }

    // 2. Lấy trạng thái từ đỉnh Stack (Snapshot)
    // Dùng tham chiếu (reference) để tránh copy dữ liệu, tối ưu hiệu năng
    const GameStateMoment& moment = m_historyStack.top();

    // 3. TÁI TẠO BẢN ĐỒ THÔNG MINH (SMART RECONSTRUCTION - O(1))
    // Logic: Nếu số lượng Shrine hiện tại nhiều hơn trong quá khứ,
    // nghĩa là bước đi vừa rồi đã ăn mất 1 Shrine.
    // Vị trí Shrine đó chính là vị trí HIỆN TẠI của Player (trước khi lùi lại).
    if (m_shrinesCollected > moment.shrinesCollected) {
        int tileSize = m_pMap->GetTileSize();
        
        // Tính vị trí Grid hiện tại của Player (nơi vừa ăn Shrine)
        int currentGridRow = m_pPlayer->GetY() / tileSize;
        int currentGridCol = m_pPlayer->GetX() / tileSize;
        
        // Hồi sinh Shrine tại đúng vị trí này (Set ID = 2)
        // Không cần duyệt mảng, không cần tìm kiếm -> O(1) tuyệt đối
        m_pMap->SetTileID(currentGridRow, currentGridCol, 2);
    }

    // 4. Khôi phục Vị trí Player (Teleport về quá khứ)
    int tileSize = m_pMap->GetTileSize();
    int restoreX = moment.playerGridCol * tileSize;
    int restoreY = moment.playerGridRow * tileSize;
    
    if (m_pPlayer) {
        // Hàm này sẽ reset cả cooldown di chuyển để gameplay mượt mà
        m_pPlayer->SetPosition(restoreX, restoreY);
    }

    // 5. Khôi phục dữ liệu Gameplay
    m_currentSteps = moment.currentSteps;
    m_shrinesCollected = moment.shrinesCollected;
    
    // Copy lại danh sách đã ăn (Vector assignment cũng được tối ưu rất tốt trong C++)
    m_visitedShrinesList = moment.visitedShrinesSnapshot;

    // 6. Xóa trạng thái vừa dùng khỏi Stack
    m_historyStack.pop();

    std::cout << "<<< HOI TUONG THANH CONG! (Buoc: " << m_currentSteps << ") <<<" << std::endl;
}

// Xử lý sự kiện
void GameEngine::HandleEvents() {
    SDL_Event event;
    // Xử lý tất cả các sự kiện trong hàng đợi
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT
            || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
            m_bRunning = false;
        }

        // --- XỬ LÝ THEO TRẠNG THÁI ---
        switch (m_currentState) {
            case STATE_MENU:
                // Điều hướng Menu bằng phím mũi tên
                if (event.key.keysym.sym == SDLK_UP) {
                    m_menuSelection--;
                    if (m_menuSelection < 0) m_menuSelection = 1; // Loop về cuối
                    SoundManager::GetInstance()->PlaySFX("step");
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    m_menuSelection++;
                    if (m_menuSelection > 1) m_menuSelection = 0; // Loop về đầu
                    SoundManager::GetInstance()->PlaySFX("step");
                }
                
                // Chọn (Enter/Space)
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                    SoundManager::GetInstance()->PlaySFX("collect"); // Âm thanh chọn
                    
                    if (m_menuSelection == 0) { // Nút Play
                        // ResetGame(); // Bỏ comment nếu muốn reset mỗi khi bấm Play
                        SwitchState(STATE_PLAY);
                    } 
                    else if (m_menuSelection == 1) { // Nút Quit
                        m_bRunning = false;
                    }
                }
                break;

            case STATE_PLAY:
                // Các phím điều khiển nhân vật chỉ hoạt động ở đây
                if (event.type == SDL_KEYDOWN) {
                    // Undo
                    if (event.key.keysym.sym == SDLK_u) Undo();
                
                    // Bấm ESC để TẠM DỪNG
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SwitchState(STATE_PAUSE);
                    }

                    // --- ADMIN CHEATS (QUYỀN LỰC TỐI THƯỢNG) ---
                    // Chỉ hoạt động khi bật F1
                    if (m_isAdminMode) {
                        // Phím N: Next Level (Skip màn chơi ngay lập tức)
                        if (event.key.keysym.sym == SDLK_n) {
                            SoundManager::GetInstance()->PlaySFX("win_sound");
                            NextLevel();
                        }
                    }
                }
                break;
            
            case STATE_PAUSE:
                if (event.type == SDL_KEYDOWN) {
                    // ESC: Tiếp tục
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        m_currentState = STATE_PLAY;
                    }
                    // M: Về Menu chính
                    if (event.key.keysym.sym == SDLK_m) {
                        SwitchState(STATE_MENU);
                    }
                    // Q: Thoát Game
                    if (event.key.keysym.sym == SDLK_q) {
                        m_bRunning = false;
                    }
                }
                break;

            case STATE_WIN:
                if (event.type == SDL_KEYDOWN) {
                    // Phím Enter hoặc Space: Qua màn tiếp theo
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                        NextLevel(); 
                    }

                    // Phím R: Chơi lại màn hiện tại
                    if (event.key.keysym.sym == SDLK_r) {
                        ResetGame(); // Reset dữ liệu
                        SwitchState(STATE_PLAY); // Chuyển cảnh lại vào game
                    }

                    // Phím M: Về Menu chính
                    if (event.key.keysym.sym == SDLK_m) {
                        SwitchState(STATE_MENU);
                    }
                }
                break;
        }
    }
}

// Cập nhật logic Game
void GameEngine::Update() {
    // Tính toán Delta Time (with caching for efficiency)
    static Uint32 lastTick = SDL_GetTicks();
    Uint32 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - lastTick) / 1000.0f;
    lastTick = currentTick;

    // Đặt giới hạn DeltaTime để tránh lỗi khi pause/debug quá lâu
    const float MAX_DELTA = 0.05f;
    if (m_deltaTime > MAX_DELTA) {
        m_deltaTime = MAX_DELTA;
    }
    
    // --- LOGIC CHUYỂN CẢNH (TRANSITION SYSTEM) ---
    if (m_isFadingOut) {
        m_fadeAlpha += FADE_SPEED * m_deltaTime;
        if (m_fadeAlpha >= 1.0f) {
            m_fadeAlpha = 1.0f;
            m_currentState = m_nextState; // CHÍNH THỨC ĐỔI TRẠNG THÁI
            m_isFadingOut = false;
            m_isFadingIn = true; // Bắt đầu sáng lại
            
            // Nếu vào lại MENU hoặc PLAY từ đầu, có thể cần Reset nhạc/Game tại đây
            if (m_currentState == STATE_PLAY && m_nextState == STATE_PLAY) {
                 // Logic Reset nếu cần
            }
        }
        return; // Đang chuyển cảnh thì KHÔNG update logic game
    }

    if (m_isFadingIn) {
        m_fadeAlpha -= FADE_SPEED * m_deltaTime;
        if (m_fadeAlpha <= 0.0f) {
            m_fadeAlpha = 0.0f;
            m_isFadingIn = false;
        }
    }

    // --- LOGIC THEO TRẠNG THÁI ---
    switch (m_currentState) {
        case STATE_MENU:
        case STATE_WIN:
            m_blinkTimer += m_deltaTime;
            if (m_blinkTimer > 1.0f) m_blinkTimer = 0.0f;
            break;

        case STATE_PLAY: {
            // Chỉ Update Game khi ĐANG CHƠI (Không Pause)
            if (m_pPlayer) m_pPlayer->Update();
            ParticleSystem::GetInstance()->Update(m_deltaTime);
            
            // Mây trôi (cache delta time multiplications)
            const float cloudDelta1 = CLOUD_SPEED_1 * m_deltaTime;
            const float cloudDelta2 = CLOUD_SPEED_2 * m_deltaTime;
            m_cloudScrollX_1 -= cloudDelta1;
            m_cloudScrollX_2 -= cloudDelta2;
            
            // Reset khi trôi hết kích thước ảnh
            int mapW = m_pMap->GetMapPixelWidth();
            
            // Logic cuộn vô tận
            if (m_cloudScrollX_1 <= -mapW) m_cloudScrollX_1 = 0;
            if (m_cloudScrollX_2 <= -mapW) m_cloudScrollX_2 = 0;
            break;
        }

        case STATE_PAUSE:
            // KHI PAUSE: KHÔNG update Player, Map, Particles.
            // Game "đóng băng" hoàn toàn. 
            // Chỉ update các hiệu ứng riêng của Menu Pause nếu có.
            break;
    }

    // --- LOGIC VICTORY SEQUENCE ---
    if (m_isWinningSequence) {
        m_winDelayTimer -= m_deltaTime;
        
        // Tạo thêm hạt nổ tưng bừng trong lúc chờ
        if ((int)(m_winDelayTimer * 10) % 2 == 0) { // Cứ mỗi vài frame
            int w = m_pMap->GetMapPixelWidth();
            int h = m_pMap->GetMapPixelHeight();
            // Bắn pháo hoa ngẫu nhiên (cache colors)
            static SDL_Color goldColor = {255, 215, 0, 255};
            ParticleSystem::GetInstance()->Emit(rand() % w, rand() % h, 10, goldColor);
        }

        if (m_winDelayTimer <= 0.0f) {
            m_currentState = STATE_WIN; // Chuyển cảnh chính thức
            m_isWinningSequence = false;
        }
    }
}

void GameEngine::OnPlayerMove() {
    m_currentSteps++;
    // Phát tiếng bước chân
    SoundManager::GetInstance()->PlaySFX("step");
}

void GameEngine::ResetGame() {
    // 1. Reset thông số gameplay
    m_currentSteps = 0;
    m_shrinesCollected = 0;
    m_visitedShrinesList.clear();
    
    // 2. Xóa lịch sử Undo
    while (!m_historyStack.empty()) {
        m_historyStack.pop();
    }

    // 3. Reset Player về vị trí xuất phát
    MapPoint startPos = m_pMap->GetStartPoint();
    int tileSize = m_pMap->GetTileSize();
    m_pPlayer->SetPosition(startPos.col * tileSize, startPos.row * tileSize);

    // 4. Khôi phục toàn bộ Map (Vẽ lại tất cả Shrine)
    const std::vector<MapPoint>& shrines = m_pMap->GetShrines();
    for (const auto& s : shrines) {
        m_pMap->SetTileID(s.row, s.col, 2); // 2 = Shrine ID
    }

    // 5. Chuyển về trạng thái Menu hoặc Play
    m_currentState = STATE_PLAY; // Hoặc STATE_MENU nếu muốn về menu

    m_isWinningSequence = false;
    m_winDelayTimer = 0.0f;

    // Bật lại nhạc nền
    SoundManager::GetInstance()->PlayMusic("bgm");
    
    std::cout << "[He thong] Game da duoc Reset!" << std::endl;
}

void GameEngine::OnShrineVisited(int row, int col) {
    // Kiểm tra xem shrine này đã mở chưa (use linear search only once)
    for (const auto& pos : m_visitedShrinesList) {
        if (pos.first == row && pos.second == col) return; // Đã mở rồi
    }

    // Đánh dấu đã mở
    m_visitedShrinesList.push_back({row, col});
    m_shrinesCollected++;

    // --- HIỆU ỨNG HÌNH ẢNH AAA ---
    // Khi ăn xong, biến Trận Nhãn (ID 2) thành Đất Thường (ID 0)
    m_pMap->SetTileID(row, col, 0);
    
    // Phát hiệu ứng âm thanh khi khai mở Trận Nhãn
    SoundManager::GetInstance()->PlaySFX("collect");

    // --- KÍCH HOẠT HIỆU ỨNG NĂNG LƯỢNG (AAA MOMENT) ---
    int tileSize = m_pMap->GetTileSize();
    // Tính tâm của ô Trận Nhãn (cache calculations)
    int centerX = col * tileSize + (tileSize >> 1);  // Use bit shift instead of division
    int centerY = row * tileSize + (tileSize >> 1);
    
    // Màu xanh ngọc (Cyan)
    SDL_Color magicColor = {0, 255, 255, 255}; 
    SDL_Color goldColor = {255, 215, 0, 255};
    
    // Bắn ra hạt năng lượng
    ParticleSystem::GetInstance()->Reserve(800);
    ParticleSystem::GetInstance()->Emit(centerX, centerY, 500, magicColor);
    ParticleSystem::GetInstance()->Emit(centerX, centerY, 200, goldColor);
    
    std::cout << ">>> DA KHAI MO TRAN NHAN! (" << m_shrinesCollected << "/" << m_totalShrines << ")" << std::endl;

    // Kiểm tra Chiến Thắng
    if (m_shrinesCollected >= m_totalShrines) {
        // KHÔNG chuyển state ngay lập tức!
        if (!m_isWinningSequence) {
            m_isWinningSequence = true;
            m_winDelayTimer = 2.0f; // Chờ 2 giây để người chơi ngắm hiệu ứng
            
            // Phát âm thanh chiến thắng
            SoundManager::GetInstance()->PlaySFX("win_sound");
            
            // Fade out nhạc nền trong 2 giây (Tắt dần)
            Mix_FadeOutMusic(2000); 
            
            std::cout << ">>> VICTORY SEQUENCE STARTED! <<<" << std::endl;
        }
    }
}

// Vẽ HUD
void GameEngine::DrawHUD() {
    int w = m_windowWidth;
    
    // 1. THANH TOP BAR (CINEMATIC BAR)
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
    // Gradient đen mờ -> Giúp chữ dễ đọc tuyệt đối
    SDL_Rect bar = {0, 0, w, 50};
    SDL_SetRenderDrawColor(m_pRenderer, 10, 10, 10, 240); // Gần như đen kịt
    SDL_RenderFillRect(m_pRenderer, &bar);
    
    // Đường chỉ vàng ngăn cách HUD và Game
    SDL_SetRenderDrawColor(m_pRenderer, 255, 215, 0, 255);
    SDL_RenderDrawLine(m_pRenderer, 0, 50, w, 50);
    
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_NONE);

    // 2. HIỂN THỊ THÔNG SỐ (Dùng màu tương phản cao)
    SDL_Color gold = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {0, 255, 255, 255};
    SDL_Color red  = {255, 80, 80, 255};

    // --- LEFT: THIÊN MỆNH (BƯỚC CHÂN) ---
    static std::string lastStepStr;
    std::string stepStr = "THIEN MENH: " + std::to_string(m_currentSteps) + " / " + std::to_string(m_optimalSteps);
    // Nếu đổi nội dung -> cập nhật cache
    if (stepStr != lastStepStr) {
        TextureManager::GetInstance()->DropText("hud_steps_shadow");
        TextureManager::GetInstance()->DropText("hud_steps");
        TextureManager::GetInstance()->CreateTextTexture("gui_font", stepStr, "hud_steps_shadow", {0,0,0,255}, m_pRenderer);
        SDL_Color stepColorDyn = (m_currentSteps <= m_optimalSteps) ? white : red;
        TextureManager::GetInstance()->CreateTextTexture("gui_font", stepStr, "hud_steps", stepColorDyn, m_pRenderer);
        lastStepStr = stepStr;
    }
    // Vẽ bóng + chữ từ cache
    TextureManager::GetInstance()->DrawTextCached("hud_steps_shadow", 32, 12, m_pRenderer);
    TextureManager::GetInstance()->DrawTextCached("hud_steps", 30, 10, m_pRenderer);

    // --- CENTER: KHAI MỞ (TIẾN ĐỘ) ---
    static std::string lastShrineStr;
    std::string shrineStr = "TRAN NHAN: " + std::to_string(m_shrinesCollected) + " / " + std::to_string(m_totalShrines);
    if (shrineStr != lastShrineStr) {
        TextureManager::GetInstance()->DropText("hud_shrine_shadow");
        TextureManager::GetInstance()->DropText("hud_shrine");
        TextureManager::GetInstance()->CreateTextTexture("gui_font", shrineStr, "hud_shrine_shadow", {0,0,0,255}, m_pRenderer);
        TextureManager::GetInstance()->CreateTextTexture("gui_font", shrineStr, "hud_shrine", cyan, m_pRenderer);
        lastShrineStr = shrineStr;
    }
    TextureManager::GetInstance()->DrawTextCached("hud_shrine_shadow", w/2 - 118, 12, m_pRenderer);
    TextureManager::GetInstance()->DrawTextCached("hud_shrine", w/2 - 120, 10, m_pRenderer);

    // --- RIGHT: PHÍM TẮT ---
    // Help text (cached gold label)
    TextureManager::GetInstance()->DrawTextCached("help_label", w - 320, 10, m_pRenderer);
}

// ---------------------------------------------------------------------------
// RENDER LOOP - VẼ TOÀN BỘ THẾ GIỚI GAME
// ---------------------------------------------------------------------------
void GameEngine::Render() {
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);

    // Lấy kích thước màn hình
    int w = m_pMap->GetMapPixelWidth(); // Hoặc m_windowWidth nếu dùng Auto-Scale
    int h = m_pMap->GetMapPixelHeight();
    // Cache TextureManager and Renderer pointers
    auto* texMgr = TextureManager::GetInstance();
    auto* renderer = m_pRenderer;

    // --- MÁY TRẠNG THÁI HÌNH ẢNH (VISUAL STATE MACHINE) ---
    switch (m_currentState) {
        
        // =================================================================
        // 1. MENU SCREEN: TUYỆT TÁC MỞ ĐẦU
        // =================================================================
        case STATE_MENU: {
            // A. Lớp Nền: Background Art (player.png)
            // Vẽ full màn hình
            texMgr->Draw("menu_background", 0, 0, w, h, renderer);
            
            // B. Lớp Khí Quyển: Mây Trôi (Vẽ NGAY SAU nền, TRƯỚC UI)
            // Lớp 1 (Xa)
            texMgr->Draw("clouds", (int)m_cloudScrollX_1, 0, w, h, renderer);
            texMgr->Draw("clouds", (int)m_cloudScrollX_1 + w, 0, w, h, renderer);
            
            // Lớp 2 (Gần - Mờ ảo sương khói)
            // Alpha mây đã được set tại Init; không cần đổi lại mỗi frame
            texMgr->Draw("clouds", (int)m_cloudScrollX_2, 0, w, h, renderer);
            texMgr->Draw("clouds", (int)m_cloudScrollX_2 + w, 0, w, h, renderer);

            // C. Lớp Giao Diện (UI Layer) - Vẽ đè lên tất cả
            
            // --- LOGO GAME: Khung Tiêu Đề Hoành Tráng ---
            int titleBoxW = 600;
            int titleBoxH = 120;
            int titleY = h/5; // Vị trí 20% từ trên xuống
            
            // Vẽ khung rỗng (chỉ viền và nền mờ)
            DrawStylishBox((w - titleBoxW)/2, titleY, titleBoxW, titleBoxH, ""); 
            
            // Vẽ Chữ Tiêu Đề (Hiệu ứng Bóng Đổ Kép - Double Drop Shadow)
            int titleTextX = w/2 - 190; 
            int titleTextY = titleY + 40;
            // Bóng đen đậm
            texMgr->DrawTextCached("title_shadow", titleTextX + 4, titleTextY + 4, renderer);
            // Viền sáng nhẹ
            texMgr->DrawTextCached("title_outline", titleTextX + 2, titleTextY + 2, renderer);
            // Chữ chính màu Vàng Kim
            texMgr->DrawTextCached("title_main", titleTextX, titleTextY, renderer);

            // --- MENU BUTTONS ---
            int btnW = 350; 
            int btnH = 60;
            int startY = h/2 + 60; // Dịch xuống dưới một chút để không che nhân vật

            // Nút 1: Nhập Thế
            DrawButton("NHAP THE (Play)", (w - btnW)/2, startY, btnW, btnH, m_menuSelection == 0);
            
            // Nút 2: Rời Khỏi
            DrawButton("ROI KHOI (Quit)", (w - btnW)/2, startY + 80, btnW, btnH, m_menuSelection == 1);

            // Footer
            TextureManager::GetInstance()->DrawText("gui_font", "Phien ban AAA - PBL2 Project", 20, h - 40, {150, 150, 150, 255}, m_pRenderer);
            
            if (m_isAdminMode) {
                TextureManager::GetInstance()->DrawText("gui_font", "[ADMIN MODE ONLINE]", w - 280, h - 40, {255, 50, 50, 255}, m_pRenderer);
            }
            break;
        }

        // =================================================================
        // 2. PLAY SCREEN: TRẢI NGHIỆM MƯỢT MÀ
        // =================================================================
        case STATE_PLAY: {
            // Lớp 1: Nền trời
            TextureManager::GetInstance()->Draw("background", 0, 0, w, h, m_pRenderer);
            
            // Lớp 2: Mây trôi (Xa)
            TextureManager::GetInstance()->Draw("clouds", (int)m_cloudScrollX_1, 0, w, h, m_pRenderer);
            TextureManager::GetInstance()->Draw("clouds", (int)m_cloudScrollX_1 + w, 0, w, h, m_pRenderer);

            // Lớp 3: Thế giới Game (Map -> Player -> VFX)
            if (m_pMap) m_pMap->DrawMap();
            if (m_pPlayer) m_pPlayer->Draw();
            ParticleSystem::GetInstance()->Render(m_pRenderer); // Hạt năng lượng vẽ đè lên tất cả
            
            // Lớp 4: UI (HUD)
            DrawHUD(); 
            break;
        }

        // =================================================================
        // 3. PAUSE SCREEN: TĨNH LẶNG & RÕ RÀNG
        // =================================================================
        case STATE_PAUSE: {
            // Vẽ lại toàn bộ màn hình Play làm nền (đóng băng)
            TextureManager::GetInstance()->Draw("background", 0, 0, w, h, m_pRenderer);
            if (m_pMap) m_pMap->DrawMap();
            if (m_pPlayer) m_pPlayer->Draw();
            DrawHUD(); // Vẫn hiện HUD mờ bên dưới
            
            // LỚP PHỦ TỐI (DIMMING OVERLAY)
            SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 200); // Đen đậm (80% opacity)
            SDL_RenderFillRect(m_pRenderer, NULL);
            
            // HỘP THOẠI PAUSE
            int boxW = 500; int boxH = 320;
            int boxX = (w - boxW)/2;
            int boxY = (h - boxH)/2;
            DrawStylishBox(boxX, boxY, boxW, boxH, "--- TAM DUNG ---");
            
            // Nội dung hướng dẫn (Căn giữa Box)
            int contentY = boxY + 100;
            int contentX = w/2 - 130;
            SDL_Color white = {255, 255, 255, 255};
            SDL_Color gray = {200, 200, 200, 255};
            
            TextureManager::GetInstance()->DrawText("gui_font", "[ESC]  TIEP TUC", contentX, contentY, white, m_pRenderer);
            TextureManager::GetInstance()->DrawText("gui_font", "[M]    VE MENU", contentX, contentY + 60, gray, m_pRenderer);
            TextureManager::GetInstance()->DrawText("gui_font", "[Q]    THOAT GAME", contentX, contentY + 120, gray, m_pRenderer);
            break;
        }

        // =================================================================
        // 4. WIN SCREEN: VINH QUANG TỘT ĐỈNH
        // =================================================================
        case STATE_WIN: {
            // Nền: Art Menu tái sử dụng (hoặc ảnh Win riêng nếu có)
            TextureManager::GetInstance()->Draw("menu_background", 0, 0, w, h, m_pRenderer);

            // Lớp phủ tối cực đậm để tôn vinh bảng kết quả
            SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 220);
            SDL_RenderFillRect(m_pRenderer, NULL);

            // HỘP THOẠI CHIẾN THẮNG (Lớn hơn bình thường)
            int boxW = 700; int boxH = 400;
            DrawStylishBox((w - boxW)/2, (h - boxH)/2, boxW, boxH, "");
            
            // Tiêu đề lớn (Vàng kim rực rỡ)
            SDL_Color gold = {255, 215, 0, 255};
            TextureManager::GetInstance()->DrawText("gui_font", "THIEN MENH HOAN TAT!", w/2 - 250, h/2 - 120, gold, m_pRenderer);

            // Đánh giá Rank (S/A)
            std::string rankStr;
            SDL_Color rankColor;
            if (m_currentSteps <= m_optimalSteps) {
                rankStr = "DANH GIA: TUYET DINH (S)";
                rankColor = {0, 255, 255, 255}; // Cyan (Hoàn hảo)
            } else {
                rankStr = "DANH GIA: HOAN THANH (A)";
                rankColor = {200, 200, 200, 255}; // Bạc (Khá)
            }
            
            TextureManager::GetInstance()->DrawText("gui_font", rankStr, w/2 - 220, h/2 - 40, rankColor, m_pRenderer);

            // --- CẬP NHẬT HƯỚNG DẪN MỚI ---
            
            // Nếu còn level tiếp theo -> Hiện "Next Level"
            if (m_currentLevelIdx < m_levelFiles.size() - 1) {
                // Hiệu ứng nhấp nháy cho nút quan trọng nhất
                if (m_blinkTimer < 0.5f) {
                    TextureManager::GetInstance()->DrawText("gui_font", "[ENTER]  MAN TIEP THEO >>", w/2 - 180, h/2 + 50, {0, 255, 255, 255}, m_pRenderer);
                }
            } else {
                // Nếu là màn cuối
                if (m_blinkTimer < 0.5f) {
                    TextureManager::GetInstance()->DrawText("gui_font", "CHUC MUNG! BAN DA PHA DAO!", w/2 - 180, h/2 + 50, {255, 215, 0, 255}, m_pRenderer);
                }
            }

            TextureManager::GetInstance()->DrawText("gui_font", "[R] Choi Lai    [M] Menu", w/2 - 140, h/2 + 100, {150, 150, 150, 255}, m_pRenderer);

            break;
        }
    }

    // --- HIỆU ỨNG CHUYỂN CẢNH (FADE IN/OUT) ---
    // Vẽ một màn hình đen với độ trong suốt thay đổi để làm dịu mắt khi chuyển cảnh
    if (m_fadeAlpha > 0.0f) {
        SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
        Uint8 alphaByte = (Uint8)(m_fadeAlpha * 255);
        SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, alphaByte);
        SDL_RenderFillRect(m_pRenderer, NULL);
        SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(m_pRenderer);
}

void GameEngine::SwitchState(GameState newState) {
    m_nextState = newState;
    m_isFadingOut = true; // Bắt đầu hiệu ứng tối dần
    // Khi màn hình đen kịt (Alpha = 1), ta sẽ đổi m_currentState
}

// Dọn dẹp Tài nguyên
void GameEngine::Quit() {
    // Dọn dẹp Player
    if (m_pPlayer) {
        m_pPlayer->Clean();
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }
    // Dọn dẹp Map
    if (m_pMap) {
        delete m_pMap;
        m_pMap = nullptr;
    }

    TextureManager::GetInstance()->Clean();

    // Dọn dẹp "Cây cọ" (Renderer) và "Bức toan" (Window)
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;
    m_pRenderer = nullptr;

    // Dọn dẹp các thư viện
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    // Tự hủy Instance
    m_bRunning = false;
    
    std::cout << "Game Engine đã dọn dẹp và kết thúc!" << std::endl;
}

// ---------------------------------------------------------------------------
// HỆ THỐNG UI AAA - TUYỆT KỸ VẼ GIAO DIỆN (PROCEDURAL UI RENDERING)
// ---------------------------------------------------------------------------

void GameEngine::DrawStylishBox(int x, int y, int w, int h, std::string title) {
    // 1. LỚP NỀN (BACKGROUND): Hiệu ứng kính tối màu (Glassmorphism)
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
    // Màu: Nâu đen pha tím than (Rất sang trọng cho game tiên hiệp)
    SDL_SetRenderDrawColor(m_pRenderer, 20, 15, 30, 235); 
    SDL_Rect bg = {x, y, w, h};
    SDL_RenderFillRect(m_pRenderer, &bg);

    // 2. KHUNG VIỀN (BORDERS): Vàng Kim Loại (Metallic Gold)
    SDL_SetRenderDrawColor(m_pRenderer, 255, 215, 0, 255); 
    SDL_Rect border = {x, y, w, h};
    SDL_RenderDrawRect(m_pRenderer, &border);
    
    // Viền nội (Inner Border) tạo chiều sâu 3D
    SDL_SetRenderDrawColor(m_pRenderer, 200, 160, 0, 150); // Vàng tối hơn
    SDL_Rect border2 = {x+4, y+4, w-8, h-8};
    SDL_RenderDrawRect(m_pRenderer, &border2);

    // 3. HỌA TIẾT 4 GÓC (CORNER ACCENTS) - Tăng độ tinh xảo
    SDL_SetRenderDrawColor(m_pRenderer, 0, 255, 255, 255); // Màu Cyan điểm xuyết
    int s = 25; // Độ dài cạnh góc
    int t = 3;  // Độ dày nét
    
    // Vẽ 4 góc thủ công
    SDL_Rect c1 = {x, y, s, t}; SDL_RenderFillRect(m_pRenderer, &c1); // Top-Left H
    SDL_Rect c2 = {x, y, t, s}; SDL_RenderFillRect(m_pRenderer, &c2); // Top-Left V
    
    SDL_Rect c3 = {x + w - s, y, s, t}; SDL_RenderFillRect(m_pRenderer, &c3); // Top-Right H
    SDL_Rect c4 = {x + w - t, y, t, s}; SDL_RenderFillRect(m_pRenderer, &c4); // Top-Right V

    SDL_Rect c5 = {x, y + h - t, s, t}; SDL_RenderFillRect(m_pRenderer, &c5); // Bot-Left H
    SDL_Rect c6 = {x, y + h - s, t, s}; SDL_RenderFillRect(m_pRenderer, &c6); // Bot-Left V

    SDL_Rect c7 = {x + w - s, y + h - t, s, t}; SDL_RenderFillRect(m_pRenderer, &c7); // Bot-Right H
    SDL_Rect c8 = {x + w - t, y + h - s, t, s}; SDL_RenderFillRect(m_pRenderer, &c8); // Bot-Right V

    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_NONE);

    // 4. TIÊU ĐỀ HỘP THOẠI (HEADER)
    if (!title.empty()) {
        // Vẽ đường gạch chân phân cách tiêu đề
        SDL_SetRenderDrawColor(m_pRenderer, 255, 215, 0, 100);
        SDL_RenderDrawLine(m_pRenderer, x + 20, y + 50, x + w - 20, y + 50);

        // Vẽ Text Tiêu đề (Căn giữa)
        SDL_Color gold = {255, 215, 0, 255};
        // Ước lượng width text để căn giữa (Giả sử 1 char ~ 15px với font to)
        int textX = x + (w - (int)title.length() * 12) / 2; 
        TextureManager::GetInstance()->DrawText("gui_font", title, textX, y + 15, gold, m_pRenderer);
    }
}

void GameEngine::DrawButton(std::string text, int x, int y, int w, int h, bool isSelected) {
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
    
    if (isSelected) {
        // --- HIỆU ỨNG GLOW KHI CHỌN (HOVER STATE) ---
        // Vầng hào quang Cyan tỏa ra ngoài
        SDL_SetRenderDrawColor(m_pRenderer, 0, 255, 255, 100); 
        SDL_Rect glow = {x-5, y-5, w+10, h+10};
        SDL_RenderFillRect(m_pRenderer, &glow);
        
        // Nền nút sáng hơn (Xanh rêu đậm)
        SDL_SetRenderDrawColor(m_pRenderer, 0, 80, 80, 240); 
        
        // Mũi tên chỉ dẫn phong cách RPG
        TextureManager::GetInstance()->DrawText("gui_font", ">>", x - 35, y + (h/2) - 12, {0, 255, 255, 255}, m_pRenderer);
    } else {
        // --- TRẠNG THÁI BÌNH THƯỜNG (NORMAL STATE) ---
        // Nền xám đen
        SDL_SetRenderDrawColor(m_pRenderer, 40, 40, 50, 220); 
    }
    
    // Vẽ nền nút
    SDL_Rect bg = {x, y, w, h};
    SDL_RenderFillRect(m_pRenderer, &bg);
    SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_NONE);

    // Vẽ viền nút
    SDL_Color borderColor = isSelected ? SDL_Color{0, 255, 255, 255} : SDL_Color{150, 150, 150, 255};
    SDL_SetRenderDrawColor(m_pRenderer, borderColor.r, borderColor.g, borderColor.b, 255);
    SDL_RenderDrawRect(m_pRenderer, &bg);

    // Vẽ Text nút (Căn lề trái có margin)
    SDL_Color textColor = isSelected ? SDL_Color{255, 255, 255, 255} : SDL_Color{180, 180, 180, 255};
    TextureManager::GetInstance()->DrawText("gui_font", text, x + 40, y + 10, textColor, m_pRenderer);
}

void GameEngine::LoadLevel(int levelIndex) {
    // 1. Kiểm tra hợp lệ
    if (levelIndex < 0 || levelIndex >= m_levelFiles.size()) {
        std::cout << "[System] Da het Level! Quay ve Menu." << std::endl;
        SwitchState(STATE_MENU);
        return;
    }

    m_currentLevelIdx = levelIndex;

    // 2. Dọn dẹp Map cũ
    if (m_pMap) {
        delete m_pMap;
        m_pMap = nullptr;
    }

    // 3. Load Map mới
    m_pMap = new Map();
    m_pMap->LoadMap(m_levelFiles[m_currentLevelIdx]);

    // 4. Auto-Fit Map vào màn hình (AAA Feature)
    int mapRealW = m_pMap->GetMapPixelWidth();
    int mapRealH = m_pMap->GetMapPixelHeight();
    SDL_RenderSetLogicalSize(m_pRenderer, mapRealW, mapRealH);

    // 5. Tính toán Thiên Cơ (AI) cho Map mới
    // Reset dữ liệu cũ
    m_visitedShrinesList.clear();
    m_currentSteps = 0;
    m_shrinesCollected = 0;
    m_totalShrines = m_pMap->GetShrines().size();
    
    // Xóa lịch sử Undo cũ
    while (!m_historyStack.empty()) m_historyStack.pop();

    // Tính toán lại đường đi tối ưu
    std::vector<std::vector<int>> insight = ThienCoEngine::GetInstance()->CalculateInsight(m_pMap);
    DestinyResult destiny = ThienCoEngine::GetInstance()->CalculateDestiny(insight);
    m_optimalSteps = destiny.totalSteps;

    // 6. Reset Player về vị trí xuất phát mới
    if (m_pPlayer) {
        MapPoint start = m_pMap->GetStartPoint();
        int tileSize = m_pMap->GetTileSize();
        m_pPlayer->SetPosition(start.col * tileSize, start.row * tileSize);
        // Reset animation về IDLE
        // (Lưu ý: Nếu Player chưa có hàm ResetState, nó sẽ tự update trong vòng lặp sau)
    }

    std::cout << "[System] Loaded Level " << (m_currentLevelIdx + 1) << ": " << m_levelFiles[m_currentLevelIdx] << std::endl;
}

void GameEngine::NextLevel() {
    // Tăng index và load
    m_currentLevelIdx++;
    
    if (m_currentLevelIdx >= m_levelFiles.size()) {
        // Đã phá đảo toàn bộ game!
        std::cout << ">>> CHUC MUNG! BAN DA PHA DAO TOAN BO GAME! <<<" << std::endl;
        // Có thể chuyển đến một màn hình Credit đặc biệt ở đây
        // Tạm thời quay về Menu
        m_currentLevelIdx = 0;
        SwitchState(STATE_MENU);
    } else {
        LoadLevel(m_currentLevelIdx);
        SwitchState(STATE_PLAY); // Vào chơi ngay
    }
}