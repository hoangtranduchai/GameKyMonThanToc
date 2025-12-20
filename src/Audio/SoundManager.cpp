#include "Audio/SoundManager.h"
#include <iostream>

// Khởi tạo con trỏ Singleton
SoundManager* SoundManager::s_Instance = nullptr;

SoundManager* SoundManager::GetInstance() {
    if (s_Instance == nullptr) s_Instance = new SoundManager();
    return s_Instance;
}

SoundManager::SoundManager() {
    std::cout << "[ÂM THANH] Đang khởi tạo hệ thống âm thanh..." << std::endl;
}

SoundManager::~SoundManager() { Clean(); }

void SoundManager::Clean() {
    // 1. Giải phóng nhạc nền (BGM)
    for (auto const& [id, music] : m_musicMap) Mix_FreeMusic(music);
    m_musicMap.clear();

    // 2. Giải phóng hiệu ứng âm thanh (SFX)
    for (auto const& [id, chunk] : m_sfxMap) Mix_FreeChunk(chunk);
    m_sfxMap.clear();

    // 3. Đóng thiết bị âm thanh
    Mix_CloseAudio();
    std::cout << "[ÂM THANH] Hệ thống đã được dọn dẹp và tắt." << std::endl;
}

bool SoundManager::LoadMusic(const std::string& fileName, const std::string& id) {
    // Tải file nhạc (chế độ phát trực tiếp - Stream mode)
    Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
    if (pMusic == nullptr) {
        std::cerr << "[LỖI] Không thể tải nhạc nền: " << fileName << " | Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    // Lưu vào Map
    m_musicMap[id] = pMusic;
    return true;
}

bool SoundManager::LoadSFX(const std::string& fileName, const std::string& id) {
    // Tải file hiệu ứng (tải vào RAM)
    Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
    if (pChunk == nullptr) {
        std::cerr << "[LỖI] Không thể tải hiệu ứng âm thanh: " << fileName << " | Lỗi: " << Mix_GetError() << std::endl;
        return false;
    }

    // Lưu vào Map
    m_sfxMap[id] = pChunk;
    return true;
}

void SoundManager::PlayMusic(const std::string& id) {
    // Tìm nhạc trong Map
    if (m_musicMap.find(id) != m_musicMap.end()) {
        Mix_PlayMusic(m_musicMap[id], -1); // -1 = Lặp vô tận
    } else {
        std::cerr << "[CẢNH BÁO] Không tìm thấy ID nhạc nền: " << id << std::endl;
    }
}

void SoundManager::PlaySFX(const std::string& id) {
    // Tìm SFX trong Map
    if (m_sfxMap.find(id) != m_sfxMap.end()) {
        Mix_PlayChannel(-1, m_sfxMap[id], 0); // 0 = Phát 1 lần
    } else {
        std::cerr << "[CẢNH BÁO] Không tìm thấy ID hiệu ứng âm thanh: " << id << std::endl;
    }
}

// CHỨC NĂNG BẬT/TẮT ÂM THANH
void SoundManager::SetMute(bool isMuted) {
    if (isMuted) {
        // Tắt tiếng: Đặt âm lượng về 0
        Mix_VolumeMusic(0);
        Mix_Volume(-1, 0); // -1 là tất cả kênh SFX
        std::cout << "[ÂM THANH] Đã tắt tiếng." << std::endl;
    } else {
        // Bật tiếng: Set volume về Max (128)
        Mix_VolumeMusic(128);
        Mix_Volume(-1, 128);
        std::cout << "[ÂM THANH] Đã bật tiếng." << std::endl;
    }
}