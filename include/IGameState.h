#pragma once

// Forward declare GameEngine để tránh include vòng tròn
class GameEngine;

// Đây là một LỚP TRỪU TƯỢNG (Abstract Class) hay còn gọi là Interface
// Nó định nghĩa "luật chơi" cho tất cả các trạng thái (Menu, Play, Pause)
// Bất kỳ lớp nào kế thừa từ IGameState BẮT BUỘC phải cung cấp code
// cho các hàm "virtual ... = 0" (pure virtual functions).
class IGameState {
public:
    // Hàm hủy ảo (Virtual Destructor) là bắt buộc khi dùng kế thừa
    virtual ~IGameState() {}

    // Hàm này được gọi KHI BƯỚC VÀO trạng thái
    virtual void Init(GameEngine* engine) = 0;

    // Hàm này được gọi KHI RỜI KHỎI trạng thái
    virtual void CleanUp() = 0;

    // Hàm này được gọi khi trạng thái bị tạm dừng (ví dụ: vào Pause)
    virtual void Pause() = 0;
    
    // Hàm này được gọi khi quay lại từ trạng thái tạm dừng
    virtual void Resume() = 0;

    // Các hàm vòng lặp game, được gọi bởi GameEngine
    virtual void HandleEvents(GameEngine* engine) = 0;
    virtual void Update(GameEngine* engine) = 0;
    virtual void Render(GameEngine* engine) = 0;
};