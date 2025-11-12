// -------------------------------------------------------------------------
// src/BaiToan3.cpp
// Giải quyết "Bài toán 3: Nhập Thế" (Phiên bản Console)
// Tích hợp BaiToan1 (BFS) và BaiToan2 (Bitmask DP)
// -------------------------------------------------------------------------

#include <iostream>  // Dùng cho cin, cout
#include <vector>    // Dùng để lưu bản đồ, ma trận, dp, path
#include <string>    // Dùng để đọc bản đồ
#include <queue>     // Dùng cho thuật toán BFS
#include <utility>   // Dùng cho std::pair
#include <algorithm> // Dùng cho std::min
#include <sstream>   // Không thực sự cần nữa, nhưng vẫn tốt

// ----- TỪ BÀI TOÁN 1: Biến toàn cục cho BFS -----
typedef std::pair<int, int> Point;
int R, C, N;
std::vector<std::string> grid;
std::vector<Point> importantPoints;

// ----- TỪ BÀI TOÁN 2: Biến toàn cục cho DP -----
const int INF = 1e9;
int numPoints; // = N + 1
std::vector<std::vector<int>> dist; // Ma trận Tri Thức
std::vector<std::vector<int>> dp;
std::vector<std::vector<int>> path;

// -----------------------------------------------------------------
// HÀM 1: SAO CHÉP TỪ BaiToan1.cpp (Đọc Input & Tìm Điểm)
// -----------------------------------------------------------------
void ReadInputAndFindPoints() {
    std::cin >> R >> C >> N;
    numPoints = N + 1; // Khởi tạo biến toàn cục
    
    importantPoints.push_back(Point(0, 0)); // Điểm 0
    grid.resize(R);
    for (int i = 0; i < R; ++i) {
        std::cin >> grid[i];
        for (int j = 0; j < C; ++j) {
            if (grid[i][j] == 'S') {
                importantPoints.push_back(Point(i, j));
            }
        }
    }
}

// -----------------------------------------------------------------
// HÀM 2: SAO CHÉP TỪ BaiToan1.cpp (Thuật toán BFS)
// -----------------------------------------------------------------
std::vector<std::vector<int>> RunBFS(Point startPoint) {
    std::vector<std::vector<int>> distMap(R, std::vector<int>(C, -1));
    std::queue<Point> q;

    distMap[startPoint.first][startPoint.second] = 0;
    q.push(startPoint);

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (!q.empty()) {
        Point u = q.front();
        q.pop();
        for (int i = 0; i < 4; ++i) {
            int newR = u.first + dr[i];
            int newC = u.second + dc[i];
            if (newR >= 0 && newR < R && newC >= 0 && newC < C &&
                grid[newR][newC] != '#' &&
                distMap[newR][newC] == -1) 
            {
                distMap[newR][newC] = distMap[u.first][u.second] + 1;
                q.push(Point(newR, newC));
            }
        }
    }
    return distMap;
}

// -----------------------------------------------------------------
// HÀM 3: SAO CHÉP TỪ BaiToan2.cpp (Thuật toán DP & Truy vết)
// -----------------------------------------------------------------
void SolveTSP() {
    // (Hàm này sao chép y hệt, vì nó đọc từ biến 'dist' toàn cục)
    int numStates = (1 << numPoints); 
    dp.resize(numStates, std::vector<int>(numPoints, INF));
    path.resize(numStates, std::vector<int>(numPoints, -1));
    dp[1][0] = 0; 

    // 3. Chạy vòng lặp DP
    // Lặp qua tất cả các mask có thể
    for (int mask = 1; mask < numStates; ++mask) {
        // Lặp qua node cuối 'u' của trạng thái hiện tại
        for (int u = 0; u < numPoints; ++u) {
            // Kiểm tra xem 'u' có nằm trong 'mask' không
            if (mask & (1 << u)) {

                // Nếu trạng thái (mask, u) không thể đạt tới (chi phí vẫn là vô cùng), bỏ qua
                if (dp[mask][u] == INF) continue;

                // Từ node 'u', thử đi đến tất cả các node 'v' chưa thăm
                for (int v = 0; v < numPoints; ++v) {
                    // Kiểm tra 'v' đã nằm trong 'mask' chưa
                    if (!(mask & (1 << v))) {
                        // Nếu không có đường đi từ u đến v thì bỏ qua
                        if (dist[u][v] == -1) continue;

                        // Tạo trạng thái mới bằng cách thêm điểm 'v' vào 'mask'
                        int newMask = mask | (1 << v);

                        // Tính toán chi phí mới để đến trạng thái (newMask, v) thông qua 'u'
                        int newCost = dp[mask][u] + dist[u][v];

                        // Cập nhật nếu chi phí mới nhỏ hơn chi phí hiện tại
                        if (newCost < dp[newMask][v]) {
                            dp[newMask][v] = newCost;
                            // Ghi lại truy vết: để đến được 'v' trong 'newMask', ta đã đi từ 'u'
                            path[newMask][v] = u;
                        }
                    }
                }
            }
        }
    }

    // Truy vết (y hệt BaiToan2.cpp)
    int allVisitedMask = (1 << numPoints) - 1;
    int minCost = INF;
    int lastNode = -1;

    for (int u = 1; u < numPoints; ++u) {
        if (dp[allVisitedMask][u] < minCost) {
            minCost = dp[allVisitedMask][u];
            lastNode = u;
        }
    }

    if (minCost == INF) {
        std::cout << "-1" << std::endl;
        return;
    }

    std::vector<int> finalPath;
    int currNode = lastNode;
    int currMask = allVisitedMask;
    for (int i = 0; i < N; ++i) {
        finalPath.push_back(currNode);
        int prevNode = path[currMask][currNode];
        currMask = currMask ^ (1 << currNode);
        currNode = prevNode;
    }
    finalPath.push_back(0); 

    std::cout << minCost << std::endl;
    for (int i = finalPath.size() - 1; i >= 0; --i) {
        std::cout << finalPath[i] << (i == 0 ? "" : " ");
    }
    std::cout << std::endl;
}

// -----------------------------------------------------------------
// HÀM MAIN (Hàm "Keo Dán" Mới)
// -----------------------------------------------------------------
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // ----- BƯỚC 1: Chạy logic của Bài 1 (Đọc Input) -----
    ReadInputAndFindPoints();

    // ----- BƯỚC 2: Chạy logic của Bài 1 (Tính toán Ma trận) -----
    // (Lấy từ main() của BaiToan1.cpp)
    
    // Khởi tạo ma trận 'dist' toàn cục
    dist.resize(numPoints, std::vector<int>(numPoints));

    for (int i = 0; i < numPoints; ++i) {
        Point startPoint = importantPoints[i];
        std::vector<std::vector<int>> distMap = RunBFS(startPoint);
        for (int j = 0; j < numPoints; ++j) {
            Point endPoint = importantPoints[j];
            dist[i][j] = distMap[endPoint.first][endPoint.second];
        }
    }
    
    // ----- BƯỚC 3: Chạy logic của Bài 2 (Giải DP) -----
    // (Lấy từ main() của BaiToan2.cpp)
    SolveTSP();

    return 0;
}