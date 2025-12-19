#include "Entities/Map.h"
#include "Graphics/TextureManager.h"
#include "Core/GameEngine.h" // Để truy cập bộ kết xuất đồ họa (Renderer)
#include <fstream>
#include <iostream>

// HÀM KHỞI TẠO & HỦY (CONSTRUCTOR & DESTRUCTOR)

Map::Map() : m_rows(0), m_cols(0) {
    m_startPoint = {1, 1}; // Điểm xuất phát mặc định
}

Map::~Map() {}

// XỬ LÝ DỮ LIỆU (DATA PROCESSING)

void Map::LoadMap(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[LỖI] Không thể mở file bản đồ: " << path << std::endl;
        return;
    }

    // 1. Đọc kích thước bản đồ
    file >> m_rows >> m_cols;

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

    // 2. Đọc dữ liệu từng ô lưới (Grid Data)
    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            int tileCode = 0;
            file >> tileCode; // Đọc mã số từ file
            
            // Xử lý logic đặc biệt
            if (tileCode == Config::TileID::SHRINE) { // Mã 2: Trận nhãn
                m_shrines.push_back(MapPoint(row, col));
            }

            // Lưu vào ma trận
            m_mapMatrix[row][col] = tileCode;
        }
    }

    file.close();
    std::cout << "[HỆ THỐNG] Đã tải bản đồ: " << path << " [" << m_rows << "x" << m_cols << "]" << std::endl;
    std::cout << "[THÔNG TIN] Số trận nhãn phát hiện: " << m_shrines.size() << std::endl;
}

// KẾT XUẤT ĐỒ HỌA (RENDERING)

void Map::DrawMap() {
    int tileID = 0;

    // Lấy bộ kết xuất một lần để dùng cho vòng lặp
    SDL_Renderer* pRenderer = GameEngine::GetInstance()->GetRenderer();

    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            tileID = m_mapMatrix[row][col];

            // Tọa độ vẽ trên màn hình
            int x = col * Config::TILE_SIZE;
            int y = row * Config::TILE_SIZE;

            // VẼ BẰNG PHƯƠNG THỨC DRAW FRAME (CẮT TỪ TILESET)
            
            if (tileID >= 0) {
                TextureManager::GetInstance()->DrawFrame(
                    "tileset",          // ID của Texture
                    x, y,               // Vị trí vẽ trên màn hình
                    Config::TILE_SIZE,  // Chiều rộng một ô
                    Config::TILE_SIZE,  // Chiều cao một ô
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