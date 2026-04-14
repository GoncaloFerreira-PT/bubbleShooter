#include "core/node/toggleButton.h"

void ToggleButton::OnClick() {
  pressed = !pressed;
  if (callback) callback(pressed);
}

void ToggleButton::Draw(SDL_Renderer *renderer) {
  SDL_Color modulate = GetModulate();
  SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
  if (!textureIDDown.empty() && !textureIDUp.empty()) {
    if (pressed)
      TextureManager::Instance().Draw(textureIDDown, rect);
    else
      TextureManager::Instance().Draw(textureIDUp, rect);
  } else {

    SDL_RenderFillRect(renderer, &rect);
  }

  if (label) {
    // Center the label's texture inside the button
    label->rect.x = rect.x + (rect.w - label->rect.w) / 2.0f;
    label->rect.y = rect.y + (rect.h - label->rect.h) / 2.0f - 4.0f;
    label->Draw(renderer);
  }

  if (textureRect) textureRect->Draw(renderer);
}