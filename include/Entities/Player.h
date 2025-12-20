#pragma once

#include "Entities/GameObject.h"
#include "Core/Config.h"

// ENUM: ĐỊNH HƯỚNG DI CHUYỂN
enum class PlayerDirection {
    DOWN  = 0,
    UP    = 1,
    LEFT  = 2,
    RIGHT = 3
};

// CLASS: Player
class Player : public GameObject {
public:
    // Hàm khởi tạo sử dụng LoaderParams từ lớp cha
    Player(const LoaderParams* pParams);
    
    ~Player() override;

    // Vẽ nhân vật lên màn hình
    void Draw() override;

    // Cập nhật logic nhân vật (Input -> Move -> Animation)
    void Update() override;

    // Dọn dẹp tài nguyên (nếu có)
    void Clean() override;

    // Thiết lập lại vị trí nhân vật
    void SetPosition(float x, float y);

private:
    // Hàm xử lý đầu vào bàn phím
    void HandleInput();
    
    // Cập nhật hoạt ảnh
    void UpdateAnimation();
    
    // Xử lý di chuyển vật lý
    void HandleMovement(float dt);

    // Xử lý tương tác với ô gạch (tiles)
    void ProcessTileInteraction();

    // Hiệu ứng rơi xuống vực
    void Falling();

    // Vận tốc (Pixel/Giây)
    float m_velX, m_velY;

    // Trạng thái hướng hiện tại
    PlayerDirection m_currentDir;

    // Lưu hướng cuối cùng khi dừng lại (để hiển thị Idle đúng hướng)
    PlayerDirection m_lastDir;

    // Cờ đánh dấu đang di chuyển hay đứng yên
    bool m_isMoving;

    // Di chuyển theo từng ô: trạng thái và đích đến
    bool m_isMovingToTile;
    float m_targetX;
    float m_targetY;

    // Cờ đánh dấu đang trong quá trình rơi
    bool m_isFalling;

    // Thời gian đã trôi qua kể từ lúc bắt đầu rơi
    float m_fallTimer;

    // Tốc độ rơi (Co giãn nhỏ dần hoặc tụt xuống theo trục Y)
    const float FALL_DURATION = 1.5f; // Rơi trong 1.5 giây thì chuyển màn hình thua
    
    // Tọa độ lúc bắt đầu rơi
    float m_fallStartY;

    // Hàm kiểm tra xem game đã thắng chưa (từ GameEngine)
    bool IsGameWon() const;
};