#include "core/node/label.h"
#include "core/managers/fontManager.h"

void Label::SetText(const std::string &newText) {
  if (text != newText) {
    text = newText;
    dirty = true;
    UpdateSizeOnly();
  }
}

void Label::SetFont(const std::string &fontPath, const int fontSize) {
  std::shared_ptr<TTF_Font> newFont = FontManager::Instance().GetFont(fontPath, fontSize);
  if (font != newFont) {
    font = newFont;
    dirty = true;
  }
}

void Label::RegenerateTexture(SDL_Renderer *renderer) {
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }

  if (!font || text.empty()) {
    dirty = false;
    return;
  }

  SDL_Surface *surface = TTF_RenderText_Blended(font.get(), text.c_str(), text.length(), {255, 255, 255, 255});
  if (!surface) return;

  texture = SDL_CreateTextureFromSurface(renderer, surface);
  rect.w = static_cast<float>(surface->w);
  rect.h = static_cast<float>(surface->h);

  SDL_DestroySurface(surface);
  dirty = false;
}

void Label::UpdateSizeOnly() {
  if (font && !text.empty()) {
    int w, h;
    if (TTF_GetStringSize(font.get(), text.c_str(), 0, &w, &h)) {
      rect.w = static_cast<float>(w);
      rect.h = static_cast<float>(h);
    }
  }
}

void Label::Draw(SDL_Renderer *renderer) {
  if (dirty) RegenerateTexture(renderer);
  if (!texture) return;

  SDL_SetTextureColorMod(texture, modulate.r, modulate.g, modulate.b);
  SDL_SetTextureAlphaMod(texture, modulate.a);

  SDL_FRect drawRect = GetScaledRect();
  SDL_RenderTexture(renderer, texture, nullptr, &drawRect);
}