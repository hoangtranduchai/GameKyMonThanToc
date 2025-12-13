#pragma once
#include <SDL.h>
#include <string>
#include <vector>

class Map {
public:
    Map();
    ~Map();

    // Tải bản đồ
    void LoadMap(std::string path);
    
    // Vẽ bản đồ ra màn hình
    void DrawMap();

    // Hàm quan trọng để các lớp khác (Player, AI) biết ô nào đi được
    // Trả về: 0 (Nước/Biên), 1 (Đất/S - Đi được), 2 (Núi - Chặn)
    int GetTileID(int row, int col);

    // Getter kích thước bản đồ
    int GetRows() const {
        return m_rows;
    }
    int GetCols() const {
        return m_cols;
    }
    
    // Lấy vị trí bắt đầu (1,1) theo đề bài (nhưng trong mảng là 0,0)
    // Thực tế đề bài bảo (1,1) là đất trống, ta cần map nó vào tọa độ pixel
    int GetTileSize() const {
        return m_tileSize;
    }

    // Hàm lấy chiều rộng thực tế của map (Pixel)
    int GetMapPixelWidth() const {
        return m_cols * m_tileSize;
    }

    // Hàm lấy chiều cao thực tế của map (Pixel)
    int GetMapPixelHeight() const {
        return m_rows * m_tileSize;
    }

private:
    // ID của texture tileset
    std::string m_textureID;

    int m_rows;
    int m_cols;
    int m_numShrines; // Số lượng Trận Nhãn (N)
    int m_tileSize;

    // Dùng vector động để chứa bản đồ kích thước bất kỳ (R x C)
    std::vector<std::vector<int>> m_mapLayer;
};