#pragma once
#include "GameObject.h"

// Thêm enum để quản lý trạng thái chuyên nghiệp
enum PlayerState {
    STATE_IDLE,
    STATE_RUN,
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
    
    // --- BIẾN LOGIC DI CHUYỂN ---
    Uint32 m_lastMoveTime; 
    const int MOVE_DELAY = 130; // Move cooldown in milliseconds (controls movement speed)

    // --- BIẾN VISUAL AAA (Giữ lại để mượt mà) ---
    float m_visualX;
    float m_visualY;
    const float SMOOTH_SPEED = 15.0f; // Độ mượt khi nội suy
    float m_scale;                    // Tỷ lệ phóng to nhân vật
    
    // --- BIẾN ANIMATION ---
    int m_animSpeed;
    int m_numFrames;

    // --- QUẢN LÝ TRẠNG THÁI ---
    PlayerState m_currentState;
    PlayerState m_lastState;
    PlayerDirection m_currentDirection;
    PlayerDirection m_lastDirection;
    
    void UpdateAnimationID();
};