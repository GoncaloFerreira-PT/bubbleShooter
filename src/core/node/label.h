#pragma once
#include "node.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <string>

class Label : public Node {
public:
  Label() = default;

  Label(SDL_FRect rect) : Node(rect) {}

  ~Label() override {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }

  void Update(float deltaTime) override {}

  void Draw(SDL_Renderer *renderer) override;
  void SetText(const std::string &newText);
  void SetFont(const std::string &fontPath, const int fontSize);
  void UpdateSizeOnly();

private:
  std::string text;
  std::shared_ptr<TTF_Font> font;
  SDL_Texture *texture = nullptr;
  bool dirty = true;
  void RegenerateTexture(SDL_Renderer *renderer);
};