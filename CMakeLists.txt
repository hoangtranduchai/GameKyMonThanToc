# ==============================================================================
# PROJECT: KY MON THAN TOC (THE MYSTIC LABYRINTH)
# TYPE:    Professional Build Configuration
# DESC:    Hỗ trợ cấu trúc thư mục phân cấp (Modular Directory Structure)
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(KyMonThanToc VERSION 1.0.0 LANGUAGES CXX)

# --- 1. CẤU HÌNH COMPILER ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /MP) # Đa luồng biên dịch
else()
    add_compile_options(-Wall -O3)
endif()

# --- 2. TỰ ĐỘNG TÌM KIẾM MÃ NGUỒN (RECURSIVE SEARCH) ---
# Quét tất cả file .cpp trong src/ và các thư mục con
file(GLOB_RECURSE SOURCES "src/*.cpp")
# Quét tất cả file .h trong include/ và các thư mục con
file(GLOB_RECURSE HEADERS "include/*.h")

# Tạo Source Groups (Để hiển thị đẹp trong Visual Studio theo đúng folder)
foreach(FILE ${SOURCES} ${HEADERS}) 
    # Lấy đường dẫn tương đối
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    
    # Thay thế đường dẫn thực bằng tên Filter ảo
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src" "Source Files" GROUP "${PARENT_DIR}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include" "Header Files" GROUP "${GROUP}")
    
    # Thay dấu / bằng \ cho Windows
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

message(STATUS "Da tim thay " ${SOURCES})

# --- 3. THIẾT LẬP EXECUTABLE ---
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- 4. CẤU HÌNH INCLUDE PATH (QUAN TRỌNG) ---
# Chỉ định thư mục 'include' là gốc. 
# Khi code, ta sẽ gọi: #include "Core/GameEngine.h" thay vì "GameEngine.h"
target_include_directories(${PROJECT_NAME} PRIVATE include)

# --- 5. LIÊN KẾT THƯ VIỆN SDL2 ---
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mingw32 SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer
)

# --- 6. TỰ ĐỘNG COPY ASSETS ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
    COMMENT "Dang dong bo tai nguyen (Assets)..."
)