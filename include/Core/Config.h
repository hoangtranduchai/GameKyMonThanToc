#pragma once

/* Sử dụng constexpr (Biểu thức hằng số) thay vì #define hay const là một kỹ thuật tối ưu hóa hiện đại của C++
giúp trình biên dịch tính toán giá trị ngay tại thời điểm biên dịch (Compile time)
giảm tải cho CPU khi chạy game (Runtime) */

namespace Config {

    // 1. THIẾT LẬP HIỂN THỊ & CỬA SỔ (DISPLAY & WINDOW SETTINGS)
    
    // Tiêu đề hiển thị trên thanh tiêu đề cửa sổ
    static const char* const WINDOW_TITLE = "Ky Mon Than Toc";

    // Chiều rộng màn hình thiết kế (Độ phân giải logic)
    // Game sẽ luôn kết xuất ở độ phân giải này, sau đó co giãn lên màn hình thực tế
    static constexpr int SCREEN_WIDTH = 1280;
    
    // Chiều cao màn hình thiết kế (Độ phân giải logic)
    static constexpr int SCREEN_HEIGHT = 720;

    // Chế độ toàn màn hình mặc định khi khởi động (true/false)
    static constexpr bool FULLSCREEN_ON_START = true;

    // Tỷ lệ phóng kết xuất (chỉ ảnh hưởng hiển thị, không đổi logic)
    // Ví dụ: 1.0 = nguyên gốc, 1.5 = phóng 150%, 2.0 = phóng 200%
    static constexpr float RENDER_SCALE = 2.0f;

    // 2. THIẾT LẬP HIỆU SUẤT & THỜI GIAN (PERFORMANCE & TIMING)

    // Tốc độ khung hình mục tiêu (Frames Per Second)
    static constexpr int TARGET_FPS = 60;
    
    // Thời gian tối đa cho phép của một khung hình (tính bằng mili giây)
    // Công thức: 1000ms / 60fps ≈ 16.66ms
    static constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    // 3. THIẾT LẬP BẢN ĐỒ & LƯỚI (MAP & GRID SETTINGS)

    // Kích thước một ô gạch (Tile Size) tính bằng pixel
    static constexpr int TILE_SIZE = 64;

    // Khoảng trống phía dưới bản đồ (pixel) để chừa chỗ cho HUD/tooltip
    static constexpr int MAP_BOTTOM_MARGIN = 36;

    // Kích thước nhân vật (Collision Box - Hộp va chạm)
    static constexpr int PLAYER_WIDTH = 96;
    static constexpr int PLAYER_HEIGHT = 80;

    // 4. THIẾT LẬP LOGIC TRÒ CHƠI (GAMEPLAY MECHANICS)

    // Tốc độ di chuyển của nhân vật (Pixel per second - Pixel mỗi giây)
    // Giá trị này nhân với DeltaTime để ra quãng đường di chuyển
    // Điều chỉnh để vẫn ~2.5 ô/giây với TILE_SIZE mới (2.5 * 80 = 200)
    static constexpr float PLAYER_SPEED = 200.0f;

    // Các định danh loại ô trên bản đồ (Tile IDs)
    // Đồng bộ với file bản đồ .txt và logic xử lý
    namespace TileID {
        static constexpr int GROUND = 0; // Đường đi / Đã ăn
        static constexpr int AIR   = 1;  // Chân không (Gây thua)
        static constexpr int SHRINE = 2; // Trận Nhãn (Mục tiêu cần thu thập)
        static constexpr int START  = 0; // Vị trí bắt đầu
    }

    // 5. ĐƯỜNG DẪN TÀI NGUYÊN (RESOURCE PATHS)
    
    // Đường dẫn gốc
    static const char* const ASSET_DIR = "assets/";
    
    // Đường dẫn chi tiết
    static const char* const LEVEL_DIR = "assets/levels/";
    static const char* const TEXTURE_DIR = "assets/images/";
    static const char* const AUDIO_DIR = "assets/audio/";
    static const char* const FONT_PATH = "assets/fonts/Roboto-Regular.ttf";
}