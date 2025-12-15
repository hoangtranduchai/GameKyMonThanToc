@echo off
REM Build script for Game Ky Mon Than Toc
REM Bien dich du an game voi g++ va SDL2

echo ========================================
echo   GAME KY MON THAN TOC - BUILD SCRIPT
echo ========================================
echo.

REM Duong dan SDL2 (MSYS2 MinGW64)
set SDL_INCLUDE=C:\msys64\mingw64\include\SDL2
set SDL_LIB=C:\msys64\mingw64\lib

REM Kiem tra SDL2 co ton tai khong
if not exist "%SDL_INCLUDE%" (
    echo [LOI] Khong tim thay SDL2 headers tai: %SDL_INCLUDE%
    echo Vui long cai dat SDL2 thong qua MSYS2:
    echo   pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
    exit /b 1
)

echo [1/3] Kiem tra moi truong... OK

REM Tao thu muc build neu chua co
if not exist "build" mkdir build

echo [2/3] Bien dich du an...

REM Lenh bien dich
g++ -Iinclude -I"%SDL_INCLUDE%" -L"%SDL_LIB%" -o build\KyMonThanToc.exe src\GameEngine.cpp src\main.cpp src\Map.cpp src\ParticleSystem.cpp src\Player.cpp src\SoundManager.cpp src\TextureManager.cpp src\ThienCoEngine.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++17

if errorlevel 1 (
    echo THAT BAI
    exit /b 1
)

echo THANH CONG
echo [3/3] Tao file thuc thi... build\KyMonThanToc.exe
echo.
echo ========================================
echo   BUILD HOAN TAT!
echo ========================================
echo.
echo Chay game bang lenh: .\build\KyMonThanToc.exe
