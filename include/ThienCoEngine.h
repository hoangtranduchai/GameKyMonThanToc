#pragma once
#include <vector>
#include <queue>
#include "Map.h"

// Cấu trúc để lưu trạng thái khi duyệt BFS
struct Node {
    int row;
    int col;
    int distance;
};

// Cấu trúc lưu kết quả Thiên Mệnh
struct DestinyResult {
    int totalSteps;                 // Tổng số bước tối ưu (S_optimal)
    std::vector<int> pathOrder;     // Thứ tự đi qua các điểm (0 -> 2 -> 1 -> 3...)
};

class ThienCoEngine {
public:
    // Singleton Pattern
    static ThienCoEngine* GetInstance() {
        static ThienCoEngine instance;
        return &instance;
    }

    // Hàm chính: Tính toán ma trận khoảng cách giữa các điểm quan trọng (Start + Shrines)
    // Trả về ma trận kề (Adjacency Matrix) kích thước (N+1)x(N+1)
    std::vector<std::vector<int>> CalculateInsight(Map* pMap);

    // --- THÊM MỚI: TÍNH TOÁN THIÊN MỆNH (TSP) ---
    // Input: Ma trận khoảng cách (Insight)
    // Output: Kết quả tối ưu (Số bước + Lộ trình)
    DestinyResult CalculateDestiny(const std::vector<std::vector<int>>& insightMatrix);

private:
    ThienCoEngine() {}
    
    // Thuật toán BFS tìm đường ngắn nhất từ một điểm start đến toàn bộ bản đồ
    // Trả về bảng khoảng cách từ start đến mọi ô (distMap)
    std::vector<std::vector<int>> BFS(Map* pMap, MapPoint start);

    // Hằng số vô cực (đại diện cho không thể đi tới)
    const int INF = 1e9;
};