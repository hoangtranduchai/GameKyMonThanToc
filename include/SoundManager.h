#pragma once
#include <SDL_mixer.h>
#include <string>
#include <map>
#include <iostream>

class SoundManager {
public:
    // Singleton Pattern
    static SoundManager* GetInstance() {
        static SoundManager instance;
        return &instance;
    }

    // Tải Nhạc nền (Music - file dài, nén như mp3)
    bool LoadMusic(std::string fileName, std::string id);

    // Tải Hiệu ứng âm thanh (SFX - file ngắn, wav)
    bool LoadSFX(std::string fileName, std::string id);

    // Phát nhạc nền (Loop = -1 là lặp vô tận)
    void PlayMusic(std::string id, int loop = -1);

    // Phát hiệu ứng âm thanh
    void PlaySFX(std::string id);

    // Thiết lập âm lượng Nhạc nền
    void SetMusicVolume(int volume);    // 0-128

    // Dọn dẹp bộ nhớ
    void Clean();

private:
    SoundManager() {}
    ~SoundManager() {}

    std::map<std::string, Mix_Music*> m_musicMap;
    std::map<std::string, Mix_Chunk*> m_sfxMap;
};