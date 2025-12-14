#include "Map.h"
#include "TextureManager.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>

Map::Map() {
    m_textureID = "tiles"; // ID của texture chúng ta sẽ load
    m_rows = 0;
    m_cols = 0;
    m_numShrines = 0;
    m_tileSize = 640;
}

Map::~Map() {
}

void Map::LoadMap(std::string path) {
    std::ifstream inputFile(path);

    if (!inputFile.is_open()) {
        std::cout << "[Lỗi] Khong the mo file ban do: " << path << std::endl;
        return;
    }

    // Đọc Header (R, C, N)
    if (!(inputFile >> m_rows >> m_cols >> m_numShrines)) {
        std::cout << "[Lỗi] File map khong dung dinh dang header!" << std::endl;
        return;
    }

    // Resize vector động theo kích thước R x C
    m_mapLayer.resize(m_rows, std::vector<int>(m_cols));

    // Xóa dữ liệu cũ nếu có
    m_shrines.clear();

    // Đọc lưới ký tự
    char tileChar;
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            inputFile >> tileChar;
            
            // LOGIC PHÂN LOẠI TILE
            if (tileChar == '#') {
                m_mapLayer[i][j] = 1; // 1 = Núi (Vật cản)
            } 
            else if (tileChar == 'S') {
                m_mapLayer[i][j] = 2; // 2 = Trận Nhãn (Shrine)
                
                // --- QUAN TRỌNG: LƯU TỌA ĐỘ TRẬN NHÃN ---
                m_shrines.push_back({i, j}); 
            } 
            else {
                // '.' là đất trống
                m_mapLayer[i][j] = 0; // 0 = Đất
            }
        }
    }

    m_startPoint = {0, 0};

    inputFile.close();
    std::cout << "[He thong] Map Loaded (" << m_rows << "x" << m_cols << "). Tim thay " << m_shrines.size() << " Tran Nhan." << std::endl;
}

// Kiểm tra xem một ô có phải là Trận Nhãn không
bool Map::IsShrine(int row, int col) {
    // Cách nhanh nhất: kiểm tra ID trong mảng map
    // (Vì lúc load ta đã gán ID 2 cho Shrine)
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        return m_mapLayer[row][col] == 2;
    }
    return false;
}

int Map::GetTileID(int row, int col) {
    // Kiểm tra biên an toàn
    if (row >= 0 && row < m_rows && col >= 0 && col < m_cols) {
        return m_mapLayer[row][col];
    }
    return 1; // Mặc định trả về 1 (Núi/Không đi được) nếu ra ngoài biên
}

void Map::DrawMap() {
    for (int row = 0; row < m_rows; row++) {
        for (int col = 0; col < m_cols; col++) {
            int type = m_mapLayer[row][col];
            // Vẽ đúng tại tọa độ gốc của tile
            int x = col * m_tileSize;
            int y = row * m_tileSize;

            TextureManager::GetInstance()->DrawTile(m_textureID, 0, 0, x, y, m_tileSize, m_tileSize, 1, type, GameEngine::GetInstance()->GetRenderer());
        }
    }
}