#include "Algorithms/ThienCoEngine.h"

// Khởi tạo con trỏ Singleton
ThienCoEngine* ThienCoEngine::s_Instance = nullptr;

// TRIỂN KHAI MẪU THIẾT KẾ SINGLETON (SINGLETON PATTERN IMPLEMENTATION)

ThienCoEngine* ThienCoEngine::GetInstance() {
    if (s_Instance == nullptr) s_Instance = new ThienCoEngine();
    return s_Instance;
}

void ThienCoEngine::DestroyInstance() {
    if (s_Instance != nullptr) {
        delete s_Instance;
        s_Instance = nullptr;
    }
}

// Hàm khởi tạo riêng tư (Private Constructor)
ThienCoEngine::ThienCoEngine() {}

ThienCoEngine::~ThienCoEngine() {}

// TÍNH TOÁN MA TRẬN KHOẢNG CÁCH GIỮA CÁC ĐIỂM QUAN TRỌNG (Vị trí xuất phát + Các trận nhãn)
Matrix ThienCoEngine::CalculateMatrix(Map* pMap) {
    Matrix matrix;
    
    // 1. Danh sách các điểm quan trọng (Nodes)
    // Node 0: Vị trí xuất phát của nhân vật (Start Point)
    // Node 1..N: Các trận nhãn (Shrines)
    std::vector<MapPoint> keyPoints;
    keyPoints.push_back(pMap->GetStartPoint()); // Thêm điểm xuất phát vào đầu tiên
    
    const std::vector<MapPoint>& shrines = pMap->GetShrines();
    keyPoints.insert(keyPoints.end(), shrines.begin(), shrines.end());
    
    int N = (int)keyPoints.size();
    matrix.nodeCount = N;
    
    // 2. Khởi tạo kích thước ma trận khoảng cách (NxN)
    // Giá trị ban đầu là 0
    matrix.distances.assign(N, std::vector<int>(N, 0));
    
    // 3. Chạy BFS giữa mọi cặp điểm để tính khoảng cách ngắn nhất
    // Vì đồ thị vô hướng (đi từ A->B bằng B->A), chỉ cần tính một nửa ma trận
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            // Tính khoảng cách từ Node i đến Node j
            int dist = BFS_FindShortestPath(keyPoints[i], keyPoints[j], pMap);
            
            // Lưu vào ma trận đối xứng (vì đồ thị vô hướng)
            matrix.distances[i][j] = dist;
            matrix.distances[j][i] = dist;
        }
    }
    return matrix;
}

// THUẬT TOÁN BFS (TÌM ĐƯỜNG NGẮN NHẤT TRÊN LƯỚI)
int ThienCoEngine::BFS_FindShortestPath(MapPoint start, MapPoint end, Map* pMap) {
    if (start.row == end.row && start.col == end.col) return 0;

    int rows = pMap->GetRows();
    int cols = pMap->GetCols();

    // Thay đổi kích thước hoặc đặt lại ma trận khoảng cách
    if (dist.size() != rows || (rows > 0 && dist[0].size() != cols)) {
        dist.assign(rows, std::vector<int>(cols, -1));
    } else {
        for (int i = 0; i < rows; i++) {
            std::fill(dist[i].begin(), dist[i].end(), -1);
        }
    }

    std::queue<MapPoint> q;
    q.push(start);
    dist[start.row][start.col] = 0;

    // 4 hướng: Lên, Xuống, Trái, Phải
    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    while (!q.empty()) {
        MapPoint current = q.front();
        q.pop();

        if (current.row == end.row && current.col == end.col) {
            return dist[current.row][current.col];
        }

        for (int i = 0; i < 4; i++) {
            int newRow = current.row + dRow[i];
            int newCol = current.col + dCol[i];

            // Kiểm tra biên và tính hợp lệ
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                if (dist[newRow][newCol] == -1 && pMap->GetTileID(newRow, newCol) != 1) {
                    dist[newRow][newCol] = dist[current.row][current.col] + 1;
                    q.push({newRow, newCol});
                }
            }
        }
    }

    return INF;
}


// TÍNH TOÁN KẾT QUẢ TỐI ƯU THEO THIÊN MỆNH (BÀI TOÁN TSP)
Solution ThienCoEngine::CalculateSolution(const Matrix& matrix) {
    Solution solution;
    int N = matrix.nodeCount;

    // Trường hợp biên: Nếu chỉ có điểm xuất phát (N = 1) hoặc không có điểm nào
    if (N <= 1) {
        solution.totalSteps = 0;
        return solution;
    }

    // Gọi thuật toán Quy hoạch động với Bitmask
    SolveTSP_DP(matrix, solution);
    return solution;
}

// THUẬT TOÁN DP BITMASK GIẢI BÀI TOÁN TSP
void ThienCoEngine::SolveTSP_DP(const Matrix& matrix, Solution& solution) {
    int N = matrix.nodeCount;
    int limitMask = 1 << N; // Tổng số trạng thái: 2^N

    // 1. Khởi tạo bảng DP và bảng truy vết Parent
    // dp[mask][u]: Chi phí nhỏ nhất để đi qua tập 'mask' và kết thúc tại 'u'
    std::vector<std::vector<int>> dp(limitMask, std::vector<int>(N, INF));
    
    // parent[mask][u]: Lưu node trước đó đã đi để đến được 'u' với trạng thái 'mask'
    std::vector<std::vector<int>> parent(limitMask, std::vector<int>(N, -1));

    // 2. Trạng thái cơ sở (Base Case)
    // Bắt đầu tại node 0 (Start Point), mask chỉ có bit 0 bật
    dp[1][0] = 0;

    // 3. Quy hoạch động với Bitmask
    // Duyệt qua tất cả các trạng thái (mask)
    for (int mask = 1; mask < limitMask; ++mask) {
        
        // Duyệt qua node cuối cùng 'u' trong tập mask hiện tại
        for (int u = 0; u < N; ++u) {
            
            // Nếu u có trong mask (bit thứ u bật)
            if ((mask & (1 << u))) {
                
                // Nếu trạng thái này chưa tới được, bỏ qua
                if (dp[mask][u] == INF) continue;

                // Thử đi tiếp đến node 'v' kế tiếp
                for (int v = 0; v < N; ++v) {
                    
                    // Nếu v chưa có trong mask (chưa thăm)
                    if (!(mask & (1 << v))) {
                        
                        // Tính mask mới khi thêm v
                        int nextMask = mask | (1 << v);
                        
                        // Kiểm tra khoảng cách u->v có hợp lệ không
                        int distUV = matrix.distances[u][v];
                        if (distUV != INF && distUV != -1) {
                            
                            // Tối ưu hóa (Relaxation):
                            // Nếu đường mới đi qua v ngắn hơn đường cũ đã biết
                            if (dp[nextMask][v] > dp[mask][u] + distUV) {
                                dp[nextMask][v] = dp[mask][u] + distUV;
                                parent[nextMask][v] = u; // Lưu vết: Để đến v ở trạng thái nextMask, ta đi từ u
                            }
                        }
                    }
                }
            }
        }
    }

    // 4. Tìm kết quả tốt nhất
    // Trạng thái đích: Mask full bit 1 (tất cả các bit đều bật) -> Đã thăm hết N node
    int fullMask = limitMask - 1;
    int minTotalSteps = INF;
    int bestLastNode = -1;

    // Điểm kết thúc có thể là bất kỳ Shrine nào
    // Duyệt tất cả các node 'i' để xem kết thúc ở đâu là tối ưu nhất
    for (int i = 1; i < N; ++i) { // i chạy từ 1 vì node 0 là Start
        if (dp[fullMask][i] < minTotalSteps) {
            minTotalSteps = dp[fullMask][i];
            bestLastNode = i;
        }
    }

    // 5. Lưu kết quả vào Solution
    solution.totalSteps = minTotalSteps;
    std::cout << "[AI] Số bước tối ưu: " << minTotalSteps << std::endl;
    // Truy vết lại đường đi từ kết quả DP
    solution.pathSequence = TracebackPath(dp, parent, N, fullMask, bestLastNode);
}

// TRUY VẾT ĐƯỜNG ĐI (TRACEBACK PATH)
std::vector<int> ThienCoEngine::TracebackPath(const std::vector<std::vector<int>>& dp, 
                                              const std::vector<std::vector<int>>& parent,
                                              int nodeCount, int finalMask, int lastNode) 
{
    std::vector<int> path;

    // Trường hợp không tìm thấy đường đi
    if (lastNode == -1 || dp[finalMask][lastNode] >= INF) {
        std::cout << "[AI] Không tìm thấy giải pháp!" << std::endl;
        return path; // Trả về rỗng
    }

    int currMask = finalMask;
    int currNode = lastNode;

    // Lần ngược từ trạng thái cuối về trạng thái đầu
    while (currNode != -1) {
        path.push_back(currNode);
        
        int prevNode = parent[currMask][currNode];
        
        // Loại bỏ bit của currNode ra khỏi mask để quay về trạng thái trước
        int prevMask = currMask ^ (1 << currNode);
        
        currNode = prevNode;
        currMask = prevMask;
        
        // Nếu quay về mask = 0 thì dừng
        if (currMask == 0) break;
    }
    
    // Thêm điểm xuất phát (Node 0) vào cuối danh sách
    if (path.back() != 0) path.push_back(0);

    // Đảo ngược vector để có thứ tự đúng
    std::reverse(path.begin(), path.end());

    // In lộ trình để kiểm tra (debug)
    std::cout << "Chuỗi đường đi tối ưu: ";
    for (int i = 0; i < (int)path.size() - 1; i++) {
        std::cout << path[i] << " -> ";
    }
    std::cout << path.back() << std::endl;

    return path;
}