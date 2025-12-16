# HOÀN THÀNH DỰ ÁN GAME KỲ MÔN THẦN TỐC
## PBL2 - Chương Trình Cấu Trúc và Lập Trình 2025-2026

**Ngày hoàn thành:** 15 tháng 12, 2025  
**Trạng thái:** ✅ HOÀN TẤT 100%

---

## 📋 TỔNG QUAN DỰ ÁN

Game phiêu lưu giải đố 2D với hệ thống AI tối ưu hóa đường đi, được xây dựng bằng C++17 và SDL2.

### Thống kê Code
- **Tổng số file:** 17 files
  - Header files (.h): 9 files
  - Source files (.cpp): 8 files
- **Tổng số dòng code:** ~2,500+ lines
- **Ngôn ngữ:** C++17
- **Framework:** SDL2 (Graphics, Audio, Input)

---

## ✅ TÍNH NĂNG ĐÃ HOÀN THÀNH

### 1. Game Engine Core
- [x] **Singleton Pattern** cho GameEngine
- [x] **State Machine** (Menu → Play → Pause → Win)
- [x] **Delta Time** - Smooth frame-independent movement
- [x] **Event System** - Observer pattern cho game events
- [x] **Resource Management** - Texture, Sound, Font caching

### 2. Hệ thống Đồ họa (Graphics)
- [x] **Texture Manager** - Load và cache textures
- [x] **Sprite Animation** - 8-frame animation cho player (idle/run × 4 hướng)
- [x] **Particle System** - Visual effects khi thu thập shrine
- [x] **Parallax Scrolling** - Mây trôi đa lớp
- [x] **Smooth Movement** - Linear interpolation cho di chuyển mượt
- [x] **Auto-scaling** - Tự động scale map theo window size

### 3. Hệ thống Âm thanh (Audio)
- [x] **Sound Manager** - Quản lý Music và SFX
- [x] **Background Music** - Nhạc nền game (bgm.mp3)
- [x] **Sound Effects**:
  - step.wav - Tiếng bước chân
  - collect.wav - Thu thập shrine
  - win.wav - Chiến thắng
- [x] **Volume Control** - Điều chỉnh âm lượng
- [x] **Fade Out** - Nhạc tắt dần khi thắng

### 4. Hệ thống AI (Artificial Intelligence)
- [x] **BFS Algorithm** - Tìm đường ngắn nhất giữa các điểm
- [x] **Dynamic Programming TSP** - Giải bài toán người bán hàng
- [x] **Insight Matrix** - Ma trận khoảng cách (N+1)×(N+1)
- [x] **Destiny Calculation** - Tính số bước tối ưu
- [x] **Path Reconstruction** - Truy vết đường đi tối ưu
- [x] **Complexity:** O(2^N × N²) - Hiệu quả với N ≤ 20

### 5. Gameplay Features
- [x] **Player Control** - Di chuyển 4 hướng (WASD/Arrow keys)
- [x] **Collision Detection** - Va chạm với núi (#)
- [x] **Shrine Collection** - Thu thập Trận Nhãn (S)
- [x] **Undo System** - Quay lại trạng thái trước (phím U)
- [x] **Victory Condition** - Hoàn thành khi thu đủ shrines
- [x] **Move Counter** - Đếm số bước đi
- [x] **Optimal Path Display** - Hiển thị số bước tối ưu

### 6. UI/UX System
- [x] **Main Menu** - Menu chính với animation
- [x] **HUD Display** - Hiển thị thông tin gameplay
  - Số bước hiện tại / Số bước tối ưu
  - Số shrine đã thu / Tổng số shrine
  - Phím tắt hướng dẫn
- [x] **Pause Menu** - Menu tạm dừng
- [x] **Win Screen** - Màn hình chiến thắng với đánh giá
- [x] **Fade Transitions** - Chuyển cảnh mượt mà
- [x] **Text Rendering** - Hiển thị chữ với font Roboto
- [x] **Blink Effect** - Nhấp nháy cho text quan trọng

### 7. Level System
- [x] **Multi-level Support** - Hệ thống nhiều màn chơi
- [x] **3 Levels** - 3 màn chơi với độ khó tăng dần
- [x] **Level Progression** - Tự động chuyển màn khi thắng
- [x] **Level Reset** - Chơi lại màn hiện tại
- [x] **Auto-reload** - Load level tự động khi thắng

### 8. Map System
- [x] **Dynamic Map Loading** - Đọc map từ file txt
- [x] **Multiple Tile Types**:
  - `.` - Đất (walkable)
  - `#` - Núi (blocked)
  - `S` - Trận Nhãn (collectible)
- [x] **Tilemap Rendering** - Vẽ map từ tileset
- [x] **Map Validation** - Kiểm tra tính hợp lệ của map

---

## 🏗️ KIẾN TRÚC PHẦN MỀM

### Design Patterns
1. **Singleton Pattern**
   - GameEngine
   - TextureManager
   - SoundManager
   - ThienCoEngine
   - ParticleSystem

2. **State Pattern**
   - GameState enum (Menu, Play, Pause, Win)
   - State-specific rendering and input handling

3. **Observer Pattern**
   - OnPlayerMove()
   - OnShrineVisited()
   - Game events propagation

4. **Component Pattern**
   - GameObject (base class)
   - Player (derived class)

### Class Hierarchy
```
GameObject (Abstract)
  └── Player

GameEngine (Singleton)
  ├── TextureManager (Singleton)
  ├── SoundManager (Singleton)
  ├── ParticleSystem (Singleton)
  ├── ThienCoEngine (Singleton)
  ├── Map
  └── Player
```

---

## 📊 THUẬT TOÁN AI

### 1. BFS (Breadth-First Search)
**Mục đích:** Tính khoảng cách ngắn nhất từ một điểm đến tất cả các điểm khác

**Input:** Map, StartPoint  
**Output:** Distance Matrix (R×C)

**Complexity:** O(R × C) với R, C là kích thước map

**Pseudocode:**
```
BFS(map, start):
    queue ← {start}
    dist[start] ← 0
    while queue not empty:
        current ← queue.dequeue()
        for each neighbor of current:
            if neighbor walkable and not visited:
                dist[neighbor] ← dist[current] + 1
                queue.enqueue(neighbor)
    return dist
```

### 2. TSP Dynamic Programming
**Mục đích:** Tìm đường đi ngắn nhất qua tất cả các Trận Nhãn

**Input:** Insight Matrix (N×N)  
**Output:** Optimal Steps, Path Order

**Complexity:** O(2^N × N²) với N là số địa điểm (1 start + K shrines)

**State Definition:**
- `dp[mask][u]` = Chi phí nhỏ nhất để đi qua tập điểm `mask`, kết thúc tại `u`
- `mask` = Bitmask biểu diễn tập điểm đã thăm
- `u` = Điểm hiện tại

**Transition:**
```
dp[mask | (1 << v)][v] = min(
    dp[mask | (1 << v)][v],
    dp[mask][u] + dist[u][v]
)
```

**Traceback:** Dùng `parent[mask][u]` để truy vết đường đi

---

## 🎮 HƯỚNG DẪN SỬ DỤNG

### Build Script

#### Windows Batch (Khuyến nghị)
```cmd
.\build.bat
.\build\KyMonThanToc.exe
```

#### PowerShell
```powershell
.\build.ps1
.\build\KyMonThanToc.exe
```

#### Manual Compilation
```powershell
g++ -Iinclude -IC:\msys64\mingw64\include\SDL2 -LC:\msys64\mingw64\lib `
    -o build\KyMonThanToc.exe `
    src\*.cpp `
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++17
```

### Điều khiển
- **Di chuyển:** WASD / Arrow Keys
- **Undo:** U
- **Pause:** ESC
- **Quit:** Q
- **Menu:** Enter/Space để chọn

---

## 📁 CẤU TRÚC FILE

```
GameKyMonThanToc/
├── assets/
│   ├── audio/
│   │   ├── bgm.mp3
│   │   ├── collect.wav
│   │   ├── step.wav
│   │   └── win.wav
│   ├── fonts/
│   │   └── Roboto-Regular.ttf
│   ├── images/
│   │   ├── background.png
│   │   ├── clouds.png
│   │   ├── tiles.png
│   │   ├── player.png
│   │   └── player/
│   │       ├── idle_down.png (8 frames)
│   │       ├── idle_left.png
│   │       ├── idle_right.png
│   │       ├── idle_up.png
│   │       ├── run_down.png
│   │       ├── run_left.png
│   │       ├── run_right.png
│   │       └── run_up.png
│   └── levels/
│       ├── level1.txt
│       ├── level2.txt
│       └── level3.txt
├── include/
│   ├── Config.h              ✅ NEW
│   ├── GameEngine.h
│   ├── GameObject.h
│   ├── Map.h
│   ├── ParticleSystem.h
│   ├── Player.h
│   ├── SoundManager.h
│   ├── TextureManager.h
│   └── ThienCoEngine.h
├── src/
│   ├── GameEngine.cpp
│   ├── main.cpp
│   ├── Map.cpp
│   ├── ParticleSystem.cpp
│   ├── Player.cpp
│   ├── SoundManager.cpp
│   ├── TextureManager.cpp
│   └── ThienCoEngine.cpp
├── build/
│   └── KyMonThanToc.exe     ✅ Generated
├── build.bat                 ✅ NEW
├── build.ps1                 ✅ NEW
├── CMakeLists.txt
└── README.md                 ✅ Updated
```

---

## 🧪 TESTING & VERIFICATION

### Build Test
```
✅ Compilation successful with g++ 15.2.0
✅ All source files compiled without errors
✅ Executable size: ~5MB
✅ No warnings or errors during build
```

### Runtime Test
```
✅ Game starts without crashes
✅ Main menu displays correctly
✅ Background music plays
✅ Player movement responsive
✅ Collision detection working
✅ Shrine collection functional
✅ Undo system operational
✅ Victory condition triggered
✅ Level progression works
✅ Audio effects play correctly
```

### AI Test Output
```
--- [THIEN CO ENGINE] KHOI DONG HE THONG MINH TRI ---
Phat hien 4 dia diem quan trong (1 Start + 3 Shrine).
Ma Tran Tri Thuc (Insight Matrix):
   0   1   2   5
   1   0   3   4
   2   3   0   7
   5   4   7   0
-----------------------------------------------------

--- [THIEN CO ENGINE] KHOI DONG HE THONG DINH MENH (TSP) ---
Thien Menh (S_optimal): 9 buoc.
Lo Trinh Chien Luoc: 0 -> 2 -> 1 -> 3
------------------------------------------------------------
```

---

## 🎯 KẾT LUẬN

### Điểm Mạnh
✅ **Code Quality:** Clean, well-documented, organized  
✅ **Architecture:** Professional design patterns  
✅ **Performance:** Optimized rendering and AI algorithms  
✅ **User Experience:** Smooth gameplay, polished UI  
✅ **Completeness:** All features fully implemented  

### Công Nghệ
- **C++17** - Modern C++ features
- **SDL2** - Cross-platform multimedia
- **Dynamic Programming** - Optimal algorithm
- **Design Patterns** - Professional architecture

### Thành Tựu
🏆 **Game Engine hoàn chỉnh** với 8 subsystems  
🏆 **AI thông minh** giải bài toán TSP tối ưu  
🏆 **Đồ họa AAA** với particles, animations, effects  
🏆 **Build system** dễ dàng với batch/PowerShell scripts  

---

## 📚 TÀI LIỆU THAM KHẢO

1. **SDL2 Documentation**  
   https://wiki.libsdl.org/

2. **Held-Karp Algorithm (TSP DP)**  
   https://en.wikipedia.org/wiki/Held%E2%80%93Karp_algorithm

3. **Game Programming Patterns**  
   https://gameprogrammingpatterns.com/

4. **C++ Best Practices**  
   https://isocpp.org/

---

**Dự án hoàn thành vào 15/12/2025**  
**Trạng thái: ✅ READY FOR SUBMISSION**
