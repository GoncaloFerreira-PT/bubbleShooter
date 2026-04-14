#pragma once
#include "core/config/config.h"
#include "core/gameScene.h"
#include "core/utils/math.h"
#include <SDL3/SDL.h>
#include <functional>

using namespace Math;

class RayCast {
private:
  vec2 origin = {0, 0};
  vec2 direction = {0, 0};
  SDL_Color color = {255, 0, 0, 255};
  std::function<bool(const vec2 &)> checkCondition;

public:
  RayCast() = default;

  RayCast(vec2 _origin, vec2 _direction) : origin(_origin), direction(_direction) {}

  void Update(vec2 _origin, vec2 _direction) {
    origin = _origin;
    direction = _direction.normalized();
  }

  void SetCheckCondition(std::function<bool(const vec2 &)> condition) { checkCondition = std::move(condition); }

  void Draw(SDL_Renderer *renderer) {
    if (!checkCondition) return;

    float maxDist = Core::Config::Physics::Raycast::MAX_DISTANCE;
    float step = Core::Config::Physics::Raycast::STEP_SIZE;
    float collisionDist = maxDist;

    // 1. Calculate collision distance
    for (float dist = 0; dist < maxDist; dist += step) {
      vec2 currentPos = origin + (direction * dist);

      if (checkCondition(currentPos) || !GameScene::Instance().IsInsideScreen(currentPos)) {
        collisionDist = dist;
        break;
      }
    }

    // 2. Render the ray dots
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    float visualStep = Core::Config::Physics::Raycast::VISUAL_STEP_SIZE;

    for (float dist = 0; dist < collisionDist; dist += visualStep) {
      vec2 dotPos = origin + (direction * dist);
      SDL_FRect dotRect = {dotPos.x - 3.0f, dotPos.y - 3.0f, 6.0f, 6.0f};
      SDL_RenderFillRect(renderer, &dotRect);
    }
  }
};