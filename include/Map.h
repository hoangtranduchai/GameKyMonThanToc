#pragma once
#include <SDL.h>
#include <string>
#include <vector>

class Map {
public:
    Map();
    ~Map();

    // Tải bản đồ (Hiện tại ta dùng mảng cứng, sau này sẽ đọc file .txt)
    void LoadMap(int arr[20][25]);
    
    // Vẽ bản đồ ra màn hình
    void DrawMap();

private:
    SDL_Rect m_src, m_dest;
    
    // ID của texture tileset
    std::string m_textureID;

    // Mảng 2 chiều chứa dữ liệu bản đồ (20 hàng, 25 cột - phù hợp màn hình 800x640 tile 32px)
    int m_map[20][25];
};