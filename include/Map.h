#pragma once

#include <string>
#include <vector>
#include "TextureManager.h" // Chúng ta sẽ cần gọi TextureManager

class Map {
private:
    std::string m_tilesetID; // ID của texture (vd: "tileset")
    int m_tileSize; // Kích thước 1 ô (vd: 32)
    int m_mapWidth, m_mapHeight; // Kích thước bản đồ (số ô)

    // Ma trận 2D lưu trữ toàn bộ bản đồ
    // Chúng ta sẽ map: '.' -> 0, 'S' -> 1, '#' -> 2
    std::vector<std::vector<int>> m_pMapData; 

public:
    // Constructor: nhận vào ID của tileset (đã load ở PlayState)
    // và kích thước của một ô gạch (tile)
    Map(std::string tilesetID, int tileSize);
    
    ~Map();

    // Hàm đọc file .txt và chuyển nó thành một ma trận 2D
    void Load(std::string filepath, int mapWidth, int mapHeight);

    // Hàm vẽ bản đồ lên màn hình
    void Render(SDL_Renderer* renderer); // Truyền renderer vào
};