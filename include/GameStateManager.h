#pragma once

#include <vector> // Tạm thời dùng vector làm Stack
                  // Tuần 11 chúng ta sẽ thay bằng Stack tự cài đặt

// Chúng ta cần biết IGameState là gì
#include "IGameState.h"

// Forward declare GameEngine
class GameEngine;

class GameStateManager {
private:
    GameEngine* m_pEngine; // Con trỏ để biết đang chạy trên Engine nào
    
    // Một "chồng" (Stack) các trạng thái.
    // Trạng thái ở cuối vector (back) là trạng thái đang chạy.
    std::vector<IGameState*> m_States;

public:
    GameStateManager();
    ~GameStateManager();

    void Init(GameEngine* engine); // Khởi tạo, gán con trỏ engine

    void CleanUp(); // Dọn dẹp tất cả các trạng thái còn lại

    // Các hàm này sẽ gọi hàm tương ứng của trạng thái ở trên cùng
    void HandleEvents();
    void Update();
    void Render();

    // ----- Các hàm quan trọng nhất -----

    // Hàm này dùng để CHUYỂN hẳn sang trạng thái mới
    // (VD: từ Menu -> Play)
    // Nó sẽ DỌN DẸP trạng thái cũ và KHỞI TẠO trạng thái mới
    void ChangeState(IGameState* state);

    // Hàm này dùng để ĐẨY một trạng thái mới lên trên
    // (VD: từ Play -> Pause)
    // Nó chỉ TẠM DỪNG (Pause) trạng thái cũ
    void PushState(IGameState* state);

    // Hàm này dùng để QUAY LẠI trạng thái trước đó
    // (VD: từ Pause -> Play)
    // Nó DỌN DẸP trạng thái hiện tại và QUAY LẠI (Resume) trạng thái cũ
    void PopState();
};