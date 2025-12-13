#include "ThienCoEngine.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// Hàm BFS cơ bản (Breath-First Search) - Trái tim của bài toán tìm đường
std::vector<std::vector<int>> ThienCoEngine::BFS(Map* pMap, MapPoint start) {
    int rows = pMap->GetRows();
    int cols = pMap->GetCols();

    // Khởi tạo bảng khoảng cách với giá trị -1 (chưa đến được)
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    
    // Hàng đợi cho BFS
    std::queue<Node> q;

    // Bắt đầu từ điểm start
    q.push({start.row, start.col, 0});
    dist[start.row][start.col] = 0;

    // 4 hướng di chuyển: Lên, Xuống, Trái, Phải
    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        // Duyệt 4 hướng
        for (int i = 0; i < 4; i++) {
            int newRow = current.row + dRow[i];
            int newCol = current.col + dCol[i];

            // Kiểm tra biên và vật cản
            // GetTileID trả về 1 là NÚI (#) -> Không đi được
            if (pMap->GetTileID(newRow, newCol) != 1) {
                // Nếu chưa thăm (dist == -1)
                if (dist[newRow][newCol] == -1) {
                    dist[newRow][newCol] = current.distance + 1;
                    q.push({newRow, newCol, current.distance + 1});
                }
            }
        }
    }

    return dist;
}

std::vector<std::vector<int>> ThienCoEngine::CalculateInsight(Map* pMap) {
    std::cout << "\n--- [THIEN CO ENGINE] KHOI DONG HE THONG MINH TRI ---" << std::endl;

    // 1. Thu thập danh sách các điểm quan trọng
    std::vector<MapPoint> points;
    
    // Điểm 0: Xuất phát
    points.push_back(pMap->GetStartPoint());
    
    // Điểm 1..N: Các Trận Nhãn
    const std::vector<MapPoint>& shrines = pMap->GetShrines();
    points.insert(points.end(), shrines.begin(), shrines.end());

    int N = points.size(); // Tổng số điểm (1 Start + K Shrine)
    std::cout << "Phat hien " << N << " dia diem quan trong (1 Start + " << N-1 << " Shrine)." << std::endl;

    // 2. Khởi tạo Ma trận kết quả (N x N)
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 0));

    // 3. Chạy BFS từ từng điểm để tính khoảng cách đến các điểm còn lại
    for (int i = 0; i < N; i++) {
        // Chạy BFS lan truyền từ điểm i ra toàn bản đồ
        std::vector<std::vector<int>> distMap = BFS(pMap, points[i]);

        for (int j = 0; j < N; j++) {
            if (i == j) {
                matrix[i][j] = 0;
            } else {
                // Lấy khoảng cách từ bản đồ BFS tại tọa độ của điểm j
                int d = distMap[points[j].row][points[j].col];
                matrix[i][j] = d;
            }
        }
    }

    // 4. In Ma trận Tri thức ra Console (để kiểm tra - đúng yêu cầu Bài toán 1)
    std::cout << "Ma Tran Tri Thuc (Insight Matrix):" << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << std::setw(4) << matrix[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------------" << std::endl;

    return matrix;
}

DestinyResult ThienCoEngine::CalculateDestiny(const std::vector<std::vector<int>>& dist) {
    std::cout << "\n--- [THIEN CO ENGINE] KHOI DONG HE THONG DINH MENH (TSP) ---" << std::endl;

    int N = dist.size(); // Số lượng địa điểm (bao gồm điểm xuất phát 0)
    
    // DP State: dp[mask][u] là chi phí nhỏ nhất để đi qua tập hợp các điểm trong 'mask', kết thúc tại điểm 'u'
    // Mask là một số nguyên biểu diễn nhị phân. Ví dụ: 0101 nghĩa là đã đi qua điểm 0 và điểm 2.
    int numStates = 1 << N; // 2^N trạng thái
    std::vector<std::vector<int>> dp(numStates, std::vector<int>(N, INF));
    
    // Traceback State: Để truy vết đường đi
    std::vector<std::vector<int>> parent(numStates, std::vector<int>(N, -1));

    // Trạng thái cơ sở: Bắt đầu tại điểm 0 (mask = 1, u = 0), chi phí = 0
    dp[1][0] = 0;

    // Duyệt qua tất cả các trạng thái (mask)
    for (int mask = 1; mask < numStates; mask++) {
        // Duyệt qua điểm kết thúc hiện tại (u)
        for (int u = 0; u < N; u++) {
            // Nếu điểm u có trong mask (bit thứ u của mask bật)
            if ((mask & (1 << u))) {
                
                // Thử đi tiếp đến điểm v
                for (int v = 0; v < N; v++) {
                    // Nếu điểm v CHƯA có trong mask (chưa thăm)
                    if (!(mask & (1 << v))) {
                        int nextMask = mask | (1 << v); // Bật bit v lên
                        int newCost = dp[mask][u] + dist[u][v];

                        // Nếu tìm được đường ngắn hơn, cập nhật DP
                        if (dist[u][v] != -1 && newCost < dp[nextMask][v]) { // dist != -1 để đảm bảo có đường
                            dp[nextMask][v] = newCost;
                            parent[nextMask][v] = u; // Lưu lại: đến v từ u
                        }
                    }
                }
            }
        }
    }

    // --- TÌM KẾT QUẢ TỐI ƯU ---
    // Trạng thái cuối cùng là mask full (tất cả bit đều bật) = (1 << N) - 1
    // Chúng ta cần tìm min(dp[fullMask][i]) với i là bất kỳ điểm nào (vì đề bài không yêu cầu quay về)
    
    int fullMask = numStates - 1;
    int minCost = INF;
    int lastNode = -1;

    for (int i = 1; i < N; i++) { // Kết thúc ở bất kỳ Trận Nhãn nào (từ 1 đến N-1)
        if (dp[fullMask][i] < minCost) {
            minCost = dp[fullMask][i];
            lastNode = i;
        }
    }

    // --- TRUY VẾT (TRACEBACK) ---
    DestinyResult result;
    if (minCost == INF) {
        std::cout << "[CANH BAO] Khong tim thay duong di qua tat ca cac diem!" << std::endl;
        result.totalSteps = -1;
        return result;
    }

    result.totalSteps = minCost;
    
    // Lần ngược từ cuối về đầu
    int currMask = fullMask;
    int currNode = lastNode;

    while (currNode != -1) {
        result.pathOrder.push_back(currNode);
        int prevNode = parent[currMask][currNode];
        currMask = currMask ^ (1 << currNode); // Tắt bit hiện tại
        currNode = prevNode;
    }

    // Đảo ngược lại để có thứ tự đúng: 0 -> ... -> ...
    std::reverse(result.pathOrder.begin(), result.pathOrder.end());

    // --- IN KẾT QUẢ RA CONSOLE (CHUẨN BÀI TOÁN 2) ---
    std::cout << "Thien Menh (S_optimal): " << result.totalSteps << " buoc." << std::endl;
    std::cout << "Lo Trinh Chien Luoc: ";
    for (size_t i = 0; i < result.pathOrder.size(); i++) {
        std::cout << result.pathOrder[i] << (i < result.pathOrder.size() - 1 ? " -> " : "");
    }
    std::cout << "\n------------------------------------------------------------" << std::endl;

    return result;
}