#pragma once
#include "IGameState.h" // Phải "tuân thủ" luật chơi của IGameState

// Forward declarations để tránh include vòng tròn
class Map; // Báo trước sẽ có 1 lớp Map

class PlayState : public IGameState {
private:
    GameEngine* m_pEngine; // Con trỏ để truy cập Engine (vd: GetRenderer)
    
    // PlayState sẽ "sở hữu" một đối tượng Bản đồ (Map)
    Map* m_pMap;

public:
    virtual ~PlayState() {}

    // Các hàm bắt buộc từ IGameState
    virtual void Init(GameEngine* engine) override;
    virtual void CleanUp() override;

    virtual void Pause() override;
    virtual void Resume() override;

    virtual void HandleEvents(GameEngine* engine) override;
    virtual void Update(GameEngine* engine) override;
    virtual void Render(GameEngine* engine) override;
};