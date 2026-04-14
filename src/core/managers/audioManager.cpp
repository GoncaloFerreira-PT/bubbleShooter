#include "core/managers/audioManager.h"
#include "core/utils/console.h"
#include <SDL3_mixer/SDL_mixer.h>

void AudioManager::Init() {
  if (!SDL_WasInit(SDL_INIT_AUDIO)) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) { return; }
  }

  if (!MIX_Init()) {
    Console::Error("Failed to initialize SDL3_mixer library");
    return;
  }

  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (!mixer) {
    Console::Error("Failed to create SDL3_mixer instance");
    MIX_Quit();
    return;
  }

  initialized = true;
}

void AudioManager::SetMuted(bool muted) {
  this->muted = muted;
  if (this->muted) {
    MIX_PauseAllTracks(mixer);
  } else {
    MIX_ResumeAllTracks(mixer);
  }
}

bool AudioManager::IsAudioLoaded(const std::string &soundID) { return audioMap.find(soundID) != audioMap.end(); }

bool AudioManager::Load(const std::string &filePath, const std::string &soundID) {
  std::string fullPath = SDL_GetBasePath() + filePath;
  if (IsAudioLoaded(soundID)) {
    Console::Warn("Audio with ID " + soundID + " already exists in the map.");
    return false;
  }

  MIX_Audio *audio = MIX_LoadAudio(mixer, fullPath.c_str(), false);
  if (!audio) {
    Console::Error("Couldn't create audio: " + std::string(SDL_GetError()));
    return false;
  }

  audioMap[soundID] = std::shared_ptr<MIX_Audio>(audio, MIX_DestroyAudio);
  return true;
}

void AudioManager::PlaySound(const std::string &soundID, bool loop) {
  if (!initialized || muted) return;
  if (!IsAudioLoaded(soundID)) {
    Console::Warn("Could not play Audio with ID " + soundID);
    return;
  }

  MIX_Track *track = trackMap.find(soundID) != trackMap.end() ? trackMap[soundID].get() : nullptr;

  if (track) {
    MIX_PlayTrack(track, 0);
    return;
  }

  track = MIX_CreateTrack(mixer);
  if (!track) { Console::Warn("Couldn't create a mixer track: " + std::string(SDL_GetError())); }

  trackMap[soundID] = std::shared_ptr<MIX_Track>(track, MIX_DestroyTrack);

  MIX_Audio *audio = audioMap[soundID].get();
  MIX_SetTrackAudio(track, audio);
  if (loop) {
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(track, props);
    SDL_DestroyProperties(props);
  } else {
    MIX_PlayTrack(track, 0);
  }
}
