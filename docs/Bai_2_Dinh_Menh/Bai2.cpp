#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int INF = 1e9;

// Cấu trúc lưu kết quả
struct Solution {
    int totalSteps;
    vector<int> pathSequence;
    Solution() : totalSteps(INF) {}
};

// Hàm truy vết đường đi từ bảng phương án DP
vector<int> TracebackPath(const vector<vector<int>>& dp, 
                          const vector<vector<int>>& parent, 
                          int nodeCount, int finalMask, int lastNode) {
    vector<int> path;

    // Nếu không tìm thấy đường đi hoặc node cuối không hợp lệ
    if (lastNode == -1 || dp[finalMask][lastNode] >= INF) {
        return path;
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
        
        // Nếu quay về mask = 0 thì dừng (tránh vòng lặp vô tận nếu có lỗi)
        if (currMask == 0) break;
    }
    
    // Đảm bảo node xuất phát (0) có trong danh sách nếu chưa có
    // (Logic trên thường đã bao gồm 0 ở bước cuối cùng của while, nhưng kiểm tra cho chắc chắn)
    if (path.back() != 0) path.push_back(0);

    // Đảo ngược vector để có thứ tự đúng: Xuất phát -> ... -> Đích
    reverse(path.begin(), path.end());
    return path;
}

void Solve() {
    int N_Shrines;
    // Đọc số lượng Trận Nhãn (N). Tổng số địa điểm sẽ là N + 1 (bao gồm điểm xuất phát 0)
    if (!(cin >> N_Shrines)) return;

    int numNodes = N_Shrines + 1;
    
    // Đọc ma trận khoảng cách
    // dist[i][j] là chi phí từ i đến j
    vector<vector<int>> dist(numNodes, vector<int>(numNodes));
    for (int i = 0; i < numNodes; ++i) {
        for (int j = 0; j < numNodes; ++j) {
            cin >> dist[i][j];
        }
    }

    // BẮT ĐẦU THUẬT TOÁN TSP BITMASK

    int limitMask = 1 << numNodes; // Tổng số trạng thái: 2^(N+1)

    // dp[mask][u]: Chi phí nhỏ nhất để đi qua tập 'mask' và kết thúc tại 'u'
    vector<vector<int>> dp(limitMask, vector<int>(numNodes, INF));
    
    // parent[mask][u]: Lưu node trước đó để truy vết
    vector<vector<int>> parent(limitMask, vector<int>(numNodes, -1));

    // Trạng thái cơ sở: Bắt đầu tại node 0, mask chỉ có bit 0 bật (binary: ...001)
    dp[1][0] = 0;

    // Duyệt qua tất cả các trạng thái mask
    for (int mask = 1; mask < limitMask; ++mask) {
        // Duyệt qua node cuối cùng 'u' trong tập mask hiện tại
        for (int u = 0; u < numNodes; ++u) {
            // Nếu u có trong mask (bit thứ u bật)
            if ((mask & (1 << u))) {
                
                // Nếu trạng thái này chưa tới được (vô cực), bỏ qua
                if (dp[mask][u] == INF) continue;

                // Thử đi tiếp đến node 'v'
                for (int v = 0; v < numNodes; ++v) {
                    // Nếu v chưa có trong mask (chưa thăm)
                    if (!(mask & (1 << v))) {
                        
                        // Kiểm tra đường đi từ u -> v có hợp lệ không
                        int d = dist[u][v];
                        if (d != -1) { // -1 nghĩa là không có đường đi
                            int nextMask = mask | (1 << v);
                            
                            // Tối ưu hóa (Relaxation)
                            if (dp[nextMask][v] > dp[mask][u] + d) {
                                dp[nextMask][v] = dp[mask][u] + d;
                                parent[nextMask][v] = u; // Lưu vết
                            }
                        }
                    }
                }
            }
        }
    }

    // TÌM KẾT QUẢ TỐI ƯU

    // Trạng thái đích: Mask full bit 1 (đã thăm hết tất cả các node)
    int fullMask = limitMask - 1;
    int minTotalSteps = INF;
    int bestLastNode = -1;

    // Điểm kết thúc có thể là bất kỳ Trận Nhãn nào (từ 1 đến N)
    // Node 0 là điểm xuất phát, không quay lại 0 (Open TSP)
    for (int i = 1; i < numNodes; ++i) {
        if (dp[fullMask][i] < minTotalSteps) {
            minTotalSteps = dp[fullMask][i];
            bestLastNode = i;
        }
    }

    // XUẤT KẾT QUẢ
    
    if (minTotalSteps == INF) {
        cout << -1 << endl;
    } else {
        cout << minTotalSteps << endl;
        
        // Truy vết đường đi
        vector<int> path = TracebackPath(dp, parent, numNodes, fullMask, bestLastNode);
        
        // In ra dãy các địa điểm
        for (int i = 0; i < path.size(); ++i) {
            cout << path[i] << (i == path.size() - 1 ? "" : " ");
        }
        cout << endl;
    }
}

int main() {
    // Tối ưu tốc độ nhập xuất
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    Solve();

    return 0;
}