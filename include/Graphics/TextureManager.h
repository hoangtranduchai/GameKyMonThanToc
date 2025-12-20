#pragma once

// THƯ VIỆN HỆ THỐNG
#include <string>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>

// MẪU THIẾT KẾ SINGLETON

class TextureManager {
public:
    // Lấy thể hiện duy nhất của TextureManager
    static TextureManager* GetInstance();

    // Hủy và giải phóng instance khi tắt game
    static void DestroyInstance();

    // 2. QUẢN LÝ TÀI NGUYÊN (RESOURCE MANAGEMENT)

    // Tải hình ảnh (Load image/texture)
    bool Load(const std::string& fileName, const std::string& id, SDL_Renderer* pRenderer);

    // Tải phông chữ (Load font)
    bool LoadFont(const std::string& fileName, const std::string& id, int fontSize);

    // Xóa một texture cụ thể khỏi bộ nhớ
    void ClearTexture(const std::string& id);

    // Xóa toàn bộ tài nguyên (Texture & Font)
    void Clean();

    // 3. KẾT XUẤT ĐỒ HỌA (RENDERING)

    // Vẽ toàn bộ Texture lên màn hình (Dùng cho hình nền)
    void Draw(const std::string& id, int x, int y, int width, int height, SDL_Renderer* pRenderer);

    // Vẽ một phần của Texture (Dùng cho Animation/Spritesheet)
    void DrawFrame(const std::string& id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer);

    // Vẽ một phần của Texture với tỷ lệ phóng (chỉ phóng kích thước vẽ, không thay đổi kích thước cắt nguồn)
    void DrawFrameScaled(const std::string& id, int x, int y, int srcWidth, int srcHeight, float scale, int currentRow, int currentFrame, SDL_Renderer* pRenderer);

    // 4. Hàm truy xuất Texture (Getter)
    SDL_Texture* GetTexture(const std::string& id) { return m_textureMap[id]; }

private:
    // Hàm khởi tạo riêng tư (Private Constructor - Singleton)
    TextureManager();
    ~TextureManager();

    // Thể hiện duy nhất (Instance)
    static TextureManager* s_Instance;

    // Cấu trúc Map lưu trữ Texture: Khóa (Key) = Tên ID chuỗi, Giá trị (Value) = Con trỏ SDL_Texture*
    std::map<std::string, SDL_Texture*> m_textureMap;

    // Cấu trúc Map lưu trữ Font: Khóa = Tên ID, Giá trị = Con trỏ TTF_Font*
    std::map<std::string, TTF_Font*> m_fontMap;
};