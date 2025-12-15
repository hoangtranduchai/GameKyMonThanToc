# CHUẨN BỊ KỸ THUẬT (TECHNICAL SETUP)
## Game Kỳ Môn Thần Tốc - Development Environment

**Document Version:** 1.0  
**Last Updated:** December 16, 2025  
**Status:** ✅ Complete & Documented

---

## 📋 MỤC LỤC

1. [Chọn Game Engine](#1-chọn-game-engine)
2. [Cài Đặt Môi Trường](#2-cài-đặt-môi-trường-lập-trình)
3. [Thiết Lập Git/GitHub](#3-thiết-lập-gitgithub)
4. [Quy Ước Đặt Tên](#4-quy-ước-đặt-tên-file-naming-convention)
5. [Build System](#5-build-system)
6. [Testing Setup](#6-testing-setup)

---

## 1. CHỌN GAME ENGINE

### 1.1. Quyết Định Cuối Cùng

**Engine/Framework đã chọn:** ✅ **SDL2 (Simple DirectMedia Layer 2)**

### 1.2. Lý Do Lựa Chọn

#### **Ưu điểm SDL2:**

✅ **Cross-platform:**
- Windows, Linux, macOS, iOS, Android
- Web (via Emscripten)
- Đảm bảo tính mở rộng trong tương lai

✅ **Lightweight:**
- Không có engine overhead
- Chỉ load những gì cần thiết
- Performance cao cho 2D games

✅ **Learning-friendly:**
- C/C++ thuần túy
- API đơn giản, dễ hiểu
- Tài liệu phong phú

✅ **Full control:**
- Kiểm soát hoàn toàn rendering pipeline
- Không bị giới hạn bởi engine features
- Hiểu rõ "under the hood"

✅ **Open source & Free:**
- Zlib license (rất tự do)
- Không phí license
- Community support mạnh

#### **Nhược điểm (đã cân nhắc):**

⚠️ **Lower level:**
- Phải tự implement game systems
- Không có built-in editor
- **Solution:** Đây là điểm mạnh cho mục đích học tập

⚠️ **No visual editor:**
- Level design bằng text files
- **Solution:** Phù hợp với dự án nhỏ, có thể build editor sau

⚠️ **Manual memory management:**
- Phải tự quản lý resources
- **Solution:** Implement resource managers (TextureManager, SoundManager)

---

### 1.3. So Sánh Với Các Engine Khác

| Feature | SDL2 | Unity | Godot | Unreal |
|---------|------|-------|-------|--------|
| **Cost** | Free | Free* | Free | Free* |
| **Language** | C/C++ | C# | GDScript | C++ |
| **2D Support** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Learning Curve** | Medium | Easy | Easy | Hard |
| **Control Level** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **Performance** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **For Puzzle Game** | ✅ Perfect | ✅ Good | ✅ Good | ❌ Overkill |

**Why NOT Unity:**
- Overkill cho puzzle game đơn giản
- C# không phù hợp với môn học C++
- License restrictions cho educational projects

**Why NOT Godot:**
- GDScript là ngôn ngữ mới cần học
- Muốn focus vào thuật toán hơn là engine features

**Why NOT Unreal:**
- Quá phức tạp và nặng
- Blueprint system không phù hợp
- C++ của Unreal khác với standard C++

---

### 1.4. SDL2 Components Used

**Core Libraries:**
```cpp
#include <SDL.h>           // Main SDL library
#include <SDL_image.h>     // Image loading (PNG, JPG)
#include <SDL_ttf.h>        // TrueType font rendering
#include <SDL_mixer.h>      // Audio playback (MP3, WAV)
```

**Purpose:**
- `SDL.h` → Window, Renderer, Input, Events
- `SDL_image.h` → Texture loading from files
- `SDL_ttf.h` → Text rendering with fonts
- `SDL_mixer.h` → Music and sound effects

---

## 2. CÀI ĐẶT MÔI TRƯỜNG LẬP TRÌNH

### 2.1. Prerequisites

#### **Operating System:**
- ✅ Windows 10/11 64-bit (Primary target)
- ⚠️ Linux (Supported but not tested)
- ⚠️ macOS (Supported but not tested)

#### **Required Tools:**
1. **MSYS2** (Windows package manager)
2. **MinGW-w64 GCC** (C++ compiler)
3. **SDL2 libraries** (Graphics, Audio, Font)
4. **CMake** (Optional - for advanced builds)
5. **Git** (Version control)
6. **VS Code** (IDE - recommended)

---

### 2.2. Installation Guide (Windows)

#### **Step 1: Install MSYS2**

```powershell
# Download from: https://www.msys2.org/
# Run installer: msys2-x86_64-xxxxxxxx.exe
# Installation path: C:\msys64 (recommended)
```

**Post-installation:**
```bash
# Open MSYS2 terminal
# Update package database
pacman -Syu

# Update again (if prompted to restart)
pacman -Su
```

---

#### **Step 2: Install Build Tools**

```bash
# GCC Compiler
pacman -S mingw-w64-x86_64-gcc

# CMake (optional but recommended)
pacman -S mingw-w64-x86_64-cmake

# Ninja build system (optional)
pacman -S mingw-w64-x86_64-ninja

# Make utility
pacman -S mingw-w64-x86_64-make
```

**Verify installation:**
```bash
gcc --version
# Should show: gcc.exe (Rev8, Built by MSYS2 project) 15.2.0
```

---

#### **Step 3: Install SDL2 Libraries**

```bash
# Core SDL2
pacman -S mingw-w64-x86_64-SDL2

# Image support (PNG, JPG)
pacman -S mingw-w64-x86_64-SDL2_image

# Font support (TrueType)
pacman -S mingw-w64-x86_64-SDL2_ttf

# Audio support (MP3, WAV, OGG)
pacman -S mingw-w64-x86_64-SDL2_mixer
```

**Verify SDL2:**
```bash
# Check if headers exist
ls /mingw64/include/SDL2/

# Should show: SDL.h, SDL_image.h, SDL_ttf.h, SDL_mixer.h
```

---

#### **Step 4: Configure Environment Variables**

**Add to Windows PATH:**
```
C:\msys64\mingw64\bin
```

**How to add:**
1. Search "Environment Variables" in Windows
2. Edit "Path" variable
3. Add new entry: `C:\msys64\mingw64\bin`
4. Click OK
5. Restart terminal/IDE

**Verify PATH:**
```powershell
where gcc
# Should show: C:\msys64\mingw64\bin\gcc.exe
```

---

#### **Step 5: Install Visual Studio Code**

**Download:**
- https://code.visualstudio.com/

**Recommended Extensions:**
```
1. C/C++ (Microsoft)
2. C/C++ Extension Pack
3. CMake Tools
4. GitLens
5. Code Spell Checker
```

**Install via VS Code:**
1. Open VS Code
2. Press `Ctrl+Shift+X`
3. Search for each extension
4. Click "Install"

---

### 2.3. IDE Configuration

#### **VS Code Settings (`.vscode/c_cpp_properties.json`)**

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include",
                "C:/msys64/mingw64/include/SDL2"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

**Benefits:**
- IntelliSense autocomplete
- Error detection
- Go to definition
- Code navigation

---

#### **VS Code Launch Configuration (`.vscode/launch.json`)**

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Game",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/KyMonThanToc.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "Build Game"
        }
    ]
}
```

**Usage:**
- Press `F5` to build & run with debugger
- Set breakpoints by clicking line numbers
- Inspect variables in debug panel

---

### 2.4. Alternative IDEs

**Visual Studio 2022 (Full IDE):**
- ✅ Powerful debugger
- ✅ Built-in profiler
- ❌ Heavy (several GB)
- ❌ Overkill for simple projects

**CLion (JetBrains):**
- ✅ Excellent CMake integration
- ✅ Smart refactoring
- ❌ Paid (free for students)

**Code::Blocks:**
- ✅ Lightweight
- ✅ Cross-platform
- ❌ Outdated UI
- ❌ Less features

**Recommendation:** Stick with **VS Code** for balance of features and simplicity.

---

## 3. THIẾT LẬP GIT/GITHUB

### 3.1. Why Version Control?

**Critical Benefits:**
- ✅ **Backup:** Never lose code
- ✅ **History:** See what changed when
- ✅ **Collaboration:** Work with team
- ✅ **Branching:** Experiment safely
- ✅ **Portfolio:** Show your work

**Horror Story Without Git:**
```
MyProject/
├── game_v1.cpp
├── game_v2.cpp
├── game_v3_final.cpp
├── game_v3_final_REAL.cpp
├── game_v3_final_REAL_USE_THIS.cpp
└── game_v3_final_REAL_USE_THIS_FIXED.cpp  ← Which one???
```

---

### 3.2. Git Installation

**Windows:**
```powershell
# Via MSYS2 (recommended)
pacman -S git

# Or download from: https://git-scm.com/
```

**Verify:**
```bash
git --version
# Should show: git version 2.43.0 or newer
```

---

### 3.3. Git Configuration

**Set up identity:**
```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

**Set default editor (VS Code):**
```bash
git config --global core.editor "code --wait"
```

**Enable colors:**
```bash
git config --global color.ui auto
```

**Check config:**
```bash
git config --list
```

---

### 3.4. GitHub Setup

#### **Create GitHub Account:**
1. Go to https://github.com/
2. Sign up (free account)
3. Verify email

#### **Generate SSH Key (Optional but recommended):**
```bash
# Generate new SSH key
ssh-keygen -t ed25519 -C "your.email@example.com"

# Start SSH agent
eval "$(ssh-agent -s)"

# Add key to agent
ssh-add ~/.ssh/id_ed25519

# Copy public key
cat ~/.ssh/id_ed25519.pub
# Paste this into GitHub Settings → SSH Keys
```

---

### 3.5. Repository Structure

**Initialize Git:**
```bash
cd GameKyMonThanToc
git init
```

**Create `.gitignore`:**
```gitignore
# Build artifacts
build/
*.exe
*.o
*.obj
*.dll
*.so
*.dylib

# IDE files
.vscode/
.vs/
*.user
*.suo

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Temporary files
*.tmp
*.log
*.swp
*~

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake

# Keep these folders but ignore contents
build/*
!build/.gitkeep
```

---

### 3.6. Git Workflow

**Daily Workflow:**
```bash
# 1. Check status
git status

# 2. Stage changes
git add .

# 3. Commit with message
git commit -m "Add player movement system"

# 4. Push to GitHub
git push origin main
```

**Branch Strategy (Future):**
```bash
# Create feature branch
git checkout -b feature/level-editor

# Work on feature...
# Commit changes...

# Merge back to main
git checkout main
git merge feature/level-editor
```

---

### 3.7. Commit Message Convention

**Format:**
```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Code style (no logic change)
- `refactor`: Code refactoring
- `test`: Add tests
- `chore`: Maintenance tasks

**Examples:**
```bash
git commit -m "feat(player): add smooth movement interpolation"
git commit -m "fix(ai): correct TSP traceback algorithm"
git commit -m "docs(readme): update build instructions"
git commit -m "refactor(engine): extract state machine logic"
```

---

### 3.8. Project on GitHub

**Repository URL (Example):**
```
https://github.com/yourusername/GameKyMonThanToc
```

**README.md should include:**
- ✅ Project description
- ✅ Screenshots/GIFs
- ✅ Build instructions
- ✅ Controls guide
- ✅ License information

**Tags/Releases:**
```bash
# Tag a version
git tag -a v1.0 -m "First release"
git push origin v1.0
```

---

## 4. QUY ƯỚC ĐẶT TÊN FILE (NAMING CONVENTION)

### 4.1. Mục Đích

**Why naming conventions matter:**
- ✅ **Consistency:** Easy to find files
- ✅ **Readability:** Understand purpose at a glance
- ✅ **Scalability:** Works as project grows
- ✅ **Collaboration:** Team understands structure

---

### 4.2. File Structure Rules

#### **Directory Naming:**
```
✅ GOOD:
/assets/audio/
/include/
/src/
/build/
/docs/

❌ BAD:
/Audio_Files/
/headers/
/source code/
/Build Output/
```

**Rules:**
- Lowercase
- Underscore for spaces (if needed)
- Descriptive names
- Plural for collections

---

#### **C++ Files:**

**Header Files (`.h`):**
```cpp
✅ GOOD:
GameEngine.h         // PascalCase for classes
Player.h
TextureManager.h
ThienCoEngine.h

❌ BAD:
gameengine.h         // Hard to read
game_engine.h        // Inconsistent style
GAMEENGINE.H         // Too shouty
```

**Source Files (`.cpp`):**
```cpp
✅ GOOD:
GameEngine.cpp       // Matches header name
Player.cpp
main.cpp             // Exception: entry point

❌ BAD:
GameEngine.cc        // Use .cpp consistently
game-engine.cpp      // Different from header
```

**Rules:**
- **PascalCase** for class files
- **lowercase** for utility files (main.cpp, config.cpp)
- Header and source must match exactly
- One class per file (generally)

---

#### **Asset Files:**

**Images:**
```
✅ GOOD:
background.png
player_idle_down.png
tiles.png
shrine_icon.png

❌ BAD:
Background_Final_V3.PNG
player-sprite-sheet.png
image001.png
```

**Rules:**
- **lowercase**
- **snake_case** for multi-word
- Descriptive names
- Consistent file extensions

**Audio:**
```
✅ GOOD:
bgm.mp3
step.wav
collect.wav
win_sound.wav

❌ BAD:
BackgroundMusic.mp3
sound_effect_1.wav
audio.wav
```

**Fonts:**
```
✅ GOOD:
Roboto-Regular.ttf
Roboto-Bold.ttf

❌ BAD:
font.ttf
my_font_file.TTF
```

---

#### **Level Files:**
```
✅ GOOD:
level1.txt
level2.txt
level3.txt

Or (with names):
level1_tutorial.txt
level2_intermediate.txt
level3_master.txt

❌ BAD:
Level 1.txt          // Space problematic
lv_01.txt            // Inconsistent format
map_easy.txt         // Use 'level' consistently
```

---

### 4.3. Code Naming Conventions

#### **Variables:**

```cpp
// Local variables: camelCase
int playerHealth;
float deltaTime;
bool isRunning;

// Member variables: m_camelCase (with prefix)
int m_x;
int m_y;
float m_deltaTime;
bool m_bRunning;

// Constants: UPPER_SNAKE_CASE
const int MAX_PLAYERS = 4;
const float PI = 3.14159f;
const int TILE_SIZE = 64;

// Global variables: g_camelCase (avoid if possible)
SDL_Renderer* g_renderer;  // Better: use Singleton pattern
```

#### **Functions:**

```cpp
// Functions: PascalCase
void UpdatePlayer();
void RenderFrame();
int CalculateDistance();

// Getters: GetXxx()
int GetWidth() const;
bool IsRunning() const;  // Boolean: Is/Has/Can prefix

// Setters: SetXxx()
void SetPosition(int x, int y);
void SetRunning(bool running);
```

#### **Classes:**

```cpp
// Classes: PascalCase
class GameEngine { };
class Player { };
class TextureManager { };

// Interfaces: IPascalCase (if using)
class IGameObject { };
class IRenderable { };

// Structs: PascalCase (same as classes)
struct LoaderParams { };
struct MapPoint { };
```

#### **Enums:**

```cpp
// Enum: PascalCase
// Values: UPPER_SNAKE_CASE
enum GameState {
    STATE_MENU,
    STATE_PLAY,
    STATE_PAUSE,
    STATE_WIN
};

enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};
```

---

### 4.4. Directory Structure Convention

**Adopted Structure:**
```
GameKyMonThanToc/
├── assets/              # Game resources
│   ├── audio/           # Sound files
│   │   ├── bgm.mp3
│   │   ├── step.wav
│   │   └── collect.wav
│   ├── fonts/           # Font files
│   │   └── Roboto-Regular.ttf
│   ├── images/          # Image files
│   │   ├── player/      # Player sprites
│   │   │   ├── idle_down.png
│   │   │   └── run_up.png
│   │   ├── background.png
│   │   └── tiles.png
│   └── levels/          # Level data
│       ├── level1.txt
│       └── level2.txt
├── include/             # Header files
│   ├── GameEngine.h
│   ├── Player.h
│   └── Map.h
├── src/                 # Source files
│   ├── GameEngine.cpp
│   ├── Player.cpp
│   └── main.cpp
├── build/               # Build output (ignored by git)
├── docs/                # Documentation
│   ├── GDD.md
│   └── PREPRODUCTION.md
├── tests/               # Unit tests (future)
├── CMakeLists.txt       # Build configuration
├── build.bat            # Build script
├── .gitignore           # Git ignore rules
└── README.md            # Project overview
```

---

### 4.5. Hungarian Notation (Limited Use)

**Member Variables:**
```cpp
// Prefix 'm_' for member variables
class Player {
private:
    int m_x;              // Member variable
    int m_y;
    float m_speed;
    bool m_bIsAlive;      // 'b' for boolean (optional)
};
```

**Pointers:**
```cpp
// Prefix 'p' for pointers (optional)
SDL_Window* m_pWindow;
SDL_Renderer* m_pRenderer;
Player* m_pPlayer;

// Or without prefix (modern C++ style)
SDL_Window* window;
std::unique_ptr<Player> player;
```

**Static Variables:**
```cpp
// Prefix 's_' for static
class GameEngine {
private:
    static GameEngine* s_Instance;  // Singleton instance
};
```

---

### 4.6. Comments Convention

**File Headers:**
```cpp
/**
 * @file GameEngine.h
 * @brief Core game engine implementation
 * @author Your Name
 * @date 2025-12-15
 */
```

**Class Comments:**
```cpp
/**
 * @class GameEngine
 * @brief Main game engine managing all subsystems
 * 
 * Singleton pattern. Handles initialization, game loop,
 * and cleanup of all game resources.
 */
class GameEngine {
    // ...
};
```

**Function Comments:**
```cpp
/**
 * @brief Initialize game engine and all subsystems
 * @param title Window title
 * @param x Window X position
 * @param y Window Y position
 * @param w Window width
 * @param h Window height
 * @param fullscreen Fullscreen flag
 * @return true if successful, false otherwise
 */
bool Init(const char* title, int x, int y, int w, int h, bool fullscreen);
```

---

## 5. BUILD SYSTEM

### 5.1. CMake Configuration

**CMakeLists.txt (Root):**
```cmake
cmake_minimum_required(VERSION 3.10)
project(KyMonThanToc VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Define PROJECT_ROOT_PATH (now in Config.h)
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    PROJECT_ROOT_PATH="${PROJECT_SOURCE_DIR}"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    SDL2_image
    SDL2_ttf
    SDL2_mixer
)
```

---

### 5.2. Build Scripts

**build.bat (Windows Batch):**
```batch
@echo off
echo Building Game Ky Mon Than Toc...

if not exist "build" mkdir build

g++ -Iinclude -IC:\msys64\mingw64\include\SDL2 ^
    -LC:\msys64\mingw64\lib ^
    -o build\KyMonThanToc.exe ^
    src\*.cpp ^
    -lmingw32 -lSDL2main -lSDL2 ^
    -lSDL2_image -lSDL2_ttf -lSDL2_mixer ^
    -std=c++17

if %errorlevel% == 0 (
    echo Build successful!
    echo Run: .\build\KyMonThanToc.exe
) else (
    echo Build failed!
)
```

**build.ps1 (PowerShell):**
```powershell
Write-Host "Building Game Ky Mon Than Toc..." -ForegroundColor Cyan

if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

$result = g++ -Iinclude -IC:\msys64\mingw64\include\SDL2 `
    -LC:\msys64\mingw64\lib `
    -o build\KyMonThanToc.exe `
    src\*.cpp `
    -lmingw32 -lSDL2main -lSDL2 `
    -lSDL2_image -lSDL2_ttf -lSDL2_mixer `
    -std=c++17

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
} else {
    Write-Host "Build failed!" -ForegroundColor Red
}
```

---

### 5.3. Build Commands

**Manual Compilation:**
```bash
# From project root
g++ -Iinclude \
    -IC:\msys64\mingw64\include\SDL2 \
    -LC:\msys64\mingw64\lib \
    -o build/KyMonThanToc.exe \
    src/*.cpp \
    -lmingw32 -lSDL2main -lSDL2 \
    -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
    -std=c++17
```

**Using Build Scripts:**
```bash
# Windows Batch
.\build.bat

# PowerShell
.\build.ps1

# Linux/macOS
./build.sh  # (if created)
```

**Using CMake:**
```bash
mkdir build
cd build
cmake .. -G Ninja
ninja
```

---

## 6. TESTING SETUP

### 6.1. Manual Testing Checklist

**Compilation Test:**
- [ ] Code compiles without errors
- [ ] No warnings (or acceptable warnings documented)
- [ ] Executable created in build/ folder

**Runtime Test:**
- [ ] Game launches without crash
- [ ] Main menu displays correctly
- [ ] All assets load (textures, audio, fonts)
- [ ] Input responsive (WASD, ESC, U)

**Gameplay Test:**
- [ ] Player movement works in all 4 directions
- [ ] Collision detection (mountains block movement)
- [ ] Shrine collection triggers effects
- [ ] Win condition triggers correctly
- [ ] Rank calculation accurate

**AI Test:**
- [ ] BFS calculates distances correctly
- [ ] TSP finds optimal path
- [ ] Optimal steps displayed accurately
- [ ] Console output shows calculations

---

### 6.2. Automated Testing (Future)

**Unit Test Framework (Catch2):**
```cpp
// Example: tests/test_map.cpp
#include <catch2/catch.hpp>
#include "Map.h"

TEST_CASE("Map loads correctly", "[map]") {
    Map map;
    map.LoadMap("assets/levels/level1.txt");
    
    REQUIRE(map.GetRows() == 3);
    REQUIRE(map.GetCols() == 6);
    REQUIRE(map.GetShrines().size() == 3);
}

TEST_CASE("Tile collision works", "[map]") {
    Map map;
    map.LoadMap("assets/levels/level1.txt");
    
    // Mountain tile should return 1
    REQUIRE(map.GetTileID(0, 4) == 1);
    
    // Ground tile should return 0
    REQUIRE(map.GetTileID(0, 0) == 0);
}
```

---

### 6.3. Performance Testing

**FPS Counter (Add to HUD):**
```cpp
// In GameEngine::Update()
static int frameCount = 0;
static float fpsTimer = 0.0f;
static float currentFPS = 0.0f;

frameCount++;
fpsTimer += m_deltaTime;

if (fpsTimer >= 1.0f) {
    currentFPS = frameCount / fpsTimer;
    frameCount = 0;
    fpsTimer = 0.0f;
}

// Display in HUD
DrawText("FPS: " + std::to_string((int)currentFPS), 10, 10);
```

**Memory Profiling:**
```cpp
// Use Valgrind (Linux) or Visual Studio Memory Profiler (Windows)
// Check for memory leaks
```

---

## 7. CONCLUSION

**Technical Setup Status:** ✅ **COMPLETE**

**What We Have:**
- ✅ SDL2 framework properly configured
- ✅ VS Code development environment ready
- ✅ Git/GitHub for version control
- ✅ Consistent naming conventions documented
- ✅ Build system with multiple options
- ✅ Testing framework outlined

**Next Steps:**
- Keep code organized following conventions
- Commit regularly with good messages
- Test after each major feature
- Document as you go

**Resources:**
- SDL2 Wiki: https://wiki.libsdl.org/
- Learn Git: https://git-scm.com/book/en/v2
- C++ Reference: https://en.cppreference.com/

---

**End of Technical Setup Document**

*"Code is poetry. But without proper tools, even the best poet cannot write."*
