# Game Kỳ Môn Thần Tốc

## Yêu cầu hệ thống

- Windows 10/11
- MSYS2 với MinGW64
- CMake 3.10+
- SDL2, SDL2_image, SDL2_ttf, SDL2_mixer

## Cài đặt môi trường

### 1. Cài đặt MSYS2
Tải và cài đặt từ: https://www.msys2.org/

### 2. Cài đặt các thư viện cần thiết
Mở MSYS2 terminal và chạy:

```bash
# Cập nhật package database
pacman -Syu

# Cài đặt build tools
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja

# Cài đặt SDL2
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-SDL2_image
pacman -S mingw-w64-x86_64-SDL2_ttf
pacman -S mingw-w64-x86_64-SDL2_mixer
```

### 3. Thêm vào PATH
Thêm `C:\msys64\mingw64\bin` vào System Environment Variables

## Build và chạy dự án

### Cách 1: Dùng CMake + Ninja

```bash
# Tạo thư mục build
mkdir build
cd build

# Configure
cmake .. -G Ninja

# Build
ninja

# Chạy game
./KyMonThanToc.exe
```

### Cách 2: Dùng VS Code

1. Mở thư mục dự án trong VS Code
2. Nhấn `Ctrl+Shift+B` để build
3. Nhấn `F5` để chạy và debug

## Cấu trúc thư mục

```
GameKyMonThanToc/
├── assets/           # Tài nguyên game (hình ảnh, âm thanh, font)
├── include/          # Header files (.h)
├── src/              # Source files (.cpp)
├── build/            # Thư mục build (không push lên Git)
├── CMakeLists.txt    # File cấu hình CMake
└── README.md         # File này
```

## Troubleshooting

### Lỗi "SDL2 not found"
```bash
# Kiểm tra SDL2 đã cài chưa
pkg-config --cflags --libs sdl2

# Nếu chưa có, cài lại
pacman -S mingw-w64-x86_64-SDL2
```

### Lỗi "PROJECT_ROOT_PATH undefined"
Đảm bảo build bằng CMake, không build trực tiếp bằng g++.