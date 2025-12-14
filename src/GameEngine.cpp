#include "GameEngine.h"
#include "Player.h"
#include "Map.h"
#include "TextureManager.h"
#include "ThienCoEngine.h"
#include <string>
#include "SoundManager.h"
#include "ParticleSystem.h"

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

    // Load Tileset Image
    std::string tilePath = std::string(PROJECT_ROOT_PATH) + "/assets/images/tiles.png";
    if (!TextureManager::GetInstance()->Load(tilePath, "tiles", m_pRenderer)) return false;

    // Khởi tạo Map
    m_pMap = new Map();
    std::string mapPath = std::string(PROJECT_ROOT_PATH) + "/assets/maps/level1.txt"; 
    m_pMap->LoadMap(mapPath);

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

    // --- LOAD FONT CHO HUD ---
    // Vì Logical Height có thể lên tới 2000-3000px, Font size phải to tương ứng
    // Công thức: Size = LogicalHeight / 20 (Ví dụ)
    int fontSize = m_pMap->GetMapPixelHeight() / 15; 
    std::string fontPath = std::string(PROJECT_ROOT_PATH) + "/assets/fonts/Roboto-Regular.ttf"; 
    
    // Nếu chưa có file font, hãy tạo folder assets/fonts và copy file ttf vào!
    // Ở đây tôi giả định bạn sẽ làm việc đó.
    TextureManager::GetInstance()->LoadFont(fontPath, "gui_font", fontSize);

    // --- [AAA SYSTEM] NẠP SPRITE SHEET PLAYER ---
    // Định nghĩa các hành động và hướng tương ứng với tên file ảnh
    std::string actions[] = {"idle", "run"};
    std::string directions[] = {"down", "left", "right", "up"};
    
    // Vòng lặp tự động nạp tất cả ảnh
    for (const auto& act : actions) {
        for (const auto& dir : directions) {
            // Xây dựng đường dẫn file: assets/images/player/[ACTION]/[action]_[dir].png
            
            // Ví dụ path: assets/images/player/idle/idle_down.png
            std::string folderName = act; // idle, run, attack1...
            std::string fileName = act + "_" + dir + ".png";
            
            std::string fullPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player/" + folderName + "/" + fileName;
            
            // ID trong TextureManager: "player_idle_down", "player_run_left"...
            std::string textureID = "player_" + act + "_" + dir;
            
            if (!TextureManager::GetInstance()->Load(fullPath, textureID, m_pRenderer)) {
                std::cout << "[Canh bao] Khong tim thay: " << fullPath << std::endl;
            }
        }
    }

    // --- KHỞI TẠO PLAYER ---
    // Lưu ý: TextureID ban đầu là "player_idle_down"
    // Kích thước frame cần đo chính xác từ ảnh (Ví dụ ảnh RUN 8 frame rộng 5120px -> 1 frame = 640px)
    int frameW = 96;
    int frameH = 80;

    MapPoint startPos = m_pMap->GetStartPoint();
    int tileSize = m_pMap->GetTileSize();

    // Tạo Player tại vị trí bắt đầu
    m_pPlayer = new Player(new LoaderParams(startPos.col * tileSize, startPos.row * tileSize, frameW, frameH, "player_idle_down"));

    std::cout << "Player spawned at Grid [" << startPos.row << "," << startPos.col << "]" << std::endl;

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
                // Nhấn ENTER để BẮT ĐẦU
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    m_currentState = STATE_PLAY;
                    SoundManager::GetInstance()->PlaySFX("collect"); // Tiếng xác nhận
                    // Bắt đầu nhạc nền game
                    SoundManager::GetInstance()->PlayMusic("bgm");
                }
                break;

            case STATE_PLAY:
                // Các phím điều khiển nhân vật chỉ hoạt động ở đây
                if (event.type == SDL_KEYDOWN) {
                    // Undo
                    if (event.key.keysym.sym == SDLK_u) Undo();
                    // Di chuyển (đã xử lý trong Player::HandleInput gọi ở Update, 
                    // nhưng Player chỉ nên nhận input khi ở State Play)
                }
                break;

            case STATE_WIN:
                // Nhấn R để CHƠI LẠI (Replay)
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                    ResetGame();
                }
                break;
        }
    }
}

// Cập nhật logic Game
void GameEngine::Update() {
    // Tính toán Delta Time
    static Uint32 lastTick = SDL_GetTicks(); // Biến static cục bộ để giữ giá trị giữa các lần gọi
    Uint32 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - lastTick) / 1000.0f;
    lastTick = currentTick;

    // Đặt giới hạn DeltaTime để tránh lỗi khi pause/debug quá lâu
    if (m_deltaTime > 0.05f) { // Ví dụ: Giới hạn 50ms/frame (20 FPS tối thiểu)
        m_deltaTime = 0.05f;
    }
    
    // Chỉ update nhân vật khi đang CHƠI
    if (m_currentState == STATE_PLAY) {
        if (m_pPlayer) m_pPlayer->Update();
    }

    // Hiệu ứng nhấp nháy cho Menu và Win screen
    if (m_currentState != STATE_PLAY) {
        m_blinkTimer += m_deltaTime;
        if (m_blinkTimer > 1.0f) m_blinkTimer = 0.0f;
    }

    ParticleSystem::GetInstance()->Update(m_deltaTime);
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
    
    std::cout << "[He thong] Game da duoc Reset!" << std::endl;
}

void GameEngine::OnShrineVisited(int row, int col) {
    // Kiểm tra xem shrine này đã mở chưa
    for (auto& pos : m_visitedShrinesList) {
        if (pos.first == row && pos.second == col) return; // Đã mở rồi
    }

    // Đánh dấu đã mở
    m_visitedShrinesList.push_back({row, col});
    m_shrinesCollected++;

    // --- HIỆU ỨNG HÌNH ẢNH AAA ---
    // Khi ăn xong, biến Trận Nhãn (ID 2) thành Đất Thường (ID 0)
    // Để người chơi thấy Trận Nhãn "biến mất" hoặc "tắt sáng"
    m_pMap->SetTileID(row, col, 0);
    
    // Phát hiệu ứng âm thanh khi khai mở Trận Nhãn
    SoundManager::GetInstance()->PlaySFX("collect");

    // --- KÍCH HOẠT HIỆU ỨNG NĂNG LƯỢNG (AAA MOMENT) ---
    int tileSize = m_pMap->GetTileSize();
    // Tính tâm của ô Trận Nhãn
    int centerX = col * tileSize + tileSize / 2;
    int centerY = row * tileSize + tileSize / 2;
    
    // Màu xanh ngọc (Cyan) rực rỡ giống vũ khí nhân vật
    SDL_Color magicColor = {0, 255, 255, 255}; 
    
    // Bắn ra 500 hạt năng lượng
    ParticleSystem::GetInstance()->Emit(centerX, centerY, 500, magicColor);
    
    // Có thể thêm màu vàng kim loại (Gold) xen kẽ
    ParticleSystem::GetInstance()->Emit(centerX, centerY, 200, {255, 215, 0, 255});
    
    std::cout << ">>> DA KHAI MO TRAN NHAN! (" << m_shrinesCollected << "/" << m_totalShrines << ")" << std::endl;

    // Kiểm tra Chiến Thắng
    if (m_shrinesCollected >= m_totalShrines) {
        m_currentState = STATE_WIN; // <--- CHUYỂN TRẠNG THÁI
        SoundManager::GetInstance()->PlaySFX("win_sound"); // Nếu có
    }
}

// Render Đồ họa
void GameEngine::Render() {
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);

    // Lấy kích thước màn hình logic
    int w = m_pMap->GetMapPixelWidth();
    int h = m_pMap->GetMapPixelHeight();

    // --- VẼ HIỆU ỨNG HẠT (VFX) ---
    ParticleSystem::GetInstance()->Render(m_pRenderer);

    SDL_Color gold = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    switch (m_currentState) {
        case STATE_MENU: {
            // 1. Vẽ Background mờ ảo (có thể vẽ Map làm nền)
            if (m_pMap) m_pMap->DrawMap();
            
            // 2. Vẽ Tiêu Đề Lớn "KỲ MÔN THẦN TỐC"
            // (Bạn nên tạo font size lớn hơn trong Init cho đẹp)
            TextureManager::GetInstance()->DrawText("gui_font", "KY MON THAN TOC", w/2 - 300, h/3, gold, m_pRenderer);
            
            // 3. Vẽ dòng chữ nhấp nháy
            if (m_blinkTimer < 0.5f) {
                TextureManager::GetInstance()->DrawText("gui_font", "Nhan ENTER de Nhap The", w/2 - 250, h/2, white, m_pRenderer);
            }
            break;
        }

        case STATE_PLAY: {
            // 1. Vẽ Map
            if (m_pMap) m_pMap->DrawMap();

            // 2. Vẽ Player
            if (m_pPlayer) m_pPlayer->Draw();
            
            // 3. Vẽ Hạt (VFX)
            ParticleSystem::GetInstance()->Render(m_pRenderer);
            
            // 4. Vẽ HUD
            // --- VẼ HUD (GIAO DIỆN NGƯỜI DÙNG) ---
            // Màu chữ: Trắng sáng (hoặc Vàng kim loại cho sang trọng)
            SDL_Color textColor = {255, 215, 0, 255}; // Gold Color

            // Chuẩn bị nội dung text
            std::string stepText = "Buoc: " + std::to_string(m_currentSteps) + " / " + std::to_string(m_optimalSteps);
            std::string shrineText = "Tran Nhan: " + std::to_string(m_shrinesCollected) + " / " + std::to_string(m_totalShrines);

            // Lấy kích thước Map để căn chỉnh vị trí (Vẽ ở góc trên trái)
            int margin = m_pMap->GetMapPixelHeight() / 30;
            
            TextureManager::GetInstance()->DrawText("gui_font", stepText, margin, margin, textColor, m_pRenderer);
            TextureManager::GetInstance()->DrawText("gui_font", shrineText, margin, margin * 3, textColor, m_pRenderer);
            break;
        }

        case STATE_WIN: {
            // Vẽ Map và Player đứng yên làm nền
            // (Giữ nguyên thứ tự Map -> Player -> VFX -> UI)
            if (m_pMap) m_pMap->DrawMap();
            if (m_pPlayer) m_pPlayer->Draw();
            ParticleSystem::GetInstance()->Render(m_pRenderer);

            // Vẽ bảng chiến thắng
            TextureManager::GetInstance()->DrawText("gui_font", "THIEN MENH HOAN TAT!", w/2 - 300, h/3, gold, m_pRenderer);
            
            std::string resultStr = "Thanh tich: " + std::to_string(m_currentSteps) + " buoc";
            TextureManager::GetInstance()->DrawText("gui_font", resultStr, w/2 - 200, h/2, white, m_pRenderer);

            if (m_blinkTimer < 0.5f) {
                TextureManager::GetInstance()->DrawText("gui_font", "Nhan 'R' de Choi Lai", w/2 - 220, h/2 + 150, white, m_pRenderer);
            }
            break;
        }
    }

    SDL_RenderPresent(m_pRenderer);
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