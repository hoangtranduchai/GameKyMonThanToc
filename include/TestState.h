#pragma once
#include "IGameState.h" // Phải "tuân thủ" luật chơi của IGameState

class TestState : public IGameState {
public:
    virtual ~TestState() {}

    virtual void Init(GameEngine* engine) override;
    virtual void CleanUp() override;

    virtual void Pause() override;
    virtual void Resume() override;

    virtual void HandleEvents(GameEngine* engine) override;
    virtual void Update(GameEngine* engine) override;
    virtual void Render(GameEngine* engine) override;
};