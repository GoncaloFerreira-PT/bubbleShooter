#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <map>
#include <unordered_map>
#include <memory>
#include "core/utils/console.h"
#include <json.hpp>

class TextureManager
{
public:
    struct TextureData {
        std::shared_ptr<SDL_Texture> texture;
        bool isPartOfAtlas = false;
        SDL_FRect rect;
    };

private:
    std::unordered_map<std::string, std::shared_ptr<TextureData>> textureMap{};
    SDL_Renderer *renderer;

public:
    static TextureManager &Instance()
    {
        static TextureManager instance;
        return instance;
    }

    void SetRenderer(SDL_Renderer *_renderer)
    {
        renderer = _renderer;
    }

    bool Load(const std::string &filePath, const std::string &textureID);
    bool LoadAtlas(nlohmann::json &atlasData);
    void Draw(const std::string &textureID, SDL_FRect rect, SDL_Color modulate);
    void DrawRotated(const std::string &textureID, SDL_FRect rect, SDL_Color modulate, float angle, SDL_FPoint center, SDL_FlipMode flip);
    void ClearFromTextureMap(const std::string &textureID);
    bool IsTextureInMap(const std::string &textureID);
    std::shared_ptr<TextureData> GetTextureData(const std::string &textureID);
};