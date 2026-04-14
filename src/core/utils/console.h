#pragma once
#include <SDL3/SDL.h>
#include <string_view>

class Console {
public:
  static void Log(std::string_view message) {
    SDL_Log("[%s] INFO: %.*s", GetTimeStamp(), (int)message.length(), message.data());
  }

  static void Warn(std::string_view message) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] WARNING: %.*s", GetTimeStamp(), (int)message.length(),
                message.data());
  }

  static void Error(std::string_view message) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[%s] ERROR: %.*s", GetTimeStamp(), (int)message.length(),
                 message.data());
  }

private:
  static const char *GetTimeStamp() {
    static char buffer[20];
    Uint64 ticks = SDL_GetTicks();

    Uint64 seconds = ticks / 1000;
    Uint64 minutes = seconds / 60;
    Uint64 hours = minutes / 60;

    SDL_snprintf(buffer, sizeof(buffer), "%02llu:%02llu:%02llu", hours % 24, minutes % 60, seconds % 60);

    return buffer;
  }
};