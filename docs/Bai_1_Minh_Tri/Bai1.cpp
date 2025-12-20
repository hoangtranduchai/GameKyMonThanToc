#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

// Cấu trúc lưu tọa độ điểm trên bản đồ
struct Point {
    int r, c;
};

// Các biến toàn cục để lưu trữ dữ liệu bản đồ
int R, C, N;
vector<string> mapData;
vector<Point> keyPoints; // Điểm quan trọng: [0] là Start, [1..N] là các Trận Nhãn

// Hằng số hướng di chuyển (Lên, Xuống, Trái, Phải)
const int dr[] = {-1, 1, 0, 0};
const int dc[] = {0, 0, -1, 1};

// Hàm BFS tìm khoảng cách từ một điểm xuất phát đến TOÀN BỘ bản đồ
// Trả về mảng 2 chiều chứa khoảng cách
vector<vector<int>> bfs_full_map(Point startNode) {
    // Khởi tạo ma trận khoảng cách với -1 (chưa thăm)
    vector<vector<int>> dist(R, vector<int>(C, -1));
    queue<Point> q;

    // Setup điểm bắt đầu
    dist[startNode.r][startNode.c] = 0;
    q.push(startNode);

    while (!q.empty()) {
        Point curr = q.front();
        q.pop();

        // Duyệt 4 hướng
        for (int i = 0; i < 4; i++) {
            int nr = curr.r + dr[i];
            int nc = curr.c + dc[i];

            // Kiểm tra biên
            if (nr >= 0 && nr < R && nc >= 0 && nc < C) {
                // Kiểm tra ô đó có phải là núi (#) không và đã thăm chưa
                if (mapData[nr][nc] != '#' && dist[nr][nc] == -1) {
                    dist[nr][nc] = dist[curr.r][curr.c] + 1;
                    q.push({nr, nc});
                }
            }
        }
    }
    return dist;
}

int main() {
    // Tối ưu tốc độ nhập xuất
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // 1. Nhập dữ liệu
    cin >> R >> C >> N;

    mapData.resize(R);
    // Điểm xuất phát luôn là (1,1) theo đề (tức 0,0 trong mảng)
    keyPoints.push_back({0, 0}); 

    // Nhập bản đồ và tìm vị trí các Trận Nhãn (S)
    for (int i = 0; i < R; i++) {
        cin >> mapData[i];
        for (int j = 0; j < C; j++) {
            if (mapData[i][j] == 'S') {
                keyPoints.push_back({i, j});
            }
        }
    }

    // Tổng số điểm quan trọng (Start + N Trận Nhãn)
    int totalNodes = N + 1; 

    // 2. Tính toán ma trận khoảng cách
    
    // Ma trận kết quả (N+1) x (N+1)
    vector<vector<int>> resultMatrix(totalNodes, vector<int>(totalNodes));

    for (int i = 0; i < totalNodes; i++) {
        // Chạy BFS từ điểm i để lấy khoảng cách đến mọi nơi
        vector<vector<int>> distMap = bfs_full_map(keyPoints[i]);

        for (int j = 0; j < totalNodes; j++) {
            Point target = keyPoints[j];
            resultMatrix[i][j] = distMap[target.r][target.c];
        }
    }

    // 3. In kết quả
    for (int i = 0; i < totalNodes; i++) {
        for (int j = 0; j < totalNodes; j++) {
            cout << resultMatrix[i][j] << (j == totalNodes - 1 ? "" : " ");
        }
        cout << "\n";
    }

    return 0;
}