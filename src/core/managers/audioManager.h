#pragma once
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <memory>
#include <string>
#include <unordered_map>

class AudioManager {
private:
  bool initialized = false;
  bool muted = false;
  std::unordered_map<std::string, std::shared_ptr<MIX_Audio>> audioMap{};
  std::unordered_map<std::string, std::shared_ptr<MIX_Track>> trackMap{};
  MIX_Mixer *mixer{nullptr};

public:
  ~AudioManager() { Destroy(); }

  static AudioManager &Instance() {
    static AudioManager instance;
    return instance;
  }

  AudioManager() { Init(); }

  void Init();
  bool Load(const std::string &filePath, const std::string &soundID);
  void PlaySound(const std::string &soundID, bool loop = false);
  void StopSound(const std::string &soundID);
  void Destroy() {};
  bool IsAudioLoaded(const std::string &soundID);

  void SetMuted(bool muted);

  bool IsMuted() const { return muted; }
};