#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <map>
#include <unordered_map>
#include <memory>
#include "core/utils/console.h"
#include <SDL3_ttf/SDL_ttf.h>


class FontManager
{
private:
    std::unordered_map<std::string, std::shared_ptr<TTF_Font>> fontMap{};

    std::string GetCompleteFontID(const std::string &fontPath, const int fontSize);
public:
    static FontManager &Instance()
    {
        static FontManager instance;
        return instance;
    }

    void Delete(const std::string &fontPath, const int fontSize);
    std::shared_ptr<TTF_Font> GetFont(const std::string &fontPath, const int fontSize);
    bool Load(const std::string &fontPath, const int fontSize);
};