#include "Entities/Map.h"
#include "Graphics/TextureManager.h"
#include "Core/GameEngine.h" // Để truy cập bộ kết xuất đồ họa (Renderer)
#include <fstream>
#include <iostream>

// HÀM KHỞI TẠO & HỦY (CONSTRUCTOR & DESTRUCTOR)

Map::Map() : m_rows(0), m_cols(0), m_offsetX(0), m_offsetY(0) {
    // Vị trí mặc định
    m_startPoint = {0, 0};
}

Map::~Map() {}

// XỬ LÝ DỮ LIỆU (DATA PROCESSING)

void Map::LoadMap(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[LỖI] Không thể mở file bản đồ: " << path << std::endl;
        return;
    }

    // 1. Đọc kích thước bản đồ và tổng trận nhãn
    int totalShrines = 0;
    file >> m_rows >> m_cols >> totalShrines;

    if (m_rows <= 0 || m_cols <= 0) {
        std::cerr << "[LỖI NGHIÊM TRỌNG] Kích thước bản đồ không hợp lệ!" << std::endl;
        return;
    }

    // Thay đổi kích thước ma trận
    m_mapMatrix.resize(m_rows);
    for (int i = 0; i < m_rows; i++) {
        m_mapMatrix[i].resize(m_cols);
    }

    // Xóa bộ nhớ đệm cũ
    m_shrines.clear();

    m_startPoint = {0, 0};

    /// 2. Đọc dữ liệu Grid (KÝ TỰ -> ID)
    char tileChar;
    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            // Đọc từng ký tự, bỏ qua khoảng trắng/xuống dòng
            file >> tileChar;
            
            int tileID = Config::TileID::GROUND; // Mặc định là đất (0)

            switch (tileChar) {
                case '.': // Đất
                    tileID = Config::TileID::GROUND; // 0
                    break;
                
                case '#': // Chân không (Air)
                    tileID = Config::TileID::AIR;   // 1
                    break;

                case 'S': // Trận Nhãn (Shrine)
                    tileID = Config::TileID::SHRINE; // 2
                    break;

                default:
                    // Ký tự lạ -> coi như đất
                    tileID = Config::TileID::GROUND; 
                    break;
            }

            // Xử lý logic đặc biệt
            if (tileID == Config::TileID::SHRINE) { // Mã 2: Trận nhãn
                m_shrines.push_back(MapPoint(row, col));
            }

            // Lưu vào ma trận
            m_mapMatrix[row][col] = tileID;
        }
    }

    file.close();
    std::cout << "[HỆ THỐNG] Đã tải bản đồ: " << path << " [" << m_rows << "x" << m_cols << "]" << std::endl;
    std::cout << "[THÔNG TIN] Số trận nhãn phát hiện: " << m_shrines.size() << std::endl;

    // Tính toán offset để căn giữa bản đồ trên màn hình, chừa margin đáy
    int mapWidth = m_cols * Config::TILE_SIZE;   // Chiều rộng toàn bộ bản đồ (pixel)
    int mapHeight = m_rows * Config::TILE_SIZE;  // Chiều cao toàn bộ bản đồ (pixel)
    int availableHeight = Config::SCREEN_HEIGHT - Config::MAP_BOTTOM_MARGIN;
    
    m_offsetX = (Config::SCREEN_WIDTH - mapWidth) / 2;   // Căn giữa theo trục X
    m_offsetY = (availableHeight - mapHeight) / 2;       // Căn giữa theo trục Y trong vùng còn lại
    if (m_offsetY < 0) m_offsetY = 0;                    // Không để map lệch âm
}

// KẾT XUẤT ĐỒ HỌA (RENDERING)

void Map::DrawMap() {
    int tileID = 0;

    SDL_Renderer* pRenderer = GameEngine::GetInstance()->GetRenderer();

    const float S = GetRenderScale(); // Tính tự động theo kích thước map

    // Tính offset căn giữa theo tỷ lệ phóng
    int mapWidth = m_cols * Config::TILE_SIZE;
    int mapHeight = m_rows * Config::TILE_SIZE;
    int centerOffsetX = static_cast<int>((Config::SCREEN_WIDTH - mapWidth * S) / 2.0f);
    int centerOffsetY = static_cast<int>((Config::SCREEN_HEIGHT - Config::MAP_BOTTOM_MARGIN - mapHeight * S) / 2.0f);
    if (centerOffsetY < 0) centerOffsetY = 0; // Không để lệch âm khi scale lớn

    // Dùng tileset.png
    // Mapping:
    // ID 0 (Ground) -> Frame 0 (Xanh lá)
    // ID 1 (Wall)   -> Frame 1 (Xám)
    // ID 2 (Shrine) -> Frame 2 (Xanh dương)

    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            tileID = m_mapMatrix[row][col];

            // Tọa độ vẽ trên màn hình (áp dụng offset để căn giữa)
            int x = static_cast<int>(col * Config::TILE_SIZE * S) + centerOffsetX;
            int y = static_cast<int>(row * Config::TILE_SIZE * S) + centerOffsetY;

            // VẼ BẰNG PHƯƠNG THỨC DRAW FRAME (CẮT TỪ TILESET)
            
            if (tileID >= 0) {
                TextureManager::GetInstance()->DrawFrameScaled(
                    "tileset",          // ID của Texture
                    x, y,               // Vị trí vẽ trên màn hình
                    Config::TILE_SIZE,  // Kích thước frame nguồn
                    Config::TILE_SIZE,  // Kích thước frame nguồn
                    S,                  // Tỷ lệ phóng đích
                    0,                  // Hàng số 0 trong tileset
                    tileID,             // Cột (Frame) tương ứng với ID
                    pRenderer           // Bộ kết xuất đồ họa
                );
            }
        }
    }
}

// TRUY XUẤT THÔNG TIN (DATA ACCESS)

int Map::GetTileID(int row, int col) const {
    // Kiểm tra biên (Bounds Check) để tránh lỗi tràn mảng và crash game
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        return m_mapMatrix[row][col];
    }
    return Config::TileID::AIR; // Nếu truy vấn ngoài phạm vi bản đồ, coi như là ô không khí
}

void Map::SetTileID(int row, int col, int id) {
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        m_mapMatrix[row][col] = id;
    }
}

float Map::GetRenderScale() const {
    // Tính tỷ lệ phóng để map vừa khít màn hình với margin dưới 68px
    int mapWidth = m_cols * Config::TILE_SIZE;
    int mapHeight = m_rows * Config::TILE_SIZE;
    
    // Chiều cao sẵn có (trừ margin dưới)
    int availableHeight = Config::SCREEN_HEIGHT - Config::MAP_BOTTOM_MARGIN;
    
    // Scale để vừa với chiều rộng và chiều cao có sẵn
    float scaleX = (Config::SCREEN_WIDTH * 0.9f) / mapWidth;
    float scaleY = (float)availableHeight / mapHeight;
    
    // Chọn scale nhỏ hơn để không vượt quá vùng có sẵn
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    
    // Giới hạn tối thiểu 0.5 (không giới hạn tối đa để map lớn vẫn vừa margin)
    if (scale < 0.5f) scale = 0.5f;
    
    return scale;
}