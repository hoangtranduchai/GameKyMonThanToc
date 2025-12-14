#pragma once
#include <SDL.h>
#include <vector>
#include <algorithm>

struct Particle {
    float x, y;          // Vị trí
    float velX, velY;    // Vận tốc
    float life;          // Tuổi thọ (giảm dần về 0)
    float startLife;     // Tuổi thọ ban đầu
    SDL_Color color;     // Màu sắc
    float size;          // Kích thước
};

class ParticleSystem {
public:
    static ParticleSystem* GetInstance() {
        static ParticleSystem instance;
        return &instance;
    }

    // Khởi tạo một vụ nổ hạt tại vị trí (x, y)
    void Emit(int x, int y, int count, SDL_Color color);

    // Cập nhật trạng thái các hạt
    void Update(float dt);

    // Vẽ các hạt lên màn hình
    void Render(SDL_Renderer* renderer);

    // Dọn dẹp
    void Clean() { m_particles.clear(); }

private:
    ParticleSystem() {}
    std::vector<Particle> m_particles;
};