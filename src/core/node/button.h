#pragma once
#include "core/node/label.h"
#include "core/node/node.h"
#include "core/node/textureRect.h"
#include <SDL3/SDL.h>

class Button : public Node {

protected:
  std::shared_ptr<Label> label;
  std::shared_ptr<TextureRect> textureRect;
  std::string textureIDUp;
  std::string textureIDDown;
  bool hovered = false;
  bool pressed = false;
  std::function<void(bool pressed)> callback;

public:
  Button(SDL_FRect rect, std::function<void(bool pressed)> callback) : Node(rect), callback(std::move(callback)) {}

  void Draw(SDL_Renderer *renderer) override;
  bool HandleInput(SDL_Event *event) override;
  void SetText(const std::string &text, const std::string &fontPath, int fontSize);
  void SetTextureRect(const std::string &textureID);
  void SetTextures(const std::string &textureIDUp, const std::string &textureIDDown);
  virtual void OnClick();
  void OnEnter();
  void OnLeave();
};