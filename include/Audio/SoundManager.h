#pragma once
#include <SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <iostream>

class SoundManager {
public:
    // Singleton Pattern
    static SoundManager* GetInstance() {
        static SoundManager instance;
        return &instance;
    }

    // Tải Nhạc nền (Music - file dài, nén như mp3)
    bool LoadMusic(const std::string& fileName, const std::string& id);

    // Tải Hiệu ứng âm thanh (SFX - file ngắn, wav)
    bool LoadSFX(const std::string& fileName, const std::string& id);

    // Phát nhạc nền (Loop = -1 là lặp vô tận)
    void PlayMusic(const std::string& id, int loop = -1) noexcept;

    // Phát hiệu ứng âm thanh
    void PlaySFX(const std::string& id) noexcept;

    // Thiết lập âm lượng Nhạc nền
    void SetMusicVolume(int volume) noexcept;    // 0-128

    // Dọn dẹp bộ nhớ
    void Clean() noexcept;

private:
    SoundManager() {}
    ~SoundManager() {}

    std::unordered_map<std::string, Mix_Music*> m_musicMap;
    std::unordered_map<std::string, Mix_Chunk*> m_sfxMap;
};