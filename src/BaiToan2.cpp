// -------------------------------------------------------------------------
// src/BaiToan2.cpp
// Giải quyết "Bài toán 2: Định Mệnh - Vạch Ra Thiên Mệnh"
// Thuật toán: Quy hoạch động Bitmask (Giải bài toán TSP)
// -------------------------------------------------------------------------

#include <iostream> // Dùng cho cin, cout
#include <vector>   // Dùng để lưu ma trận
#include <algorithm> // Dùng cho std::min
#include <string>   // Dùng cho std::string
#include <sstream>  // Dùng cho std::stringstream (để đọc ma trận)

// Định nghĩa giá trị VÔ CÙNG
// (Lớn hơn bất kỳ chi phí nào có thể)
const int INF = 1e9;

int N; // Số Trận Nhãn (Chúng ta có N+1 điểm, từ 0 đến N)
int numPoints; // Tổng số điểm = N + 1
std::vector<std::vector<int>> dist; // Ma trận khoảng cách (input)

// ----- Bước 1: Đọc Input (Ma trận Tri Thức) -----
void ReadInput() {
    std::cin >> N;
    numPoints = N + 1;

    // Khởi tạo ma trận khoảng cách
    dist.resize(numPoints, std::vector<int>(numPoints));

    // Đọc (N+1) hàng tiếp theo
    for (int i = 0; i < numPoints; ++i) {
        for (int j = 0; j < numPoints; ++j) {
            std::cin >> dist[i][j];
        }
    }
}

// ----- Bảng DP và Truy vết -----
// dp[mask][u] = Chi phí nhỏ nhất để thăm các điểm trong mask, kết thúc tại u
// 1 << numPoints = 2^(N+1)
std::vector<std::vector<int>> dp;
// path[mask][u] = Điểm 'v' (điểm ngay trước u) 
// để đạt được trạng thái dp[mask][u] tối ưu
std::vector<std::vector<int>> path;

// ----- Bước 2: Hàm giải thuật Bitmask DP -----
void SolveTSP() {
    // 1. Khởi tạo
    // Có 2^(numPoints) trạng thái mask
    int numStates = (1 << numPoints); 
    dp.resize(numStates, std::vector<int>(numPoints, INF));
    path.resize(numStates, std::vector<int>(numPoints, -1));

    // 2. Thiết lập trạng thái ban đầu (Base Case)
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

    // 4. Tìm kết quả (Chi phí nhỏ nhất)
    // 'mask' đã thăm tất cả các điểm
    int allVisitedMask = (1 << numPoints) - 1;
    int minCost = INF;
    int lastNode = -1; // Lưu lại điểm kết thúc của lộ trình

    // Tìm chi phí nhỏ nhất để đến 1 trong các Trận Nhãn (1 đến N)
    for (int u = 1; u < numPoints; ++u) {
        if (dp[allVisitedMask][u] < minCost) {
            minCost = dp[allVisitedMask][u];
            lastNode = u;
        }
    }

    // 5. Xử lý trường hợp không có đường đi
    if (minCost == INF) {
        std::cout << "-1" << std::endl;
        return;
    }

    // 6. Truy vết (Traceback) để tìm đường đi
    std::vector<int> finalPath; // (Biến 'finalPath' đã được khai báo ở đây)
    int currNode = lastNode;    // (Biến 'currNode' đã được khai báo ở đây)
    int currMask = allVisitedMask; // (Biến 'currMask' đã được khai báo ở đây)

    // Lặp N lần để lấy N+1 điểm
    for (int i = 0; i < N; ++i) { // (Vòng lặp 'for' chính xác ở đây)
        finalPath.push_back(currNode);
        int prevNode = path[currMask][currNode];
        currMask = currMask ^ (1 << currNode); // Tắt bit của currNode
        currNode = prevNode;
    }
    finalPath.push_back(0); // Thêm điểm bắt đầu (0)

    // 7. In ra kết quả (theo đúng định dạng Output)
    std::cout << minCost << std::endl;
    for (int i = finalPath.size() - 1; i >= 0; --i) {
        std::cout << finalPath[i] << (i == 0 ? "" : " ");
    }
    std::cout << std::endl;
}

// ----- Hàm Main (Điểm bắt đầu của chương trình console này) -----
int main() {
    // Tắt đồng bộ với C (để cin/cout chạy nhanh hơn)
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    // Bước 1: Đọc dữ liệu
    ReadInput();

    // Bước 1.1: Kiểm tra xem đã đọc đúng chưa
    std::cout << "--- KIEM TRA DU LIEU (BAI TOAN 2) ---" << std::endl;
    std::cout << "N = " << N << " (Tong so diem = " << numPoints << ")" << std::endl;
    std::cout << "Ma tran khoang cach da doc:" << std::endl;
    for (int i = 0; i < numPoints; ++i) {
        for (int j = 0; j < numPoints; ++j) {
            std::cout << dist[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;

    // Bước 2: Giải bài toán
    SolveTSP();

    return 0;
}