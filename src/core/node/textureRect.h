#pragma once

#include "core/managers/textureManager.h"
#include "core/node/node.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>

class TextureRect : public Node {

private:
  std::string textureID;

public:
  TextureRect(SDL_FRect rect, const std::string &textureID) : Node(rect), textureID(textureID) {}

  void Draw(SDL_Renderer *renderer) override {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Color modulate = GetModulate();
    SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
    TextureManager::Instance().Draw(textureID, rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
};