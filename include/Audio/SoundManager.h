#pragma once

// --- THƯ VIỆN CẦN THIẾT ---
#include <SDL_mixer.h>
#include <string>
#include <map>
#include <iostream>
#include <vector>

// CLASS: SoundManager (MẪU THIẾT KẾ SINGLETON)
class SoundManager {
public:
    // Lấy thể hiện duy nhất của SoundManager
    static SoundManager* GetInstance();
    
    // Dọn dẹp hệ thống âm thanh
    void Clean();

    // Tải nhạc nền (Music - file dài, định dạng mp3)
    bool LoadMusic(const std::string& fileName, const std::string& id);

    // Tải hiệu ứng âm thanh (SFX - file ngắn, định dạng wav)
    bool LoadSFX(const std::string& fileName, const std::string& id);

    // Phát nhạc nền
    void PlayMusic(const std::string& id); // Mặc định lặp vô tận
    
    // Phát hiệu ứng âm thanh
    void PlaySFX(const std::string& id);   // Mặc định phát 1 lần

    // Bật / Tắt toàn bộ âm thanh
    void SetMute(bool isMuted);

private:
    SoundManager();
    ~SoundManager();

    // Con trỏ thể hiện duy nhất (Singleton instance)
    static SoundManager* s_Instance;

    // Cấu trúc Map lưu trữ nhạc nền: Khóa = Tên ID, Giá trị = Con trỏ Mix_Music*
    std::map<std::string, Mix_Music*> m_musicMap;

    // Cấu trúc Map lưu trữ hiệu ứng âm thanh: Khóa = Tên ID, Giá trị = Con trỏ Mix_Chunk*
    std::map<std::string, Mix_Chunk*> m_sfxMap;
};