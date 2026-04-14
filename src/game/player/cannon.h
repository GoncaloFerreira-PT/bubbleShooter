#pragma once
#include "core/node/node.h"
#include "core/utils/math.h"
#include "game/board/ball.h"
#include "game/controllers/ballController.h"
#include "game/player/bullet.h"
#include <any>
#include <core/utils/raycast.h>
#include <memory>

class Cannon : public Node {
private:
  std::shared_ptr<BallController> ballController;

public:
  vec2 direction = {0.0f, -1.0f};
  std::shared_ptr<Bullet> currentBullet = nullptr;
  std::unique_ptr<RayCast> rc = nullptr;
  bool blocked = false;

  Cannon(SDL_FRect rect);
  void OnAddedToScene() override;
  void Draw(SDL_Renderer *renderer) override;
  bool HandleInput(SDL_Event *event) override;
  void Shoot();
  void LoadBullet(Color color);

  void OnBallCreatedCallback(std::any data);
};
