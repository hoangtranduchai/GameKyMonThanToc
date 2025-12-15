# Game Kỳ Môn Thần Tốc

**Dự án PBL2 - Chương Trình Cấu Trúc và Lập Trình 2025-2026**

Game phiêu lưu giải đố với AI tối ưu hóa đường đi (TSP Solver), được xây dựng bằng C++17 và SDL2.

## Tính năng

✅ **Game Engine hoàn chỉnh** với hệ thống State Machine  
✅ **AI thông minh** - Tính toán đường đi tối ưu bằng Dynamic Programming  
✅ **Đồ họa AAA** - Particle effects, smooth animation, parallax scrolling  
✅ **Âm thanh đầy đủ** - Background music, sound effects  
✅ **Hệ thống Level** - 3 màn chơi với độ khó tăng dần  
✅ **Undo/Redo** - Hệ thống quay lại trạng thái trước đó  
✅ **Auto-scaling** - Tự động điều chỉnh kích thước map theo màn hình  

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

### Cách 1: Sử dụng Build Script (Khuyến nghị)

#### Windows Batch File (Dễ nhất)
```cmd
# Biên dịch dự án
.\build.bat

# Chạy game
.\build\KyMonThanToc.exe
```

#### PowerShell Script (Nếu đã bật execution policy)
```powershell
# Biên dịch dự án
.\build.ps1

# Chạy game
.\build\KyMonThanToc.exe
```

### Cách 2: Biên dịch thủ công với g++

```powershell
g++ -Iinclude -IC:\msys64\mingw64\include\SDL2 -LC:\msys64\mingw64\lib `
    -o build\KyMonThanToc.exe `
    src\GameEngine.cpp src\main.cpp src\Map.cpp src\ParticleSystem.cpp `
    src\Player.cpp src\SoundManager.cpp src\TextureManager.cpp src\ThienCoEngine.cpp `
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++17
```

### Cách 3: Dùng CMake + Ninja (Nâng cao)

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

### Cách 4: Dùng VS Code

1. Mở thư mục dự án trong VS Code
2. Nhấn `Ctrl+Shift+B` để build
3. Nhấn `F5` để chạy và debug

## Điều khiển Game

### Menu
- `↑/↓` - Di chuyển lựa chọn
- `Enter/Space` - Xác nhận
- `Q` - Thoát game

### Trong Game
- `W/↑` - Di chuyển lên
- `S/↓` - Di chuyển xuống
- `A/←` - Di chuyển trái
- `D/→` - Di chuyển phải
- `U` - Undo (Quay lại bước trước)
- `ESC` - Tạm dừng
- `Q` - Thoát game

### Màn hình Pause
- `ESC` - Tiếp tục chơi
- `M` - Về menu chính
- `Q` - Thoát game

### Màn hình Chiến thắng
- `Enter/Space` - Màn tiếp theo
- `R` - Chơi lại màn hiện tại
- `M` - Về menu chính

## Cấu trúc thư mục

```
GameKyMonThanToc/
├── assets/                    # Tài nguyên game
│   ├── audio/                 # Âm thanh (BGM, SFX)
│   ├── fonts/                 # Font chữ (Roboto)
│   ├── images/                # Hình ảnh (background, tiles, clouds)
│   │   └── player/            # Sprite sheets nhân vật
│   └── levels/                # File level (level1.txt, level2.txt, level3.txt)
├── include/                   # Header files (.h)
│   ├── Config.h               # Cấu hình đường dẫn dự án
│   ├── GameEngine.h           # Game engine chính
│   ├── GameObject.h           # Lớp cơ sở cho game objects
│   ├── Map.h                  # Hệ thống bản đồ
│   ├── ParticleSystem.h       # Hệ thống particle effects
│   ├── Player.h               # Nhân vật người chơi
│   ├── SoundManager.h         # Quản lý âm thanh
│   ├── TextureManager.h       # Quản lý texture/font
│   └── ThienCoEngine.h        # AI tối ưu hóa (BFS + TSP)
├── src/                       # Source files (.cpp)
│   ├── GameEngine.cpp         # Triển khai game engine
│   ├── main.cpp               # Entry point
│   ├── Map.cpp                # Triển khai bản đồ
│   ├── ParticleSystem.cpp     # Triển khai particle system
│   ├── Player.cpp             # Triển khai player
│   ├── SoundManager.cpp       # Triển khai sound manager
│   ├── TextureManager.cpp     # Triển khai texture manager
│   └── ThienCoEngine.cpp      # Triển khai AI (BFS + Dynamic Programming)
├── build/                     # Thư mục build (tạo tự động)
├── build.ps1                  # Script build PowerShell
├── CMakeLists.txt             # File cấu hình CMake
└── README.md                  # File hướng dẫn
```

## Kiến trúc Game

### Game Engine Architecture
- **Singleton Pattern**: GameEngine, TextureManager, SoundManager, ParticleSystem
- **State Machine**: Menu → Play → Pause → Win
- **Component-Based**: GameObject → Player
- **Observer Pattern**: Event-driven (OnPlayerMove, OnShrineVisited)

### AI System (ThienCoEngine)
1. **BFS (Breadth-First Search)**: Tính khoảng cách ngắn nhất giữa các điểm
2. **Dynamic Programming**: Giải bài toán TSP (Traveling Salesman Problem)
   - State: `dp[mask][u]` - Chi phí nhỏ nhất để đến tập điểm `mask`, kết thúc tại `u`
   - Complexity: O(2^N * N^2) với N là số Trận Nhãn
3. **Traceback**: Phục hồi đường đi tối ưu từ DP table

## Troubleshooting

### Lỗi "SDL2 not found"
```bash
# Kiểm tra SDL2 đã cài chưa
pkg-config --cflags --libs sdl2

# Nếu chưa có, cài lại
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
```

### Lỗi "PROJECT_ROOT_PATH undefined"
- Đảm bảo file `include/Config.h` tồn tại
- Kiểm tra đường dẫn trong Config.h khớp với vị trí dự án

### Lỗi biên dịch "SDL.h: No such file or directory"
- Kiểm tra SDL2 đã cài vào đúng MinGW64:
  ```powershell
  Test-Path C:\msys64\mingw64\include\SDL2\SDL.h
  ```
- Thêm `C:\msys64\mingw64\bin` vào PATH

### Game không chạy hoặc crash
- Kiểm tra các DLL SDL2 có trong PATH
- Copy các DLL sau vào thư mục `build\`:
  - SDL2.dll
  - SDL2_image.dll
  - SDL2_ttf.dll
  - SDL2_mixer.dll
- Kiểm tra file assets (hình ảnh, âm thanh, level) tồn tại đầy đủ

### Lỗi "Could not open file" khi chạy
- Kiểm tra đường dẫn trong `include/Config.h` đúng chưa
- Chạy game từ thư mục gốc của dự án (không phải từ thư mục build)

## Công nghệ sử dụng

- **Ngôn ngữ**: C++17
- **Graphics**: SDL2, SDL2_image
- **Font**: SDL2_ttf
- **Audio**: SDL2_mixer
- **Build System**: CMake 3.10+, Ninja
- **Compiler**: MinGW-w64 GCC 15.2.0

## Đóng góp

Dự án này được phát triển cho môn PBL2 tại Đại học Bách Khoa Đà Nẵng.

**Nhóm phát triển:**
- Game Engine & Rendering System
- AI Optimization System (BFS + TSP)
- Asset Management & Level Design

## License

Dự án này được phát triển cho mục đích học tập tại Đại học Bách Khoa Đà Nẵng.

## Tài liệu tham khảo

- SDL2 Documentation: https://wiki.libsdl.org/
- Dynamic Programming TSP: https://en.wikipedia.org/wiki/Held%E2%80%93Karp_algorithm
- Game Programming Patterns: https://gameprogrammingpatterns.com/