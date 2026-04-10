#include "textureManager.h"
#include <SDL3_image/SDL_image.h>

bool TextureManager::Load(const std::string &filePath, const std::string &textureID)
{
    std::string fullPath = SDL_GetBasePath() + filePath;
    if (IsTextureInMap(textureID))
    {
        Console::Warn("Texture with ID " + textureID + " already exists in the map.");
        return false;
    }

    SDL_Texture *texture = IMG_LoadTexture(renderer, fullPath.c_str());
    if (!texture)
    {
        Console::Error("Couldn't create static texture: " + std::string(SDL_GetError()));
        return false;
    }

    auto textureData = std::make_shared<TextureManager::TextureData>();
    textureData->texture = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
    textureMap[textureID] = textureData;

    return true;
}

bool TextureManager::LoadAtlas(nlohmann::json &atlasData)
{
    std::string pathValue = atlasData.at("path").get<std::string>();
    std::string texturePath = SDL_GetBasePath() + pathValue;
    SDL_Texture *texture = IMG_LoadTexture(renderer, texturePath.c_str());
    if (!texture)
    {
        Console::Error("Couldn't create atlas texture: " + std::string(SDL_GetError()));
        return false;
    }

    auto sharedTexture = std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture);
    for (auto &region : atlasData["regions"])
    {
        std::string name = region["name"];
        float x = region["x"];
        float y = region["y"];
        float w = region["w"];
        float h = region["h"];
        auto newTextureData = std::make_shared<TextureManager::TextureData>();
        newTextureData->texture = sharedTexture;
        newTextureData->isPartOfAtlas = true;
        newTextureData->rect = {x,y,w,h};
        textureMap[name] = newTextureData;
    }

    return true;
}


void TextureManager::Draw(const std::string &textureID, SDL_FRect rect, SDL_Color modulate)
{
    auto textureData = GetTextureData(textureID);
    if (!textureData) return;
    SDL_Texture *texture = textureData->texture.get();
    SDL_FRect *srcRect = nullptr;
    if (textureData->isPartOfAtlas)
    {
        srcRect = &textureData->rect;
    }
    SDL_SetTextureColorMod(texture, modulate.r, modulate.g, modulate.b);
    SDL_SetTextureAlphaMod(texture, modulate.a);
    SDL_RenderTexture(renderer, texture, srcRect, &rect);
}


void TextureManager::DrawRotated(const std::string &textureID, SDL_FRect rect, SDL_Color modulate, float angle, SDL_FPoint center, SDL_FlipMode flip)
{
    auto textureData = GetTextureData(textureID);
    if (!textureData) return;
    SDL_Texture *texture = textureData->texture.get();
    SDL_FRect *srcRect = nullptr;
    if (textureData->isPartOfAtlas)
    {
        srcRect = &textureData->rect;
    }
    SDL_SetTextureColorMod(texture, modulate.r, modulate.g, modulate.b);
    SDL_SetTextureAlphaMod(texture, modulate.a);
    SDL_RenderTextureRotated(renderer, texture, srcRect, &rect, angle, &center, flip);
}

void TextureManager::ClearFromTextureMap(const std::string &textureID)
{
    textureMap.erase(textureID);
}


bool TextureManager::IsTextureInMap(const std::string &textureID)
{
    return textureMap.find(textureID) != textureMap.end();
}


std::shared_ptr<TextureManager::TextureData> TextureManager::GetTextureData(const std::string &textureID)
{
    auto it = textureMap.find(textureID);
    if (it != textureMap.end())
    {
        return it->second;
    }
    return nullptr;
}