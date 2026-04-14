#pragma once
#include "audioManager.h"
#include "fontManager.h"
#include "textureManager.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <json.hpp>
#include <string>


class ResourceManager {


public:
  struct ResourceAtlasData {};

  struct ResourceData {
    std::vector<ResourceAtlasData> atlases;
  };

  const std::string resourcesPath = "resources/resources.json";

public:
  static ResourceManager &Instance() {
    static ResourceManager instance;
    return instance;
  }

  bool Load();
};