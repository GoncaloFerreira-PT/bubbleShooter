#pragma once
#include <SDL3/SDL.h>
#include <string>

class Console

{
public:
    static void Log(const std::string &message)
    {
        SDL_Log("[%s] INFO: %s", getTimeStamp(), message.c_str());
    }

    static void Warn(const std::string &message)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[%s] WARNING: %s", getTimeStamp(), message.c_str());
    }

    static void Error(const std::string &message)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[%s] ERROR: %s", getTimeStamp(), message.c_str());
    }

private:

    static const char* getTimeStamp()
    {
        static char buffer[20];
        Uint64 ticks = SDL_GetTicks();
        Uint64 seconds = ticks / 1000;
        Uint64 minutes = seconds / 60;
        Uint64 hours = minutes / 60;

        SDL_snprintf(buffer, sizeof(buffer), "%02llu:%02llu:%02llu", 
                     hours % 24, minutes % 60, seconds % 60);
        
        return buffer;
    }
};