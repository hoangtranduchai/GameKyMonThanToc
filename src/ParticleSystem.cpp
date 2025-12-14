#include "ParticleSystem.h"
#include <cstdlib>

void ParticleSystem::Emit(int x, int y, int count, SDL_Color color) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = (float)x;
        p.y = (float)y;
        
        // Tạo vận tốc ngẫu nhiên để hạt bay tứ phía
        // (rand() % 200 - 100) -> khoảng từ -100 đến 100
        p.velX = (float)(rand() % 400 - 200); 
        p.velY = (float)(rand() % 400 - 200);
        
        p.startLife = 0.5f + ((rand() % 100) / 200.0f); // Sống từ 0.5s - 1.0s
        p.life = p.startLife;
        
        p.color = color;
        p.size = 5.0f + (rand() % 10); // Kích thước ngẫu nhiên 5-15px
        
        m_particles.push_back(p);
    }
}

void ParticleSystem::Update(float dt) {
    for (auto it = m_particles.begin(); it != m_particles.end(); ) {
        // Di chuyển hạt
        it->x += it->velX * dt;
        it->y += it->velY * dt;
        
        // Giảm tuổi thọ
        it->life -= dt;
        
        // Hiệu ứng trọng lực nhẹ (nếu muốn hạt rơi xuống)
        // it->velY += 200.0f * dt; 

        // Nếu hết đời -> Xóa
        if (it->life <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::Render(SDL_Renderer* renderer) {
    // Bật chế độ hòa trộn Additive (Cộng màu) -> Tạo hiệu ứng phát sáng
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

    for (const auto& p : m_particles) {
        // Tính độ trong suốt (Alpha) dựa trên tuổi thọ còn lại
        Uint8 alpha = (Uint8)((p.life / p.startLife) * 255);
        
        SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, alpha);
        
        // Vẽ hạt (hình vuông nhỏ)
        SDL_Rect rect = { (int)p.x, (int)p.y, (int)p.size, (int)p.size };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Trả về chế độ hòa trộn bình thường để không ảnh hưởng cái khác
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}