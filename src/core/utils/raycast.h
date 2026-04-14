#pragma once
#include "core/config/config.h"
#include "core/gameScene.h"
#include "core/utils/math.h"
#include <SDL3/SDL.h>

using namespace Math;

class RayCast {
  vec2 origin;
  vec2 direction;
  SDL_Color modulate = {255, 0, 0, 255};
  std::function<bool(const vec2 &)> checkCondition;

public:
  RayCast() {}

  RayCast(vec2 _origin, vec2 _direction) : origin(_origin), direction(_direction) {}

  void Update(vec2 _origin, vec2 _direction) {
    origin = _origin;
    direction = _direction;
  }

  void SetCheckCondition(std::function<bool(const vec2 &)> _checkCondition) {
    checkCondition = std::move(_checkCondition);
  }

  void Draw(SDL_Renderer *renderer) {
    if (!checkCondition) return;

    vec2 currentPos = origin;
    float collisionDist = Core::Config::Physics::Raycast::MAX_DISTANCE;
    for (float dist = 0; dist < Core::Config::Physics::Raycast::MAX_DISTANCE;
         dist += Core::Config::Physics::Raycast::STEP_SIZE) {
      currentPos = origin + (direction * dist);

      if (checkCondition(currentPos) || !GameScene::Instance().IsInsideScreen(currentPos)) {
        collisionDist = dist;
        break;
      }
    }

    for (float dist = 0; dist < collisionDist; dist += Core::Config::Physics::Raycast::VISUAL_STEP_SIZE) {
      vec2 dotPos = origin + (direction * dist);
      SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
      SDL_FRect dotRect = {dotPos.x - 3, dotPos.y - 3, 6, 6};
      SDL_RenderFillRect(renderer, &dotRect);
    }
  }
};