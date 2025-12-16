#include "SoundManager.h"

bool SoundManager::LoadMusic(const std::string& fileName, const std::string& id) {
    Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
    if (pMusic == nullptr) {
        std::cout << "[Loi Audio] Khong the tai nhac: " << fileName << " - " << Mix_GetError() << std::endl;
        return false;
    }
    m_musicMap.emplace(id, pMusic);
    return true;
}

bool SoundManager::LoadSFX(const std::string& fileName, const std::string& id) {
    Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
    if (pChunk == nullptr) {
        std::cout << "[Loi Audio] Khong the tai SFX: " << fileName << " - " << Mix_GetError() << std::endl;
        return false;
    }
    m_sfxMap.emplace(id, pChunk);
    return true;
}

void SoundManager::PlayMusic(const std::string& id, int loop) noexcept {
    auto it = m_musicMap.find(id);
    if (it != m_musicMap.end() && it->second != nullptr) {
        Mix_PlayMusic(it->second, loop);
    }
}

void SoundManager::PlaySFX(const std::string& id) noexcept {
    auto it = m_sfxMap.find(id);
    if (it != m_sfxMap.end() && it->second != nullptr) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void SoundManager::SetMusicVolume(int volume) noexcept {
    Mix_VolumeMusic(volume);  // 0-128
}

void SoundManager::Clean() noexcept {
    // Xóa SFX
    for (auto const& [key, val] : m_sfxMap) {
        Mix_FreeChunk(val);
    }
    m_sfxMap.clear();

    // Xóa Music
    for (auto const& [key, val] : m_musicMap) {
        Mix_FreeMusic(val);
    }
    m_musicMap.clear();
    
    std::cout << "[He thong] Da don dep Sound Manager!" << std::endl;
}