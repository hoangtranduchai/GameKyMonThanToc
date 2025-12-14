#include "SoundManager.h"

bool SoundManager::LoadMusic(std::string fileName, std::string id) {
    Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
    if (pMusic == nullptr) {
        std::cout << "[Loi Audio] Khong the tai nhac: " << fileName << " - " << Mix_GetError() << std::endl;
        return false;
    }
    m_musicMap[id] = pMusic;
    return true;
}

bool SoundManager::LoadSFX(std::string fileName, std::string id) {
    Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
    if (pChunk == nullptr) {
        std::cout << "[Loi Audio] Khong the tai SFX: " << fileName << " - " << Mix_GetError() << std::endl;
        return false;
    }
    m_sfxMap[id] = pChunk;
    return true;
}

void SoundManager::PlayMusic(std::string id, int loop) {
    if (m_musicMap[id] != nullptr) {
        Mix_PlayMusic(m_musicMap[id], loop);
    }
}

void SoundManager::PlaySFX(std::string id) {
    if (m_sfxMap[id] != nullptr) {
        Mix_PlayChannel(-1, m_sfxMap[id], 0);
    }
}

void SoundManager::SetMusicVolume(int volume) {
    Mix_VolumeMusic(volume);  // 0-128
}

void SoundManager::Clean() {
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