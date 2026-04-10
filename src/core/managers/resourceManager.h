#pragma once
#include <string>
#include <json.hpp>
#include <fstream>
#include "textureManager.h"
#include "fontManager.h"
#include "audioManager.h"
#include <SDL3/SDL.h>

class ResourceManager{


public:
    struct ResourceAtlasData{

    };

    struct ResourceData{
        std::vector<ResourceAtlasData> atlases;
    };


    const std::string resourcesPath = "resources/resources.json";

public:
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    bool Load();
};