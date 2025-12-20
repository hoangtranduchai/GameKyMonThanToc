#include "Core/GameEngine.h"
#include "Core/Config.h"
#include "Entities/Player.h"
#include "Entities/Map.h"
#include "Graphics/TextureManager.h"
#include "Audio/SoundManager.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Algorithms/ThienCoEngine.h"   // Bộ não xử lý AI
#include <iostream>
#include <filesystem> // Thư viện quét file hệ thống
#include <algorithm>  // Thư viện sắp xếp
#include <regex>      // Để lọc lấy số từ tên file (level10 -> 10)

namespace fs = std::filesystem;

// Khởi tạo biến tĩnh (Static variable initialization)
GameEngine* GameEngine::s_Instance = nullptr;

// TRIỂN KHAI MẪU THIẾT KẾ SINGLETON (SINGLETON PATTERN IMPLEMENTATION)

GameEngine* GameEngine::GetInstance() {
    if (s_Instance == nullptr) s_Instance = new GameEngine();
    return s_Instance;
}

void GameEngine::DestroyInstance() {
    if (s_Instance != nullptr) {
        s_Instance->Clean();
        delete s_Instance;
        s_Instance = nullptr;
    }
}

// Hàm khởi tạo (Constructor): Thiết lập giá trị mặc định
GameEngine::GameEngine() 
    : m_bRunning(false), m_pWindow(nullptr), m_pRenderer(nullptr),
      m_deltaTime(0.0f), m_lastTime(0), m_performanceFrequency(0),
      m_pPlayer(nullptr), m_pMap(nullptr),
    m_currentLevelIdx(1),
    m_currentSteps(0), m_optimalSteps(0),
    m_shrinesCollected(0), m_totalShrines(0),
    m_resultLevelIdx(1),
      m_currentState(STATE_INTRO), 
      m_fadeAlpha(255.0f), m_isFadingIn(true), m_isFadingOut(false),
      m_isWinningSequence(false), m_winDelayTimer(0.0f),
      m_isMuted(false), m_isPendingNextLevel(false)
{
    // TỰ ĐỘNG QUÉT DANH SÁCH LEVEL
    std::string levelFolderPath = "assets/levels/";
    m_levelFiles.clear();
    try {
        if (fs::exists(levelFolderPath) && fs::is_directory(levelFolderPath)) {
            std::cout << "[HỆ THỐNG] Đang quét các cấp độ trong: " << levelFolderPath << std::endl;

            // 1. Quét tất cả file trong thư mục
            for (const auto& entry : fs::directory_iterator(levelFolderPath)) {
                // Chỉ lấy file thường (không lấy thư mục con) và có đuôi .txt
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    // Chuyển path về dạng string chuẩn (dùng / thay vì \)
                    std::string pathStr = entry.path().generic_string();
                    m_levelFiles.push_back(pathStr);
                }
            }

            // 2. Sắp xếp file: level1 -> level2 -> ... level10
            std::sort(m_levelFiles.begin(), m_levelFiles.end(), [](const std::string& a, const std::string& b) {
                // Lambda tách số từ tên file: "level10.txt" -> 10
                static const std::regex re("(\\d+)");
                std::smatch matchA, matchB;
                
                int numA = 9999, numB = 9999;
                if (std::regex_search(a, matchA, re)) numA = std::stoi(matchA.str());
                if (std::regex_search(b, matchB, re)) numB = std::stoi(matchB.str());
                
                return numA < numB;
            });

            // In ra danh sách đã tải để kiểm tra
            for (const auto& file : m_levelFiles) {
                std::cout << "  -> Đã tải: " << file << std::endl;
            }
            std::cout << "[HỆ THỐNG] Tổng số cấp độ đã tải: " << m_levelFiles.size() << std::endl;

        } else {
            std::cerr << "[LỖI] Không tìm thấy thư mục cấp độ: " << levelFolderPath << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "[NGOẠI LỆ] Lỗi hệ thống tập tin: " << e.what() << std::endl;
    }
}

GameEngine::~GameEngine() { Clean(); }

// VÒNG ĐỜI ỨNG DỤNG (APPLICATION LIFECYCLE)

bool GameEngine::Init(const char* title, int xPos, int yPos, int width, int height, bool fullscreen) {
    // 1. Khởi tạo thư viện SDL2 (Video, Audio, Timer)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Khởi tạo SDL thất bại: " << SDL_GetError() << std::endl;
        return false;
    }

    // 2. Cấu hình thuộc tính cửa sổ (OpenGL, Có thể thay đổi kích thước)
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // Chế độ toàn màn hình không viền

    // 3. Tạo cửa sổ ứng dụng
    m_pWindow = SDL_CreateWindow(title, xPos, yPos, width, height, flags);
    if (!m_pWindow) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Tạo cửa sổ thất bại: " << SDL_GetError() << std::endl;
        return false;
    }

    // 4. Tạo bộ kết xuất đồ họa (Renderer)
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_pRenderer) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Tạo bộ kết xuất đồ họa thất bại: " << SDL_GetError() << std::endl;
        return false;
    }

    // Thiết lập độ phân giải ảo (Logical Resolution)
    // Game sẽ luôn kết xuất ở độ phân giải Config::SCREEN_WIDTH x Config::SCREEN_HEIGHT,
    // nhưng tự động co giãn mượt mà lên màn hình 2K/4K
    SDL_RenderSetLogicalSize(m_pRenderer, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // 1 = Linear (Mượt), 0 = Nearest (Pixel Art)
    // Không đặt scale toàn cục để UI, nền giữ nguyên kích thước

    // 5. Khởi tạo các thư viện mở rộng
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Khởi tạo SDL_image thất bại: " << IMG_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Khởi tạo SDL_ttf thất bại: " << TTF_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Khởi tạo SDL_mixer thất bại: " << Mix_GetError() << std::endl;
        return false;
    }

    m_windowWidth = width;
    m_windowHeight = height;

    // 6. Khởi tạo bộ đếm thời gian hiệu suất cao
    m_performanceFrequency = SDL_GetPerformanceFrequency();
    m_lastTime = SDL_GetPerformanceCounter();

    // 7. Khởi tạo các phân hệ con (Subsystems)
    SoundManager::GetInstance();                        // Khởi tạo quản lý âm thanh
    
    // 8. Tải tài nguyên dùng chung
    // Phông chữ
    if (!TextureManager::GetInstance()->LoadFont("assets/fonts/Roboto-Regular.ttf", "gui_font", 32)) {
        std::cerr << "[CẢNH BÁO] Tải phông chữ thất bại!" << std::endl;
    }

    // Hàm Lambda tiện ích: Tự động tải tất cả ảnh .png trong một thư mục
    auto AutoLoadTextures = [&](const std::string& folderPath) {
        if (!fs::exists(folderPath)) {
            std::cerr << "[CẢNH BÁO] Không tìm thấy thư mục texture: " << folderPath << std::endl;
            return;
        }

        std::cout << "[TÀI NGUYÊN] Đang quét thư mục: " << folderPath << " ..." << std::endl;
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            // Chỉ xử lý file .png
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                // 1. Lấy đường dẫn file chuẩn (ví dụ: assets/images/characters/idle_down.png)
                std::string path = entry.path().generic_string();
                
                // 2. Lấy tên file làm ID (ví dụ: idle_down)
                std::string id = entry.path().stem().string();

                // 3. Tải ảnh
                bool success = TextureManager::GetInstance()->Load(path, id, m_pRenderer);
                
                if (success) {
                    std::cout << "  + Đã tải: " << id << std::endl;
                }
            }
        }
    };

    // Ảnh nền & UI (Quét tự động)
    AutoLoadTextures("assets/images/characters");
    AutoLoadTextures("assets/images/environment");
    AutoLoadTextures("assets/images/ui");
    AutoLoadTextures("assets/images/vfx");

    // Âm thanh
    SoundManager::GetInstance()->LoadMusic("assets/audio/bgm/bgm_menu.mp3", "bgm_menu");
    SoundManager::GetInstance()->LoadMusic("assets/audio/bgm/bgm_game.mp3", "bgm_game");
    SoundManager::GetInstance()->LoadSFX("assets/audio/sfx/sfx_click.wav", "sfx_click");
    SoundManager::GetInstance()->LoadSFX("assets/audio/sfx/sfx_collect.wav", "sfx_collect");
    SoundManager::GetInstance()->LoadSFX("assets/audio/sfx/sfx_lose.wav", "sfx_lose");
    SoundManager::GetInstance()->LoadSFX("assets/audio/sfx/sfx_step.wav", "sfx_step");
    SoundManager::GetInstance()->LoadSFX("assets/audio/sfx/sfx_win.wav", "sfx_win");

    // Khởi tạo Map và Player
    m_pMap = new Map();
    m_pPlayer = new Player(new LoaderParams(0, 0, Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT, "idle_down", 8, 100));

    // Bắt đầu nhạc nền Menu
    Mix_HaltMusic(); // Dừng nhạc hiện tại
    SoundManager::GetInstance()->PlayMusic("bgm_menu");

    m_bRunning = true;
    std::cout << "[HỆ THỐNG] Khởi tạo Game Engine thành công." << std::endl;
    return true;
}

void GameEngine::LoadLevel(int levelIndex) {
    int vectorIdx = levelIndex - 1;
    if (vectorIdx < 0 || vectorIdx >= (int)m_levelFiles.size()) return;

    // Đặt lại logic GAME (Reset game logic)
    while (!m_historyStack.empty()) m_historyStack.pop();
    m_visitedShrinesList.clear();
    m_currentSteps = 0;
    m_shrinesCollected = 0;
    m_currentLevelIdx = levelIndex;

    // 1. Tải bản đồ từ file
    if (!m_pMap || !m_pPlayer) return;
    
    m_pMap->LoadMap(m_levelFiles[vectorIdx]);
    
    // 2. Thiết lập vị trí nhân vật (áp dụng offset để căn giữa bản đồ)
    MapPoint startPos = m_pMap->GetStartPoint();
    // Chuyển tọa độ lưới (Grid) -> Tọa độ pixel (nhân với kích thước ô lưới) + offset
    m_pPlayer->SetPosition(
        startPos.col * Config::TILE_SIZE + m_pMap->GetOffsetX(), 
        startPos.row * Config::TILE_SIZE + m_pMap->GetOffsetY()
    );

    // 3. Tính toán đường đi tối ưu bằng AI
    // Sử dụng ThienCoEngine để giải bài toán người bán hàng (TSP)
    std::cout << "[AI] Đang tính toán cho Cấp độ " << levelIndex + 1 << "..." << std::endl;
    
    auto matrix = ThienCoEngine::GetInstance()->CalculateMatrix(m_pMap);
    auto solution = ThienCoEngine::GetInstance()->CalculateSolution(matrix);
    
    m_optimalSteps = solution.totalSteps;
    m_totalShrines = (int)m_pMap->GetShrines().size();

    std::cout << "[AI] Số bước tối ưu: " << m_optimalSteps << std::endl;

    // Lưu trạng thái đầu tiên vào lịch sử hoàn tác
    SaveState();
}

std::string GameEngine::GetCurrentStoryImageID(const std::string& suffix) const {
    // suffix ví dụ: "_name" (tên màn chơi), "_sensei" (lời dạy), "_rankS", "_rankA"
    
    // Logic: index 1 -> img_scence1
    // index 2 -> img_scence2
    int sceneNum = m_currentLevelIdx;

    // Với màn hình kết quả, luôn hiển thị theo level vừa hoàn thành
    if (m_currentState == STATE_RESULT_S || m_currentState == STATE_RESULT_A || m_currentState == STATE_RESULT_LOSE) {
        sceneNum = m_resultLevelIdx;
    }

    if (sceneNum <= 3) {
        // Vòng 1, 2, 3: Trả về đúng hình ảnh cốt truyện
        // Ví dụ: "img_scence1_name"
        return "img_scence" + std::to_string(sceneNum) + suffix;
    } else {
        // Vòng 4 trở đi: Dùng hình ảnh ngẫu nhiên từ 3 vòng đầu cho tất cả loại màn hình
        if (m_randomRankImageID.empty()) {
            int randomScene = 1 + (rand() % 3); // Ngẫu nhiên từ 1 đến 3
            const_cast<GameEngine*>(this)->m_randomRankImageID = "img_scence" + std::to_string(randomScene) + suffix;
        }
        return m_randomRankImageID;
    }
}

void GameEngine::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Sự kiện hệ thống (Quit)
        if (event.type == SDL_QUIT) m_bRunning = false;

        // Phím tắt toàn cục
        if (event.type == SDL_KEYDOWN) {

            // 1. Phím '1': Về Intro và Reset về Level 1
            if (event.key.keysym.sym == SDLK_1) {
                m_currentLevelIdx = 1; // Reset về vòng 1
                SwitchState(STATE_INTRO);
            }

            // 2. Phím '2': Bật/Tắt nhạc
            if (event.key.keysym.sym == SDLK_2) ToggleAudio();

            // 3. Phím 'u': Undo
            if (event.key.keysym.sym == SDLK_u) Undo();

            // 4. Phím 'q': Quit Game
            if (event.key.keysym.sym == SDLK_q) m_bRunning = false;

            // 5. Phím 'F11': Fullscreen
            if (event.key.keysym.sym == SDLK_F11) {
                Uint32 flags = SDL_GetWindowFlags(m_pWindow);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) SDL_SetWindowFullscreen(m_pWindow, 0);
                else SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }

        // Xử lý Input theo State
        switch (m_currentState) {
            case STATE_INTRO:
            case STATE_STORY_NAME:
            case STATE_STORY_SENSEI:
            case STATE_RESULT_S:
            case STATE_RESULT_A:
            case STATE_RESULT_LOSE:
            case STATE_CONTINUE:
                HandleStoryInput(event);
                break;

            default: break;
        }
    }
}

void GameEngine::Update() {
    // 1. Lấy thời gian hiện tại
    Uint64 currentTime = SDL_GetPerformanceCounter();
    
    // 2. Tính chênh lệch
    Uint64 diff = currentTime - m_lastTime;
    
    // 3. Chuyển sang giây
    m_deltaTime = (float)diff / (float)m_performanceFrequency;
    
    // 4. Cập nhật mốc thời gian
    m_lastTime = currentTime;

    // 5. Lag Protection: Kẹp tối đa 0.05s (20FPS) để tránh lỗi vật lý
    if (m_deltaTime > 0.05f) m_deltaTime = 0.05f;

    // 7. Hiệu ứng chuyển cảnh Fade In/Out
    if (m_isFadingOut) {
        m_fadeAlpha += FADE_SPEED * m_deltaTime * 255.0f;
        if (m_fadeAlpha >= 255.0f) {
            m_fadeAlpha = 255.0f;
            m_isFadingOut = false;
            m_isFadingIn = true;            // Bắt đầu sáng lên
            m_currentState = m_nextState;   // Chuyển trạng thái
            
            // Nếu vào game, dừng nhạc menu và phát nhạc game
            if (m_currentState == STATE_PLAY) {
                Mix_HaltMusic(); // Dừng nhạc hiện tại
                SoundManager::GetInstance()->PlayMusic("bgm_game");
            } else if (m_currentState == STATE_INTRO) {
                // Quay về Intro, dừng nhạc game và phát nhạc menu
                Mix_HaltMusic(); // Dừng nhạc hiện tại
                SoundManager::GetInstance()->PlayMusic("bgm_menu");
            } else if (m_currentState == STATE_RESULT_S || m_currentState == STATE_RESULT_A || m_currentState == STATE_RESULT_LOSE || m_currentState == STATE_CONTINUE) {
                // Vào màn hình kết quả hoặc continue, dừng nhạc game và phát nhạc nền menu
                Mix_HaltMusic();
                SoundManager::GetInstance()->PlayMusic("bgm_menu");
            } else if (m_currentState == STATE_STORY_NAME || m_currentState == STATE_CONTINUE) {
                // Reset ảnh ngẫu nhiên khi vào chu kỳ mới (name/continue -> sensei/game -> result)
                m_randomRankImageID = "";
            }
        }
    } else if (m_isFadingIn) {
        m_fadeAlpha -= FADE_SPEED * m_deltaTime * 255.0f;
        if (m_fadeAlpha <= 0.0f) {
            m_fadeAlpha = 0.0f;
            m_isFadingIn = false;
            
            // Nếu vừa fade xong vào STATE_STORY_NAME và đang chờ tăng level
            if (m_currentState == STATE_STORY_NAME && m_isPendingNextLevel) {
                m_currentLevelIdx++;
                m_isPendingNextLevel = false;
            }
        }
    }

    // 8. Logic theo từng trạng thái
    switch (m_currentState) {
        case STATE_PLAY:
            if (m_pPlayer) {
                m_pPlayer->Update(); // Nhân vật tự xử lý di chuyển và va chạm

                // Logic thua: Đi vào ô núi # (ID = 1)
                // Trừ offset để lấy tọa độ lưới thực tế
                int pRow = (int)((m_pPlayer->GetY() - m_pMap->GetOffsetY()) / Config::TILE_SIZE);
                int pCol = (int)((m_pPlayer->GetX() - m_pMap->GetOffsetX()) / Config::TILE_SIZE);

                if (m_pMap && m_pMap->GetTileID(pRow, pCol) == Config::TileID::AIR) {
                    // Âm thanh thua sẽ được phát ở Player::Falling()
                    SwitchState(STATE_RESULT_LOSE);
                }
                
                // Logic thắng: Thu thập hết Trận Nhãn
                // Logic này bây giờ được xử lý trong OnShrineVisited() để tránh phát 2 lần sfx_win
                // Chỉ cần xử lý delay timer ở đây
                
                // Logic sau khi thắng -> Chuyển sang màn kết quả
                if (m_isWinningSequence) {
                    m_winDelayTimer += m_deltaTime;
                    if (m_winDelayTimer > 1.0f) {
                        m_isWinningSequence = false;

                        // So sánh số bước đi với giải pháp AI
                        m_resultLevelIdx = m_currentLevelIdx; // Ghìm level hiển thị kết quả
                        if (m_currentSteps <= m_optimalSteps) {
                            SwitchState(STATE_RESULT_S); // Hạng S - Hoàn hảo
                        } else {
                            SwitchState(STATE_RESULT_A); // Hạng A - Thắng nhưng chưa tối ưu
                        }
                    }
                }
            }
            break;
            
        default: break;
    }
}

void GameEngine::Render() {
    // 1. Xóa màn hình
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_pRenderer);

    std::string imgID;

    // 2. Vẽ theo trạng thái
    switch (m_currentState) {
        case STATE_INTRO:
            TextureManager::GetInstance()->Draw("img_intro", 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_STORY_NAME:
            imgID = GetCurrentStoryImageID("_name");
            if (!imgID.empty()) TextureManager::GetInstance()->Draw(imgID, 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_STORY_SENSEI:
            imgID = GetCurrentStoryImageID("_sensei");
            if (!imgID.empty()) TextureManager::GetInstance()->Draw(imgID, 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_PLAY:
            // Vẽ Game
            DrawBackground();   // Vẽ nền
            if (m_pMap) m_pMap->DrawMap();  // Vẽ Map
            if (m_pPlayer) m_pPlayer->Draw();  // Vẽ Player
            break;

        case STATE_RESULT_S:
            imgID = GetCurrentStoryImageID("_rankS");
            TextureManager::GetInstance()->Draw(imgID, 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_RESULT_A:
            imgID = GetCurrentStoryImageID("_rankA");
            TextureManager::GetInstance()->Draw(imgID, 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_RESULT_LOSE:
            TextureManager::GetInstance()->Draw("img_lose", 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;

        case STATE_CONTINUE:
            TextureManager::GetInstance()->Draw("img_continue", 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
            break;
    }

    // 3. Vẽ hiệu ứng chuyển cảnh (Fade Overlay)
    if (m_fadeAlpha > 0.0f) {
        SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, (Uint8)m_fadeAlpha);
        SDL_RenderFillRect(m_pRenderer, NULL);
        SDL_SetRenderDrawBlendMode(m_pRenderer, SDL_BLENDMODE_NONE);
    }

    // Đẩy ra màn hình
    SDL_RenderPresent(m_pRenderer);
}

// HELPERS

void GameEngine::DrawBackground() {
    TextureManager::GetInstance()->Draw("bg_parallax", 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, m_pRenderer);
}

// XỬ LÝ SỰ KIỆN LOGIC

void GameEngine::OnPlayerMove() {
    m_currentSteps++;
    SoundManager::GetInstance()->PlaySFX("sfx_step");
}

void GameEngine::OnShrineVisited(int row, int col) {
    if (m_pMap->GetTileID(row, col) == Config::TileID::SHRINE) {
        m_shrinesCollected++;
        m_pMap->SetTileID(row, col, Config::TileID::GROUND); 
        m_visitedShrinesList.push_back({row, col});
        
        // Kiểm tra nếu thu thập hết shrine
        if (m_shrinesCollected >= m_totalShrines) {
            // Kích hoạt trạng thái thắng ngay tại đây để tránh phát 2 lần
            if (!m_isWinningSequence) {
                m_isWinningSequence = true;
                m_winDelayTimer = 0.0f;
                SoundManager::GetInstance()->PlaySFX("sfx_win");
            }
        } else {
            // Chỉ phát sfx_collect nếu chưa thu thập hết
            SoundManager::GetInstance()->PlaySFX("sfx_collect");
        }
        
        std::cout << "[GAME] Đã thu thập trấn nhãn tại (" << row << "," << col << ")" << std::endl;
    }
}

// HỆ THỐNG UNDO

void GameEngine::SaveState() {
    GameStateMoment moment;
    // Chuyển pixel -> grid
    moment.playerGridRow = (int)((m_pPlayer->GetY() - m_pMap->GetOffsetY()) / Config::TILE_SIZE);
    moment.playerGridCol = (int)((m_pPlayer->GetX() - m_pMap->GetOffsetX()) / Config::TILE_SIZE);
    moment.currentSteps = m_currentSteps;
    moment.shrinesCollected = m_shrinesCollected;
    moment.currentLevelIdx = m_currentLevelIdx;
    moment.visitedShrinesSnapshot = m_visitedShrinesList;
    m_historyStack.push(moment);
}

void GameEngine::Undo() {
    // 1. Nếu đang chơi: Undo bước đi
    if (m_currentState == STATE_PLAY) {
        if (m_historyStack.size() > 1) { // Phải còn ít nhất trạng thái Start
            m_historyStack.pop(); // Bỏ trạng thái hiện tại
            GameStateMoment prev = m_historyStack.top(); // Lấy trạng thái cũ

            // Khôi phục Player
            m_pPlayer->SetPosition(prev.playerGridCol * Config::TILE_SIZE + m_pMap->GetOffsetX(),
                                   prev.playerGridRow * Config::TILE_SIZE + m_pMap->GetOffsetY());
            m_currentSteps = prev.currentSteps;
            m_currentLevelIdx = prev.currentLevelIdx; // Khôi phục level index
            
            // Khôi phục Map (Các Trận Nhãn đã ăn phải hiện lại nếu undo)
            // Cơ chế: Khôi phục lại tất cả về chưa thu thập (ID 2), sau đó duyệt snapshot để đánh dấu đã thu thập (ID 0)
            const std::vector<MapPoint>& allShrines = m_pMap->GetShrines();
            // Khôi phục trạng thái chưa thu thập
            for (const auto& s : allShrines) m_pMap->SetTileID(s.row, s.col, Config::TileID::SHRINE);
            // Đánh dấu lại các trận nhãn đã thu thập trong quá khứ
            for (const auto& s : prev.visitedShrinesSnapshot) m_pMap->SetTileID(s.first, s.second, Config::TileID::GROUND);
            
            m_shrinesCollected = prev.shrinesCollected;
            m_visitedShrinesList = prev.visitedShrinesSnapshot;

            std::cout << "[GAME] Hoàn tác thành công!" << std::endl;
        }
    }
    // 2. Nếu đang ở màn hình khác nhưng có trạng thái game đã lưu: Quay lại game
    else if (m_hasPreviousGameState) {
        // Khôi phục trạng thái game
        m_pPlayer->SetPosition(m_previousGameState.playerGridCol * Config::TILE_SIZE + m_pMap->GetOffsetX(),
                               m_previousGameState.playerGridRow * Config::TILE_SIZE + m_pMap->GetOffsetY());
        m_currentSteps = m_previousGameState.currentSteps;
        m_currentLevelIdx = m_previousGameState.currentLevelIdx; // Khôi phục level index
        
        // Khôi phục Map
        const std::vector<MapPoint>& allShrines = m_pMap->GetShrines();
        for (const auto& s : allShrines) m_pMap->SetTileID(s.row, s.col, Config::TileID::SHRINE);
        for (const auto& s : m_previousGameState.visitedShrinesSnapshot) 
            m_pMap->SetTileID(s.first, s.second, Config::TileID::GROUND);
        
        m_shrinesCollected = m_previousGameState.shrinesCollected;
        m_visitedShrinesList = m_previousGameState.visitedShrinesSnapshot;

        // Đánh dấu đã khôi phục xong
        m_hasPreviousGameState = false;

        // Quay lại game
        m_currentState = STATE_PLAY;
        m_nextState = STATE_PLAY;
        m_isFadingOut = false;
        m_isFadingIn = false;
        m_fadeAlpha = 0.0f;

        // Dừng nhạc menu và phát lại nhạc game
        Mix_HaltMusic();
        SoundManager::GetInstance()->PlayMusic("bgm_game");

        // Xóa lịch sử state để không loop
        while (!m_stateHistory.empty()) m_stateHistory.pop();

        std::cout << "[GAME] Quay lại game thành công!" << std::endl;
    }
    // 3. Nếu đang ở màn hình khác (và không có trạng thái game): Quay về màn hình trước
    else {
        if (!m_stateHistory.empty()) {
            GameState prevState = m_stateHistory.top();
            m_stateHistory.pop();
            
            // Chuyển state trực tiếp (không lưu lịch sử để tránh loop)
            m_currentState = prevState; 
            m_nextState = prevState; 
            m_isFadingOut = false; 
            m_isFadingIn = true; 
            m_fadeAlpha = 255.0f; // Bắt đầu fade in
        }
    }
}

// Bật/Tắt âm thanh
void GameEngine::ToggleAudio() {
    // Đảo trạng thái: Đang tắt -> Bật, Đang bật -> Tắt
    m_isMuted = !m_isMuted;
    SoundManager::GetInstance()->SetMute(m_isMuted);
    std::cout << "[ÂM THANH] Trạng thái tắt tiếng: " << (m_isMuted ? "BẬT" : "TẮT") << std::endl;
}

// CÁC HÀM XỬ LÝ SỰ KIỆN ĐẦU VÀO (INPUT HANDLERS)

void GameEngine::HandleStoryInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        SoundManager::GetInstance()->PlaySFX("sfx_click");

        switch (m_currentState) {
            case STATE_INTRO:
                // Intro -> Bắt đầu vào chuỗi Level 1
                m_currentLevelIdx = 1;
                SwitchState(STATE_STORY_NAME);
                break;

            case STATE_STORY_NAME:
                SwitchState(STATE_STORY_SENSEI);
                break;

            case STATE_STORY_SENSEI:
                LoadLevel(m_currentLevelIdx); // Tải dữ liệu bản đồ
                SwitchState(STATE_PLAY);      // Vào chơi
                break;

            case STATE_RESULT_S:
                // Hạng S -> Chuyển sang màn tiếp theo (hoặc continue sau vòng 3)
                NextLevel();
                break;

            case STATE_RESULT_A:
            case STATE_RESULT_LOSE:
                // Thua hoặc Hạng A -> Chơi lại màn hiện tại (bao gồm vòng 3)
                LoadLevel(m_currentLevelIdx);
                SwitchState(STATE_PLAY);
                break;

            case STATE_CONTINUE:
                {
                    // Từ continue -> vào vòng kế tiếp trực tiếp (không cần NAME/SENSEI)
                    int nextLevel = m_currentLevelIdx + 1;
                    if (nextLevel > (int)m_levelFiles.size()) {
                        SwitchState(STATE_INTRO);
                        break;
                    }

                    m_currentLevelIdx = nextLevel;
                    LoadLevel(m_currentLevelIdx);
                    SwitchState(STATE_PLAY);
                }
                break;
                
            default: break;
        }
    }
}

void GameEngine::SwitchState(GameState newState) {
    if (m_currentState == newState) return;
    // Lưu trạng thái cũ vào lịch sử để Undo (trừ khi đang Fade)
    if (!m_isFadingOut) {
        m_stateHistory.push(m_currentState);
    }

    // Nếu đang rời khỏi STATE_PLAY sang các màn hình khác, lưu trạng thái game hiện tại
    // để có thể quay lại bằng phím U
    if (m_currentState == STATE_PLAY && newState != STATE_PLAY) {
        m_hasPreviousGameState = true;
        m_previousGameState.playerGridRow = (int)((m_pPlayer->GetY() - m_pMap->GetOffsetY()) / Config::TILE_SIZE);
        m_previousGameState.playerGridCol = (int)((m_pPlayer->GetX() - m_pMap->GetOffsetX()) / Config::TILE_SIZE);
        m_previousGameState.currentSteps = m_currentSteps;
        m_previousGameState.shrinesCollected = m_shrinesCollected;
        m_previousGameState.currentLevelIdx = m_currentLevelIdx;
        m_previousGameState.visitedShrinesSnapshot = m_visitedShrinesList;
    }

    // Reset random rank image when bước sang các màn hình hiển thị/cycle mới
    if (newState == STATE_RESULT_S || newState == STATE_RESULT_A || newState == STATE_RESULT_LOSE ||
        newState == STATE_STORY_NAME || newState == STATE_CONTINUE) {
        m_randomRankImageID.clear();
    }

    m_nextState = newState;
    m_isFadingOut = true; // Kích hoạt hiệu ứng Fade Out
}

void GameEngine::NextLevel() {
    int nextLevel = m_currentLevelIdx + 1;

    // Không còn level tiếp theo
    if (nextLevel > (int)m_levelFiles.size()) {
        m_isPendingNextLevel = false;
        SwitchState(STATE_INTRO);
        return;
    }

    // Kết thúc vòng 3 với Rank S -> màn Continue rồi chơi vòng 4
    if (m_currentLevelIdx == 3) {
        m_isPendingNextLevel = false;
        SwitchState(STATE_CONTINUE);
        return;
    }

    if (m_currentLevelIdx < 3) {
        // Vòng 1-2: tăng level và đi qua màn tên/sensei
        m_currentLevelIdx = nextLevel;
        m_isPendingNextLevel = false;
        SwitchState(STATE_STORY_NAME);
        return;
    }

    // Vòng 4 trở đi: tăng level và vào thẳng game
    m_currentLevelIdx = nextLevel;
    m_isPendingNextLevel = false;
    LoadLevel(m_currentLevelIdx);
    SwitchState(STATE_PLAY);
}

void GameEngine::Clean() {
    if (m_pMap) delete m_pMap;
    if (m_pPlayer) delete m_pPlayer;
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}