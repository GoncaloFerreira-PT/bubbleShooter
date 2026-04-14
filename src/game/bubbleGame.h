#pragma once
#include "core/gameScene.h"

class BubbleGame : public GameScene {
public:
  BubbleGame() : GameScene() {};
  void Initialize(SDL_Renderer *renderer) override;
  void OnStateChangedCallback(std::any data) override;
  void HandleInput(SDL_Event *event) override;
};