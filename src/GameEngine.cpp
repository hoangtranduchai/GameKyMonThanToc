#include "GameEngine.h"
#include "Player.h"
#include <string>

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
    if (!TextureManager::GetInstance()->Load(bgPath, "background", m_pRenderer)) {
        return false; 
    }

    // Load Player
    std::string playerPath = std::string(PROJECT_ROOT_PATH) + "/assets/images/player.png";
    if (!TextureManager::GetInstance()->Load(playerPath, "player", m_pRenderer)) {
        return false;
    }

    // Khởi tạo đối tượng Player
    int orgW, orgH;
    TextureManager::GetInstance()->GetTextureSize("player", &orgW, &orgH);
    
    // Tạo đối tượng Player với kích thước scale 20%
    double scale = 0.2;
    m_pPlayer = new Player(new LoaderParams(100, 100, (int)(orgW * 0.2), (int)(orgH * 0.2), "player"));

    // Load Tileset Image
    std::string tilePath = std::string(PROJECT_ROOT_PATH) + "/assets/images/tiles.png";
    if (!TextureManager::GetInstance()->Load(tilePath, "tiles", m_pRenderer)) return false;

    // Khởi tạo Map
    m_pMap = new Map();

    // Tạo dữ liệu bản đồ mẫu (Cấp độ 1)
    // 0: Nước, 1: Đất, 2: Núi
    int level1[20][25] = {0};
    for(int i=0; i<20; i++) {
        for(int j=0; j<25; j++) {
            if (i==0 || j==0 || i==19 || j==24) level1[i][j] = 2; // Tường bao quanh
            else level1[i][j] = 1; // Đất ở giữa
        }
    }
    level1[10][10] = 2; // Thêm hòn núi giữa nhà

    m_pMap->LoadMap(level1);

    std::cout << "Game Engine & Player Objects khoi tao thanh cong!" << std::endl;
    m_bRunning = true;
    return true;
}

// Xử lý sự kiện
void GameEngine::HandleEvents() {
    SDL_Event event;
    // Xử lý tất cả các sự kiện trong hàng đợi
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_bRunning = false;
        }
        
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            m_bRunning = false;
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
    
    // Cập nhật Player
    if (m_pPlayer) {
        m_pPlayer->Update();
    }
}

// Render Đồ họa
void GameEngine::Render() {
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);
    
    // Vẽ Background (Dùng Draw tĩnh)
    // Vẽ full màn hình: x=0, y=0, w=windowWidth, h=windowHeight
    TextureManager::GetInstance()->Draw("background", 0, 0, m_windowWidth, m_windowHeight, m_pRenderer);

    // Vẽ Map
    if (m_pMap) {
        m_pMap->DrawMap();
    }

    // Vẽ Player (Dùng Draw tĩnh hoặc DrawFrame nếu muốn animation)
    if (m_pPlayer) {
        m_pPlayer->Draw();
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