#include "Map.h"
#include "TextureManager.h"
#include "GameEngine.h"

Map::Map() {
    m_textureID = "tiles"; // ID của texture chúng ta sẽ load
}

Map::~Map() {
}

void Map::LoadMap(int arr[20][25]) {
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 25; col++) {
            m_map[row][col] = arr[row][col];
        }
    }
}

void Map::DrawMap() {
    int type = 0;
    
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 25; col++) {
            type = m_map[row][col]; // Lấy loại địa hình (0: Nước, 1: Đất, 2: Núi)

            // Vị trí vẽ trên màn hình (Tile size = 32x32)
            // Lưu ý: row là y, col là x
            int x = col * 32;
            int y = row * 32;

            // Gọi hàm DrawTile mới
            // width=32, height=32, currentRow=1, currentFrame=type
            TextureManager::GetInstance()->DrawTile(m_textureID, 0, 0, x, y, 32, 32, 1, type, GameEngine::GetInstance()->GetRenderer());
        }
    }
}