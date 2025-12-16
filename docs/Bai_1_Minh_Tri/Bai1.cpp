#include <iostream>
#include <vector>
#include <string>
#include <queue>

using namespace std;

struct Point {
    int r, c;
};

class MapKnowledgeBuilder {
    int R, C, N;
    vector<string> grid;                // Lưu trữ bản đồ trận đồ
    vector<Point> important_points;     // Lưu tọa độ các điểm quan trọng

    // Hàm thực thi thuật toán BFS từ một điểm nguồn (start_Point)
    // Trả về một bản đồ khoảng cách 2D từ điểm nguồn đến mọi ô khác
    // Hàm không thay đổi trạng thái của đối tượng nên được đánh dấu là 'const'
    vector<vector<int>> bfs(Point start_point) const {
        // Khởi tạo bản đồ khoảng cách, -1 nghĩa là chưa được thăm
        vector<vector<int>> distance(R, vector<int>(C, -1));

        // Hàng đợi cho BFS, lưu trữ tọa độ {hàng, cột}
        queue<Point> q;

        // Bắt đầu từ điểm nguồn
        distance[start_point.r][start_point.c] = 0;
        q.push({start_point.r, start_point.c});

        // Mảng định hướng để di chuyển 4 hướng: Phải, Trái, Xuống, Lên
        int dr[4] = {0, 0, 1, -1};
        int dc[4] = {1, -1, 0, 0};

        // Vòng lặp chính của BFS
        while (!q.empty()) {
            // Lấy ô hiện tại ra khỏi hàng đợi
            Point current = q.front();
            q.pop();

            // Duyệt qua 4 hàng xóm kề cạnh
            for (int i = 0; i < 4; ++i) {
                int nr = current.r + dr[i]; // Tọa độ hàng mới
                int nc = current.c + dc[i]; // Tọa độ cột mới

                // Kiểm tra các điều kiện hợp lệ của ô hàng xóm:
                // 1. Nằm trong biên của bản đồ
                // 2. Không phải là núi ('#')
                // 3. Chưa được thăm (distance == -1)
                if (nr >= 0 && nr < R && nc >= 0 && nc < C && grid[nr][nc]!= '#' && distance[nr][nc] == -1) {
                    // Cập nhật khoảng cách và đưa vào hàng đợi để duyệt tiếp
                    distance[nr][nc] = distance[current.r][current.c] + 1;
                    q.push({nr, nc});
                }
            }
        }
        return distance;
    }

public:
    // Constructor nhận dữ liệu bản đồ và khởi tạo các điểm quan trọng
    MapKnowledgeBuilder(int r, int c, int n, const vector<string>& map_data)
        : R(r), C(c), N(n), grid(map_data) {
        
        important_points.push_back({0, 0}); // Điểm xuất phát
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                if (grid[i][j] == 'S') {
                    important_points.push_back({i, j});
                }
            }
        }
    }

    // Phương thức thực hiện tính toán và trả về ma trận khoảng cách
    vector<vector<int>> computeDistanceMatrix() const {
        vector<vector<int>> dist_matrix(N + 1, vector<int>(N + 1));
        for (int i = 0; i <= N; ++i) {
            // Chạy BFS từ mỗi điểm quan trọng
            vector<vector<int>> current_distances = bfs(important_points[i]);

            // Trích xuất khoảng cách đến các điểm quan trọng khác
            for (int j = 0; j <= N; ++j) {
                dist_matrix[i][j] = current_distances[important_points[j].r][important_points[j].c];
            }
        }
        return dist_matrix;
    }
};

int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
    
    // Đọc kích thước bản đồ và số lượng Trận Nhãn
    int R, C, N;
    cin >> R >> C >> N;

    // Grid để lưu bản đồ
    vector<string> grid(R);
    for (int i = 0; i < R; ++i) {
        cin >> grid[i];
    }

    // Tạo một đối tượng của lớp
    MapKnowledgeBuilder knowledge_builder(R, C, N, grid);
    
    // Gọi phương thức để lấy kết quả
    vector<vector<int>> dist_matrix = knowledge_builder.computeDistanceMatrix();

    // In kết quả
    for (int i = 0; i <= N; ++i) {
        for (int j = 0; j <= N; ++j) {
            cout << dist_matrix[i][j] << (j == N ? "" : " ");
        }
        cout << "\n";
    }
    return 0;
}