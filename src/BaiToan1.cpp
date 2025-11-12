// -------------------------------------------------------------------------
// src/BaiToan1.cpp
// Giải quyết "Bài toán 1: Minh Trí - Khai Mở Tri Thức Trận Đồ"
// Đây là một CHƯƠNG TRÌNH CONSOLE ĐỘC LẬP.
// -------------------------------------------------------------------------

#include <iostream> // Dùng cho cin, cout
#include <vector>   // Dùng để lưu bản đồ, lưu các điểm
#include <string>   // Dùng để đọc bản đồ
#include <queue>    // Dùng cho thuật toán BFS
#include <utility>  // Dùng cho std::pair

// ----- Bước 1: Định nghĩa các cấu trúc dữ liệu cơ bản -----

// Định nghĩa 1 "Điểm" (Point) trên bản đồ
// (Chúng ta có thể dùng struct, hoặc đơn giản là std::pair)
// first = row (hàng), second = col (cột)
typedef std::pair<int, int> Point;

int R, C, N; // Kích thước bản đồ và số Trận Nhãn
std::vector<std::string> grid; // Lưu bản đồ
std::vector<Point> importantPoints; // Lưu N+1 điểm quan trọng

// ----- Bước 2: Hàm đọc Input và Tìm các điểm quan trọng -----
void ReadInputAndFindPoints() {
    // Đọc R, C, N
    std::cin >> R >> C >> N;

    // Thêm Điểm 0 (Điểm xuất phát)
    // Đề bài nói (1, 1), trong mảng 0-indexed nó là (0, 0)
    importantPoints.push_back(Point(0, 0));

    // Đọc bản đồ (grid)
    grid.resize(R); // Đảm bảo vector<string> có R hàng
    for (int i = 0; i < R; ++i) {
        std::cin >> grid[i];

        // Trong khi đọc, tìm luôn các Trận Nhãn 'S'
        for (int j = 0; j < C; ++j) {
            if (grid[i][j] == 'S') {
                // Thêm vào danh sách các điểm quan trọng
                // theo đúng thứ tự (trên-xuống, trái-qua)
                importantPoints.push_back(Point(i, j));
            }
        }
    }
}

// ----- Bước 3: Thuật toán BFS -----
// Hàm này sẽ tính toán khoảng cách từ 1 điểm BẮT ĐẦU (startPoint)
// đến TẤT CẢ các điểm khác trên bản đồ.
std::vector<std::vector<int>> RunBFS(Point startPoint) {
    
    // 1. Tạo một bản đồ khoảng cách (distance map) 2D
    // Khởi tạo tất cả bằng -1 (ý nghĩa: chưa đến được)
    std::vector<std::vector<int>> dist(R, std::vector<int>(C, -1));

    // 2. Tạo một Hàng đợi (Queue) cho BFS
    // Chúng ta sẽ tự cài đặt Queue sau, giờ tạm dùng std::queue
    std::queue<Point> q;

    // 3. Thiết lập điểm bắt đầu
    dist[startPoint.first][startPoint.second] = 0; // Khoảng cách tới chính nó là 0
    q.push(startPoint); // Đẩy điểm bắt đầu vào hàng đợi

    // 4. Định nghĩa 4 hướng di chuyển (trên, dưới, trái, phải)
    // dr = delta row (thay đổi hàng), dc = delta col (thay đổi cột)
    int dr[] = {-1, 1, 0, 0}; // Hàng: lên, xuống, đứng yên, đứng yên
    int dc[] = {0, 0, -1, 1}; // Cột: đứng yên, đứng yên, trái, phải

    // 5. Bắt đầu vòng lặp BFS
    while (!q.empty()) {
        // Lấy điểm hiện tại ra khỏi hàng đợi
        Point u = q.front();
        q.pop();

        // Thử đi theo cả 4 hướng
        for (int i = 0; i < 4; ++i) {
            int newR = u.first + dr[i];
            int newC = u.second + dc[i];

            // 6. Kiểm tra xem điểm mới (newR, newC) có hợp lệ không
            // Điều kiện hợp lệ:
            // 1. Nằm trong bản đồ (>= 0 và < R, C)
            // 2. Không phải là núi ('#')
            // 3. Chưa được thăm (dist == -1)
            if (newR >= 0 && newR < R && newC >= 0 && newC < C &&
                grid[newR][newC] != '#' &&
                dist[newR][newC] == -1) 
            {
                // Nếu hợp lệ:
                // Cập nhật khoảng cách
                dist[newR][newC] = dist[u.first][u.second] + 1;
                // Đẩy điểm mới này vào hàng đợi để xét tiếp
                q.push(Point(newR, newC));
            }
        }
    }
    
    // 7. Trả về bản đồ khoảng cách
    return dist;
}

// ----- Hàm Main (Điểm bắt đầu của chương trình console này) -----
int main() {
    // Tắt đồng bộ với C (để cin/cout chạy nhanh hơn)
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // Bước 2.1: Đọc dữ liệu
    ReadInputAndFindPoints();

    // Bước 2.2: Kiểm tra xem đã đọc đúng chưa
    std::cout << "--- KIEM TRA DU LIEU ---" << std::endl;
    std::cout << "R: " << R << ", C: " << C << ", N: " << N << std::endl;
    std::cout << "Tim thay " << importantPoints.size() << " diem quan trong:" << std::endl;
    for (int i = 0; i < importantPoints.size(); ++i) {
        std::cout << "Diem " << i << ": (" 
                  << importantPoints[i].first << ", " 
                  << importantPoints[i].second << ")" << std::endl;
    }
    std::cout << "-------------------------" << std::endl;

    // ----- Bước 3: Tính toán Ma trận Tri Thức -----
    
    // Tạo ma trận kết quả (N+1) x (N+1)
    int numPoints = N + 1;
    std::vector<std::vector<int>> knowledgeMatrix(numPoints, std::vector<int>(numPoints));

    // Chạy BFS (N+1) lần
    for (int i = 0; i < numPoints; ++i) {
        // Lấy điểm bắt đầu (Điểm 0, Điểm 1, ...)
        Point startPoint = importantPoints[i];

        // Chạy BFS từ điểm này để lấy bản đồ khoảng cách
        std::vector<std::vector<int>> distMap = RunBFS(startPoint);

        // Dùng bản đồ này để điền vào Ma trận Tri Thức
        // Điền vào hàng thứ 'i' của ma trận
        for (int j = 0; j < numPoints; ++j) {
            // Lấy tọa độ của điểm đích
            Point endPoint = importantPoints[j];
            
            // Lấy khoảng cách từ bản đồ distMap
            knowledgeMatrix[i][j] = distMap[endPoint.first][endPoint.second];
        }
    }

    // ----- Bước 4: In ra Ma trận Tri Thức -----
    // (Đây là Output cuối cùng của Bài toán 1)

    for (int i = 0; i < numPoints; ++i) {
        for (int j = 0; j < numPoints; ++j) {
            std::cout << knowledgeMatrix[i][j] << (j == numPoints - 1 ? "" : " ");
        }
        std::cout << std::endl;
    }

    return 0;
}