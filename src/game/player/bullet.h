#pragma once
#include "core/node/node.h"
#include "game/board/ball.h"

class Bullet : public MovableNode {
public:
  bool active = false;
  Color color;

  Bullet(SDL_FRect rect, Color color);
  void OnAddedToScene() override;
  void OnExitScreen() override;
  void Draw(SDL_Renderer *renderer) override;
  void Update(float deltaTime) override;
};