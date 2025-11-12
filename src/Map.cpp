#include "Map.h"
#include <fstream> // Dùng để đọc file (ifstream)
#include <iostream>

Map::Map(std::string tilesetID, int tileSize) {
    m_tilesetID = tilesetID;
    m_tileSize = tileSize;
    m_mapWidth = 0;
    m_mapHeight = 0;
    std::cout << "Map constructor called.\n";
}

Map::~Map() {
    std::cout << "Map destructor called.\n";
    // m_pMapData (vector) sẽ tự động được dọn dẹp
}

// --- Hàm Load (Đọc file) ---
void Map::Load(std::string filepath, int mapWidth, int mapHeight) {
    m_mapWidth = mapWidth;
    m_mapHeight = mapHeight;

    // 1. Khởi tạo ma trận dữ liệu
    m_pMapData.resize(m_mapHeight, std::vector<int>(m_mapWidth, 0));

    // 2. Mở file .txt
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "ERROR: Khong the mo file ban do: " << filepath << std::endl;
        return;
    }

    // 3. Đọc file và điền vào ma trận
    std::cout << "Loading map: " << filepath << std::endl;
    for (int y = 0; y < m_mapHeight; ++y) {
        for (int x = 0; x < m_mapWidth; ++x) {
            char tileChar;
            file.get(tileChar); // Đọc từng ký tự

            // Chuyển ký tự thành Tile ID (con số)
            switch (tileChar) {
                case '.':
                    m_pMapData[y][x] = 0;
                    break;
                case 'S':
                    m_pMapData[y][x] = 1;
                    break;
                case '#':
                    m_pMapData[y][x] = 2;
                    break;
                default:
                    // Bỏ qua các ký tự không mong muốn (như \n)
                    --x; // Quay lại 1 bước để đọc lại cho đúng cột
                    break;
            }
        }
    }

    file.close();
    std::cout << "Map loaded successfully.\n";
}

// --- Hàm Render (Vẽ) ---
void Map::Render(SDL_Renderer* renderer) { // Nhận renderer
    // Duyệt qua toàn bộ bản đồ 2D
    for (int y = 0; y < m_mapHeight; ++y) {
        for (int x = 0; x < m_mapWidth; ++x) {
            // Lấy Tile ID (0, 1, hoặc 2)
            int tileID = m_pMapData[y][x];

            // 1. Tính toán vị trí ĐÍCH (destRect) trên màn hình
            int destX = x * m_tileSize;
            int destY = y * m_tileSize;

            // 2. Tính toán vị trí NGUỒN (srcRect) từ file tileset.png
            // Giả sử tileset của chúng ta là một hàng ngang:
            // [Ô 0][Ô 1][Ô 2]
            // Ô 0 (Đất)  ở cột 0
            // Ô 1 (Nhãn) ở cột 1
            // Ô 2 (Núi)  ở cột 2
            int srcX = tileID * m_tileSize;
            int srcY = 0; // Hàng 0 của tileset

            // 3. Yêu cầu TextureManager vẽ ô gạch này
            TextureManager::GetInstance()->DrawFrame(
                m_tilesetID, // "tileset"
                destX, destY,   // Vị trí vẽ (trên màn hình)
                m_tileSize, m_tileSize, // Kích thước ô
                1, // Hàng 1 (row 1)
                tileID, // Cột (frame) thứ mấy
                renderer // Truyền renderer vào
            );
        }
    }
}