#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;

// HẰNG SỐ
const int INF = 1e9;

// CẤU TRÚC DỮ LIỆU
struct Point {
    int r, c;
};

// INPUT DATA
int R, C, N;
vector<string> grid;
vector<Point> keyPoints; // 0: Start, 1..N: Shrines

// Ma trận khoảng cách giữa các điểm quan trọng
struct Matrix {
    vector<vector<int>> distances;
    int nodeCount;
    Matrix() : nodeCount(0) {}
};

// Kết quả tối ưu
struct Solution {
    int totalSteps;
    vector<int> pathSequence;
    Solution() : totalSteps(INF) {}
};

// BFS Tìm đường ngắn nhất giữa 2 điểm trên lưới
int BFS_FindShortestPath(Point start, Point end) {
    if (start.r == end.r && start.c == end.c) return 0;

    // Dist array init
    vector<vector<int>> dist(R, vector<int>(C, -1));
    
    queue<Point> q;
    q.push(start);
    dist[start.r][start.c] = 0;

    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        if (current.r == end.r && current.c == end.c) {
            return dist[current.r][current.c];
        }

        for (int i = 0; i < 4; i++) {
            int newRow = current.r + dRow[i];
            int newCol = current.c + dCol[i];

            // Kiểm tra biên
            if (newRow >= 0 && newRow < R && newCol >= 0 && newCol < C) {
                // Kiểm tra chướng ngại vật (#) và chưa thăm
                if (dist[newRow][newCol] == -1 && grid[newRow][newCol] != '#') {
                    dist[newRow][newCol] = dist[current.r][current.c] + 1;
                    q.push({newRow, newCol});
                }
            }
        }
    }
    return INF;
}

// Tính toán ma trận khoảng cách giữa các điểm quan trọng
Matrix CalculateMatrix() {
    Matrix matrix;
    int numNodes = keyPoints.size();
    matrix.nodeCount = numNodes;
    matrix.distances.assign(numNodes, vector<int>(numNodes, 0));

    // Tính BFS giữa các cặp điểm
    for (int i = 0; i < numNodes; i++) {
        for (int j = i + 1; j < numNodes; j++) {
            int d = BFS_FindShortestPath(keyPoints[i], keyPoints[j]);
            matrix.distances[i][j] = d;
            matrix.distances[j][i] = d;
        }
    }
    return matrix;
}

// Hàm truy vết đường đi
vector<int> TracebackPath(const vector<vector<int>>& dp, 
                          const vector<vector<int>>& parent, 
                          int finalMask, int lastNode) 
{
    vector<int> path;
    if (lastNode == -1 || dp[finalMask][lastNode] >= INF) {
        return path;
    }

    int currMask = finalMask;
    int currNode = lastNode;

    while (currNode != -1) {
        path.push_back(currNode);
        int prevNode = parent[currMask][currNode];
        int prevMask = currMask ^ (1 << currNode);
        
        currNode = prevNode;
        currMask = prevMask;
        if (currMask == 0) break;
    }

    // Luôn đảm bảo bắt đầu từ 0
    if (path.back() != 0) path.push_back(0);
    reverse(path.begin(), path.end());
    return path;
}

// Quy hoạch động Bitmask (TSP)
Solution SolveTSP_DP(const Matrix& matrix) {
    Solution solution;
    int numNodes = matrix.nodeCount;
    int limitMask = 1 << numNodes;

    // Init DP
    vector<vector<int>> dp(limitMask, vector<int>(numNodes, INF));
    vector<vector<int>> parent(limitMask, vector<int>(numNodes, -1));

    // Base case: Tại Start (node 0), chi phí = 0
    dp[1][0] = 0;

    for (int mask = 1; mask < limitMask; ++mask) {
        for (int u = 0; u < numNodes; ++u) {
            if (mask & (1 << u)) {
                if (dp[mask][u] == INF) continue;

                // Thử đi tới v
                for (int v = 0; v < numNodes; ++v) {
                    if (!(mask & (1 << v))) {
                        int nextMask = mask | (1 << v);
                        int distUV = matrix.distances[u][v];

                        if (distUV != INF) {
                            if (dp[nextMask][v] > dp[mask][u] + distUV) {
                                dp[nextMask][v] = dp[mask][u] + distUV;
                                parent[nextMask][v] = u;
                            }
                        }
                    }
                }
            }
        }
    }

    // Tìm kết quả tốt nhất khi đã thăm hết tất cả (fullMask)
    // Kết thúc tại bất kỳ Shrine nào (node 1..N)
    int fullMask = limitMask - 1;
    int minTotalSteps = INF;
    int bestLastNode = -1;

    // Node 0 là start, nên ta xét điểm cuối là 1..N.
    // Nếu N=0 (không có shrine), thì kết quả là 0 tại node 0.
    if (numNodes == 1) {
         solution.totalSteps = 0;
         solution.pathSequence = {0};
         return solution;
    }

    for (int i = 1; i < numNodes; ++i) {
        if (dp[fullMask][i] < minTotalSteps) {
            minTotalSteps = dp[fullMask][i];
            bestLastNode = i;
        }
    }

    solution.totalSteps = minTotalSteps;
    if (minTotalSteps < INF) {
        solution.pathSequence = TracebackPath(dp, parent, fullMask, bestLastNode);
    }
    return solution;
}

int main() {
    // Tối ưu IO
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> R >> C >> N)) return 0;

    grid.resize(R);

    // Điểm xuất phát luôn là (0, 0)
    // Node 0: Start Point
    keyPoints.push_back({0, 0}); 

    // Đọc bản đồ và tìm vị trí các Shrine
    // Thứ tự 'S' xuất hiện trong input sẽ tương ứng với index 1, 2, ..., N
    for (int i = 0; i < R; i++) {
        cin >> grid[i];
        for (int j = 0; j < C; j++) {
            if (grid[i][j] == 'S') {
                keyPoints.push_back({i, j});
            }
        }
    }

    // Tính toán ma trận khoảng cách
    Matrix matrix = CalculateMatrix();

    // Giải bài toán
    Solution sol = SolveTSP_DP(matrix);

    // Xuất kết quả
    if (sol.totalSteps >= INF) {
        cout << -1 << endl;
    } else {
        cout << sol.totalSteps << endl;
        for (size_t i = 0; i < sol.pathSequence.size(); i++) {
            cout << sol.pathSequence[i] << (i == sol.pathSequence.size() - 1 ? "" : " ");
        }
        cout << endl;
    }

    return 0;
}