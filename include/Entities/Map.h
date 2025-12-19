#pragma once

// THƯ VIỆN HỆ THỐNG
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// THƯ VIỆN DỰ ÁN
#include <SDL.h>           // Cần cho SDL_Rect
#include "Core/Config.h"   // Để lấy kích thước TILE_SIZE

// STRUCT: MapPoint (TỌA ĐỘ LƯỚI)
struct MapPoint {
    int row; // Chỉ số hàng
    int col; // Chỉ số cột

    // Hàm khởi tạo (Constructor)
    MapPoint(int r = 0, int c = 0) : row(r), col(c) {}

    // // Toán tử so sánh
    // bool operator==(const MapPoint& other) const {
    //     return row == other.row && col == other.col;
    // }
};

// CLASS: Map (HỆ THỐNG BẢN ĐỒ)
class Map {
public:
    Map();
    ~Map();

    // Tải bản đồ từ file .txt
    void LoadMap(const std::string& path);
    
    // Vẽ bản đồ lên màn hình
    void DrawMap();

    // CÁC HÀM TRUY VẤN & THIẾT LẬP (GETTERS & SETTERS)

    // Lấy ID của ô tại vị trí cụ thể (0: Đường đi, 1: Không khí, 2: Trận Nhãn)
    int GetTileID(int row, int col) const;

    // Cập nhật ID của ô (dùng khi thu thập trận nhãn hoặc hoàn tác)
    void SetTileID(int row, int col, int id);

    // Lấy điểm xuất phát của nhân vật
    MapPoint GetStartPoint() const { return m_startPoint; }

    // Lấy danh sách tất cả các trận nhãn (Shrines)
    const std::vector<MapPoint>& GetShrines() const { return m_shrines; }

    // Lấy kích thước bản đồ
    int GetRows() const { return m_rows; }
    int GetCols() const { return m_cols; }

private:
    // Ma trận lưu trữ bản đồ (Vector 2 chiều)
    std::vector<std::vector<int>> m_mapMatrix;

    // Vị trí xuất phát của nhân vật
    MapPoint m_startPoint;

    // Danh sách tọa độ các trận nhãn (TileID = 2)
    std::vector<MapPoint> m_shrines;

    // Kích thước bản đồ hiện tại
    int m_rows;
    int m_cols;
};