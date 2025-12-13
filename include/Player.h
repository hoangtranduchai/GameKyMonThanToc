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

private:
    // Hàm xử lý input riêng cho Player
    void HandleInput();
    
    // Di chuyển logic từ GameEngine sang đây
    float m_moveSpeed;

    // Vị trí chính xác (float) để di chuyển mượt mà hơn int
    float m_exactX;
    float m_exactY;
};