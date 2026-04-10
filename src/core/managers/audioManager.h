#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <map>
#include <unordered_map>
#include <memory>
#include "core/utils/console.h"

class AudioManager
{
private:
    bool initialized = false;
    std::unordered_map<std::string, std::shared_ptr<MIX_Audio>> audioMap{};
    MIX_Mixer *sfxMixer{nullptr};

public:
    ~AudioManager()
    {
        Destroy();
    }

    static AudioManager &Instance()
    {
        static AudioManager instance;
        return instance;
    }

    AudioManager(){Init();}

    void Init();
    bool Load(const std::string &filePath, const std::string &soundID);
    void PlaySound(const std::string &soundID);
    void StopSound(const std::string &soundID);
    void Destroy(){};
    bool IsAudioLoaded(const std::string &soundID);
};