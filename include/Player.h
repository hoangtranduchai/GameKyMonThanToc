#pragma once
#include "GameObject.h"

// Player kế thừa từ GameObject: Sở hữu mọi thuộc tính của một vật thể game
class Player : public GameObject {
public:
    // Constructor sử dụng LoaderParams từ lớp cha
    Player(const LoaderParams* pParams);
    
    virtual ~Player() {}

    // Hiện thực hóa các phương thức thuần ảo
    virtual void Draw();
    virtual void Update();
    virtual void Clean();

    // Hàm teleport dùng cho Undo
    void SetPosition(int x, int y) {
        m_x = x;
        m_y = y;
        // Reset thời gian di chuyển để người chơi có thể đi tiếp ngay lập tức
        m_lastMoveTime = SDL_GetTicks() - MOVE_DELAY; 
    }

private:
    // Hàm xử lý input riêng cho Player
    void HandleInput();
    
    // Di chuyển logic từ GameEngine sang đây
    float m_moveSpeed;

    // Vị trí chính xác (float) để di chuyển mượt mà hơn int
    float m_exactX;
    float m_exactY;

    // --- THÊM MỚI CHO DI CHUYỂN THEO Ô ---
    Uint32 m_lastMoveTime; // Thời điểm di chuyển lần cuối

    // Tốc độ di chuyển: 150ms một bước (số càng nhỏ đi càng nhanh)
    // Bạn có thể chỉnh số này để game nhanh hay chậm tùy ý
    const int MOVE_DELAY = 150;
};