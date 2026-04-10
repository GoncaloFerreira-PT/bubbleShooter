#include "core/managers/audioManager.h"

void AudioManager::Init()
{
    if (!SDL_WasInit(SDL_INIT_AUDIO))
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            return;
        }
    }

    if (!MIX_Init())
    {
        Console::Error("Failed to initialize SDL3_mixer library");
        return;
    }

    sfxMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!sfxMixer)
    {
        Console::Error("Failed to create SDL3_mixer instance");
        MIX_Quit();
        return;
    }

    initialized = true;
}


bool AudioManager::IsAudioLoaded(const std::string &soundID)
{
    return audioMap.find(soundID) != audioMap.end();
}


bool AudioManager::Load(const std::string &filePath, const std::string &soundID)
{
    std::string fullPath = SDL_GetBasePath() + filePath;
    if (IsAudioLoaded(soundID))
    {
        Console::Warn("Audio with ID " + soundID + " already exists in the map.");
        return false;
    }

    MIX_Audio *audio = MIX_LoadAudio(sfxMixer, fullPath.c_str(), false);
    if (!audio)
    {
        Console::Error("Couldn't create audio: " + std::string(SDL_GetError()));
        return false;
    }

    audioMap[soundID] = std::shared_ptr<MIX_Audio>(audio, MIX_DestroyAudio);
    return true;
}


void AudioManager::PlaySound(const std::string &soundID)
{
    if (!initialized)return;
    if (!IsAudioLoaded(soundID))
    {
        Console::Warn("Could not play Audio with ID " + soundID);
        return;
    }

    MIX_Audio *audio = audioMap[soundID].get();
    MIX_Track *track = MIX_CreateTrack(sfxMixer);
    if (!track) {
        Console::Warn("Couldn't create a mixer track: " + std::string(SDL_GetError()));
    }

    MIX_SetTrackAudio(track, audio);
    MIX_PlayTrack(track, 0);
}
