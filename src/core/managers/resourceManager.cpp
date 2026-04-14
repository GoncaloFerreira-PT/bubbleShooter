#include "resourceManager.h"

bool ResourceManager::Load() {
  std::string jsonPath = SDL_GetBasePath() + resourcesPath;
  std::ifstream file(jsonPath);
  if (!file.is_open()) {
    Console::Error("Couldn't open resources JSON: " + jsonPath);
    return false;
  }

  nlohmann::json jsonFile;
  try {
    file >> jsonFile;
  } catch (const nlohmann::json::parse_error &e) {
    Console::Error("Failed to parse resources.json: " + std::string(e.what()));
    return false;
  }

  // Load Textures
  if (jsonFile.contains("textures") && jsonFile["textures"].is_array()) {
    for (const auto &obj : jsonFile["textures"]) {
      std::string name = obj["name"];
      std::string path = obj["path"];
      if (!TextureManager::Instance().Load(path, name)) {
        Console::Error("Failed to load texture: " + name);
        return false;
      }
    }
  }

  // Load Atlases
  if (jsonFile.contains("atlases") && jsonFile["atlases"].is_array()) {
    for (const auto &obj : jsonFile["atlases"]) {
      auto atlasData = obj;
      if (!TextureManager::Instance().LoadAtlas(atlasData)) {
        std::string path_str = obj.value("path", "[unknown atlas]");
        Console::Error("Failed to load atlas: " + path_str);
        return false;
      }
    }
  }

  // Load Sounds
  if (jsonFile.contains("sounds") && jsonFile["sounds"].is_array()) {
    for (const auto &obj : jsonFile["sounds"]) {
      std::string name = obj["name"];
      std::string path = obj["path"];

      if (!AudioManager::Instance().Load(path, name)) {
        Console::Error("Failed to load sound: " + name);
        return false;
      }
    }
  }

  // Load Fonts
  if (jsonFile.contains("fonts") && jsonFile["fonts"].is_array()) {
    for (const auto &obj : jsonFile["fonts"]) {
      std::string path = obj["path"];
      int size = obj["size"];

      if (!FontManager::Instance().Load(path, size)) {
        Console::Error("Failed to load font: " + path + " with size: " + std::to_string(size));
        return false;
      }
    }
  }

  return true;
}