# Game Kỳ Môn Thần Tốc

Dự án PBL2 (CTCT & LT 2025-2026) – game phiêu lưu giải đố tile-based, xây dựng bằng C++17 + SDL2. AI nội bộ tính số bước tối ưu để đánh giá hạng (S/A) cho từng màn.

## Điểm nổi bật
- State machine đầy đủ: Intro → Story (name/sensei) → Play → Result (S/A/Thua) → Continue.
- AI Thiên Cơ: BFS tính khoảng cách, DP bitmask giải TSP để tìm số bước tối ưu cho từng bản đồ.
- 22 level văn bản (level1.txt … level22.txt) được quét tự động trong assets/levels.
- Hoàn tác nhiều bước (Undo) dựa trên stack trạng thái bản đồ + người chơi.
- Tự động load texture/âm thanh theo thư mục, phóng tỷ lệ logic 1280x720, toggle fullscreen và bật/tắt âm thanh runtime.

## Yêu cầu hệ thống
- Windows 10/11
- MSYS2 (MinGW64) với gcc, CMake ≥ 3.10, Ninja
- SDL2, SDL2_image, SDL2_ttf, SDL2_mixer (bản MinGW64)

## Cài đặt môi trường (MSYS2 MinGW64)
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image \
           mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
```
Thêm `C:\msys64\mingw64\bin` vào PATH để Windows tìm thấy các DLL SDL2.

## Build và chạy
Khuyến nghị: dùng MSYS2 MinGW64 shell hoặc VS Code cấu hình MinGW.

### CMake + Ninja
```bash
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/KyMonThanToc.exe   # PowerShell: .\build\KyMonThanToc.exe
```

### Biên dịch g++ tối giản (không khuyến khích)
```powershell
g++ -std=c++17 -Iinclude -IC:\msys64\mingw64\include\SDL2 -LC:\msys64\mingw64\lib `
    src\main.cpp src\Core\GameEngine.cpp src\Entities\Map.cpp src\Entities\Player.cpp `
    src\Graphics\TextureManager.cpp src\Audio\SoundManager.cpp src\Algorithms\ThienCoEngine.cpp `
    -o build\KyMonThanToc.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

> Lưu ý: chạy game từ thư mục gốc dự án để asset relative path hoạt động; nếu cần, copy các DLL SDL2* vào build/ cùng exe.

## Điều khiển
- Di chuyển: W/A/S/D hoặc ↑/↓/←/→.
- Hoàn tác: U (trong game, hoặc quay lại game nếu đang ở màn hình kết quả/intro).
- Tiến trình cốt truyện/kết quả: Enter.
- Reset về Intro + Level 1: 1.
- Bật/tắt âm thanh: 2.
- Toàn màn hình: F11.
- Thoát game: Q.

Luồng màn: Intro → Enter → Story Name → Enter → Story Sensei → Enter để vào level; sau chiến thắng Rank S chuyển level tiếp theo, Rank A hoặc thua Enter để chơi lại; sau level 3 đạt Rank S sẽ vào màn Continue trước khi chơi level 4+.

## Cấu trúc thư mục
```
assets/            # Âm thanh, hình ảnh, font Roboto, level*.txt
include/
  Algorithms/ThienCoEngine.h
  Audio/SoundManager.h
  Core/Config.h, GameEngine.h
  Entities/GameObject.h, Map.h, Player.h
  Graphics/TextureManager.h
src/
  Algorithms/ThienCoEngine.cpp
  Audio/SoundManager.cpp
  Core/GameEngine.cpp
  Entities/Map.cpp, Player.cpp
  Graphics/TextureManager.cpp
  main.cpp
docs/              # Bài tập kèm theo
build/             # Sinh bởi CMake/Ninja, auto copy assets
```

## AI & chấm điểm
- Khoảng cách giữa các điểm quan trọng (xuất phát + trận nhãn) được tính bằng BFS trên lưới.
- DP bitmask giải TSP để tìm tổng bước tối ưu; số bước người chơi được so với giá trị này để phân hạng S (≤ tối ưu) hoặc A (thắng nhưng chưa tối ưu).

## Troubleshooting
- `SDL.h: No such file or directory`: kiểm tra `C:\msys64\mingw64\include\SDL2\SDL.h`, PATH đã thêm `C:\msys64\mingw64\bin`.
- Thiếu DLL khi chạy: copy SDL2.dll, SDL2_image.dll, SDL2_ttf.dll, SDL2_mixer.dll vào build/ hoặc thêm MinGW64 bin vào PATH.
- Asset/level không tải: chắc chắn chạy exe từ thư mục gốc, assets tồn tại và giữ nguyên cấu trúc.

## License
Dự án phục vụ mục đích học tập tại Đại học Bách Khoa Đà Nẵng.