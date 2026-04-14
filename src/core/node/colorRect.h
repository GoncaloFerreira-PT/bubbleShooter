#pragma once

#include "core/node/node.h"
#include <SDL3/SDL.h>

class ColorRect : public Node {

public:
  ColorRect(SDL_FRect rect, SDL_Color color) : Node(rect) { this->SetModulate(color); }

  void Draw(SDL_Renderer *renderer) override {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Color modulate = GetModulate();
    SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
};