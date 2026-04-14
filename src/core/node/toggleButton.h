#pragma once
#include "core/node/button.h"

class ToggleButton : public Button {

public:
  ToggleButton(SDL_FRect rect, std::function<void(bool pressed)> callback) : Button(rect, callback) {}

  void OnClick() override;
  void Draw(SDL_Renderer *renderer) override;
};