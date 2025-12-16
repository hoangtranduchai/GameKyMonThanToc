#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

// Khai báo hằng số cho giá trị vô cùng lớn, dùng để khởi tạo chi phí.
const int INF = 1e9; 

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

// Cấu trúc (struct) để đóng gói kết quả trả về một cách tường minh,
// bao gồm tổng chi phí tối thiểu và lộ trình chi tiết.
struct TSPSolution {
    int cost;
    vector<int> path;
};

class TSPSolver {
public:
    TSPSolution solve(const vector<vector<int>>& dist_matrix) const {
        // Tổng số địa điểm từ kích thước của ma trận
        // 0 là điểm xuất phát, 1 đến N là Trận Nhãn
        const int num_points = dist_matrix.size();

        // Xử lý trường hợp biên: nếu không có hoặc chỉ có một điểm, chi phí là 0.
        if (num_points <= 1) {
            return {0, {0}};
        }

        // Bảng quy hoạch động: dp[mask][i]
        // Lưu trữ chi phí nhỏ nhất để đi qua tập các điểm trong 'mask' và kết thúc tại điểm 'i'.
        // Kích thước: (2^num_points) x num_points.
        // 'mask' là một số nguyên, bit thứ i của mask bật (bằng 1) nếu i đã được thăm.
        vector<vector<int>> dp(1 << num_points, vector<int>(num_points, INF));

        // Bảng truy vết: parent[mask][i]
        // Lưu lại điểm ngay trước điểm 'i' trong lộ trình tối ưu đến trạng thái (mask, i).
        vector<vector<int>> parent(1 << num_points, vector<int>(num_points, -1));

        // Trường hợp cơ sở (Base Case)
        // Bắt đầu tại điểm 0, chỉ thăm điểm 0, chi phí là 0
        // mask = 1 (0...001) biểu thị chỉ thăm điểm 0
        dp[1 << 0][0] = 0;

        // Vòng lặp chính của DP
        // Duyệt qua tất cả các tập con các điểm đã thăm, được biểu diễn bằng 'mask'.
        for (int mask = 1; mask < (1 << num_points); ++mask) {
            // Với mỗi tập con, duyệt qua tất cả các điểm 'i' có thể là điểm kết thúc của lộ trình.
            for (int i = 0; i < num_points; ++i) {
                // Kiểm tra xem điểm 'i' có thực sự nằm trong tập 'mask' hay không.
                if (mask & (1 << i)) {
                    // Nếu trạng thái (mask, i) không thể đạt tới (chi phí vẫn là vô cùng), bỏ qua.
                    if (dp[mask][i] == INF) continue;

                    // Từ điểm 'i', thử "đẩy" (push) kết quả đến các điểm 'j' chưa được thăm.
                    for (int j = 0; j < num_points; ++j) {
                        // Kiểm tra xem điểm 'j' đã nằm trong 'mask' chưa.
                        if (!(mask & (1 << j))) {
                            // Nếu không có đường đi từ i đến j thì bỏ qua
                            if (dist_matrix[i][j] == -1) continue;

                            // Tạo trạng thái mới bằng cách thêm điểm 'j' vào 'mask'.
                            int new_mask = mask | (1 << j);
                            // Tính toán chi phí mới để đến trạng thái (new_mask, j) thông qua 'i'.
                            int new_cost = dp[mask][i] + dist_matrix[i][j];

                            // Nếu chi phí mới này tốt hơn chi phí hiện tại, cập nhật lại.
                            if (new_cost < dp[new_mask][j]) {
                                dp[new_mask][j] = new_cost;
                                // Ghi lại truy vết: để đến được 'j' trong 'new_mask', ta đã đi từ 'i'.
                                parent[new_mask][j] = i;
                            }
                        }
                    }
                }
            }
        }

        // Tìm kết quả cuối cùng
        // Kết quả là chi phí nhỏ nhất để thăm tất cả các điểm,
        // (mask = (1 << n) - 1, là mask có tất cả các bit đều được bật)
        // và kết thúc ở một 'i' bất kỳ (0 <= i < n).
        int final_mask = (1 << num_points) - 1; // Mask tất cả
        int min_cost = INF;
        int last_point = -1; // Lưu lại điểm kết thúc của lộ trình tối ưu.

        // Lộ trình phải kết thúc ở một Trận Nhãn (từ 1 đến N)
        // Tìm chi phí nhỏ nhất trong tất cả các khả năng kết thúc này.
        for (int i = 1; i < num_points; ++i) {
            if (dp[final_mask][i] < min_cost) {
                min_cost = dp[final_mask][i];
                last_point = i;
            }
        }

        // Nếu không tìm thấy lộ trình (min_cost vẫn là INF), trả về -1
        if (min_cost == INF) {
            return {-1, {}};
        }

        // Truy vết đường đi
        vector<int> path;
        int current_mask = final_mask;
        int current_point = last_point; // Bắt đầu từ điểm kết thúc

        // Đi ngược từ điểm cuối cùng về điểm xuất phát bằng cách sử dụng bảng 'parent'.
        while(current_point != -1) {
            path.push_back(current_point);
            
            // Lấy điểm trước đó từ mảng parent
            int prev_point = parent[current_mask][current_point];
            
            // Cập nhật mask về trạng thái trước đó (tắt bit của current_point)
            current_mask ^= (1 << current_point);
            
            // Di chuyển về trước đó
            current_point = prev_point;
        }

        // Đảo ngược vector để có được lộ trình theo đúng thứ tự từ đầu đến cuối.
        reverse(path.begin(), path.end());

        // Trả về kết quả dưới dạng một đối tượng TSPSolution.
        return {min_cost, path};
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

    // 2. Giai đoạn "Minh Trí": Sử dụng MapKnowledgeBuilder để tính ma trận khoảng cách.
    MapKnowledgeBuilder knowledge_builder(R, C, N, grid);
    vector<vector<int>> dist_matrix = knowledge_builder.computeDistanceMatrix();

    // 3. Giai đoạn "Định Mệnh": Sử dụng TSPSolver để tìm lộ trình tối ưu từ ma trận khoảng cách.
    TSPSolver solver;
    TSPSolution solution = solver.solve(dist_matrix);

    // In kết quả
    // Kiểm tra xem chi phí có phải là INF không ?
    // Nếu vẫn là INF (1e9), nghĩa là không tìm thấy đường đi
    if (solution.cost == INF) {
        cout << -1 << "\n";
    } else {
        cout << solution.cost << "\n";
        for (int i = 0; i < solution.path.size(); ++i) {
            cout << solution.path[i] << (i == solution.path.size() - 1? "" : " ");
        }
        cout << "\n";
    }
    return 0;
}