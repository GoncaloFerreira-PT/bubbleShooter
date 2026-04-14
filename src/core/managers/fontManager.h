#pragma once
#include "core/utils/console.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

class FontManager {
private:
  std::unordered_map<std::string, std::shared_ptr<TTF_Font>> fontMap{};

  std::string GetCompleteFontID(const std::string &fontPath, const int fontSize);

public:
  static FontManager &Instance() {
    static FontManager instance;
    return instance;
  }

  void Delete(const std::string &fontPath, const int fontSize);
  std::shared_ptr<TTF_Font> GetFont(const std::string &fontPath, const int fontSize);
  bool Load(const std::string &fontPath, const int fontSize);
};