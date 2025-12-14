#pragma once
#include "GameObject.h"

// Thêm enum để quản lý trạng thái chuyên nghiệp
enum PlayerState {
    STATE_IDLE,
    STATE_RUN,
    STATE_ATTACK
};

enum PlayerDirection {
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP
};

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
    void SetPosition(int x, int y);

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

    // --- BIẾN CHO VISUAL AAA ---
    float m_visualX;
    float m_visualY;
    
    // Tốc độ trôi (Interpolation Speed)
    // Giá trị càng lớn trôi càng nhanh. 10.0f - 15.0f là đẹp.
    const float SMOOTH_SPEED = 12.0f; 
    
    // Biến Animation
    int m_animSpeed;   // Tốc độ chuyển frame
    int m_numFrames;   // Tổng số frame trong sprite sheet

    // --- BIẾN QUẢN LÝ ANIMATION AAA ---
    PlayerState m_currentState;
    PlayerDirection m_currentDirection;

    // Thêm biến để theo dõi trạng thái cũ -> Reset frame khi đổi hành động
    PlayerState m_lastState; 
    PlayerDirection m_lastDirection;
    
    // Hàm cập nhật Texture ID dựa trên Trạng thái & Hướng
    void UpdateAnimationID();

    float m_scale; // Tỷ lệ phóng to (ví dụ: 1.5, 2.0)
};