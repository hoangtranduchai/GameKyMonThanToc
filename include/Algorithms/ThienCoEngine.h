#pragma once

// THƯ VIỆN CHUẨN
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>
#include <iostream>

// Include Map để AI có thể truy cập dữ liệu địa hình
#include "Entities/Map.h"

// CẤU TRÚC DỮ LIỆU

// Ma trận khoảng cách giữa các điểm quan trọng (Vị trí xuất phát + Các trận nhãn)
struct Matrix {
    // Ma trận vuông [N][N] lưu khoảng cách
    // N là số lượng trận nhãn. Index 0 là vị trí xuất phát
    // distance[i][j] = Số bước đi từ điểm i đến điểm j
    std::vector<std::vector<int>> distances;
    
    // Tổng số lượng điểm quan trọng (1 điểm xuất phát + N trận nhãn)
    int nodeCount; 

    // Hàm khởi tạo
    Matrix() : nodeCount(0) {}
};

// Kết quả tối ưu của thuật toán Thiên Mệnh (Bài toán TSP)
struct Solution {
    // Tổng số bước đi nhỏ nhất để hoàn thành màn chơi (Kỷ lục Rank S)
    int totalSteps;

    // Thứ tự các điểm cần đi qua để đạt được số bước trên
    // Ví dụ: {0, 2, 1, 3} nghĩa là: Xuất phát -> trận nhãn 2 -> trận nhãn 1 -> trận nhãn 3
    std::vector<int> pathSequence;

    Solution() : totalSteps(1e9) {}
};

// CLASS: ThienCoEngine (MẪU THIẾT KẾ SINGLETON)
class ThienCoEngine {
public:
    // Đảm bảo chỉ có 1 bộ não AI duy nhất trong suốt vòng đời game
    static ThienCoEngine* GetInstance();
    static void DestroyInstance();

    // TÍNH TOÁN MA TRẬN KHOẢNG CÁCH GIỮA CÁC ĐIỂM QUAN TRỌNG (Vị trí xuất phát + Các trận nhãn)
    Matrix CalculateMatrix(Map* pMap);

    // TÍNH TOÁN KẾT QUẢ TỐI ƯU THEO THIÊN MỆNH (BÀI TOÁN TSP)
    Solution CalculateSolution(const Matrix& matrix);

private:
    // Hàm khởi tạo riêng tư
    ThienCoEngine();
    ~ThienCoEngine();

    static ThienCoEngine* s_Instance;

    // Mảng 2 chiều lưu khoảng cách và đánh dấu đã thăm
    // Giá trị -1 nghĩa là chưa thăm
    std::vector<std::vector<int>> dist;
    
    // THUẬT TOÁN BFS (TÌM ĐƯỜNG NGẮN NHẤT TRÊN LƯỚI)
    int BFS_FindShortestPath(MapPoint start, MapPoint end, Map* pMap);

    // THUẬT TOÁN QUY HOẠCH ĐỘNG VỚI BITMASK GIẢI BÀI TOÁN TSP
    void SolveTSP_DP(const Matrix& matrix, Solution& solution);

    // TRUY VẾT ĐƯỜNG ĐI TỪ KẾT QUẢ QUY HOẠCH ĐỘNG
    std::vector<int> TracebackPath(const std::vector<std::vector<int>>& dp, 
                                   const std::vector<std::vector<int>>& parent,
                                   int nodeCount, int finalMask, int lastNode);

    // HẰNG SỐ
    const int START_NODE_INDEX = 0; // Node 0 luôn được quy định là vị trí bắt đầu của nhân vật
    const int INF = 1e9;
};