#include "fontManager.h"

void FontManager::Delete(const std::string &fontID, const int fontSize) {
  fontMap.erase(GetCompleteFontID(fontID, fontSize));
}

bool FontManager::Load(const std::string &fontID, const int fontSize) {
  const std::string fullID = GetCompleteFontID(fontID, fontSize);
  const std::string fullPath = SDL_GetBasePath() + fontID;
  TTF_Font *font = TTF_OpenFont(fullPath.c_str(), fontSize);
  if (!font) {
    Console::Error("Failed to load font: " + fullPath + " with error: " + std::string(SDL_GetError()));
    return false;
  }
  auto newFont = std::shared_ptr<TTF_Font>(font, TTF_CloseFont);
  fontMap[fullID] = newFont;
  return true;
}

std::shared_ptr<TTF_Font> FontManager::GetFont(const std::string &fontID, const int fontSize) {
  const std::string fullID = GetCompleteFontID(fontID, fontSize);
  const auto it = fontMap.find(fullID);
  if (it != fontMap.end()) { return it->second; }

  return Load(fontID, fontSize) ? fontMap[fullID] : nullptr;
}

std::string FontManager::GetCompleteFontID(const std::string &fontID, const int fontSize) {
  return fontID + "_" + std::to_string(fontSize);
}