# Build script for Game Kỳ Môn Thần Tốc
# Biên dịch dự án game với g++ và SDL2

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  GAME KỲ MÔN THẦN TỐC - BUILD SCRIPT  " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Đường dẫn SDL2 (MSYS2 MinGW64)
$SDL_INCLUDE = "C:\msys64\mingw64\include\SDL2"
$SDL_LIB = "C:\msys64\mingw64\lib"

# Kiểm tra SDL2 có tồn tại không
if (!(Test-Path $SDL_INCLUDE)) {
    Write-Host "[LỖI] Không tìm thấy SDL2 headers tại: $SDL_INCLUDE" -ForegroundColor Red
    Write-Host "Vui lòng cài đặt SDL2 thông qua MSYS2:" -ForegroundColor Yellow
    Write-Host "  pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer" -ForegroundColor Yellow
    exit 1
}

Write-Host "[1/3] Kiểm tra môi trường... " -ForegroundColor Green -NoNewline
Write-Host "OK" -ForegroundColor White

# Danh sách file nguồn
$SOURCE_FILES = @(
    "src\GameEngine.cpp",
    "src\main.cpp",
    "src\Map.cpp",
    "src\ParticleSystem.cpp",
    "src\Player.cpp",
    "src\SoundManager.cpp",
    "src\TextureManager.cpp",
    "src\ThienCoEngine.cpp"
)

# Tạo thư mục build nếu chưa có
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Write-Host "[2/3] Biên dịch dự án... " -ForegroundColor Green -NoNewline

# Lệnh biên dịch
$COMPILE_CMD = "g++ -Iinclude -I`"$SDL_INCLUDE`" -L`"$SDL_LIB`" -o build\KyMonThanToc.exe " + ($SOURCE_FILES -join " ") + " -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++17"

# Thực thi
try {
    Invoke-Expression $COMPILE_CMD
    if ($LASTEXITCODE -eq 0) {
        Write-Host "THÀNH CÔNG" -ForegroundColor White
        Write-Host "[3/3] Tạo file thực thi... " -ForegroundColor Green -NoNewline
        Write-Host "build\KyMonThanToc.exe" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  BUILD HOÀN TẤT!                      " -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "Chạy game bằng lệnh: " -ForegroundColor Yellow -NoNewline
        Write-Host ".\build\KyMonThanToc.exe" -ForegroundColor Cyan
    } else {
        Write-Host "THẤT BẠI" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "THẤT BẠI" -ForegroundColor Red
    Write-Host "Lỗi: $_" -ForegroundColor Red
    exit 1
}
