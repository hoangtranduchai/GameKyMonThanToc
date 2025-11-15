# File: CMakeLists.txt

# Yêu cầu phiên bản CMake tối thiểu
cmake_minimum_required(VERSION 3.10)

# Đặt tên dự án và phiên bản
project(KyMonThanToc VERSION 1.0.0 LANGUAGES CXX)

# Thiết lập chuẩn C++17 (hoặc 20) hiện đại cho code đẳng cấp thế giới
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Bật Warning Levels cao để bắt lỗi code ngay từ đầu (giúp code không tì vết)
if(CMAKE_COMPILER_IS_GNU_CXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Các cờ tối ưu và cảnh báo nghiêm ngặt
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -g -O3")
endif()

# 1. Thêm thư mục chứa file header (.h) của dự án
include_directories(include)

# 2. Tìm kiếm các gói SDL2
# Pacman đã cài đặt các file cấu hình, nên CMake sẽ tìm thấy ngay lập tức.
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Định nghĩa các file nguồn (.cpp)
file(GLOB SOURCE_FILES
    "src/*.cpp"
)

# 3. Tạo tệp thực thi (executable)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Truyền biến PROJECT_ROOT_PATH vào code nguồn để sử dụng đường dẫn gốc
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    PROJECT_ROOT_PATH="${PROJECT_SOURCE_DIR}"
)

# 4. Liên kết các thư viện tìm được vào tệp thực thi
# Sử dụng tên biến và tên thư viện thủ công để đảm bảo MinGW-w64 liên kết đúng.
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Thư viện Core SDL2 (Sử dụng biến tìm kiếm)
    ${SDL2_LIBRARIES}

    # Các thư viện mở rộng bị thiếu (Liên kết thủ công bằng tên tiêu chuẩn)
    SDL2_image
    SDL2_ttf
    SDL2_mixer
)

# 5. [Tối ưu Windows/MSYS2] Đảm bảo DLL được copy (nếu cần)
# Tuy nhiên, trong môi trường MSYS2, khi bạn chạy từ terminal, các DLL thường được tìm thấy.
# Nếu bạn muốn chạy file .exe độc lập, bạn sẽ cần copy các DLL sau:
# SDL2.dll, SDL2_image.dll, SDL2_ttf.dll, SDL2_mixer.dll