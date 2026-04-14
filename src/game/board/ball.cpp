#include "game/board/ball.h"
#include "core/managers/textureManager.h"
#include "game/bubbleGameConfig.h"

void Ball::Animate(Ball::Animation type) {
  animation = type;
  RemoveCollisionBox();
  if (type == Animation::Pop) {
    speed = 0.0f;
  } else if (type == Animation::Fall) {
    speed = Game::Config::Rules::BALL_FALL_SPEED;
  }
}

vec2i Ball::GetGridPosition() { return gridPosition; }

void Ball::SetGridPosition(vec2i position) { gridPosition = position; }

void Ball::Draw(SDL_Renderer *renderer) {
  SDL_FRect scaledRect = GetScaledRect();
  TextureManager::Instance().Draw("img_bubble", scaledRect, GetModulate());
}

void Ball::Update(float deltaTime) {
  int alpha = GetAlpha();
  vec2 scale = GetScale();
  if (animation == Animation::Pop) {
    SetAlpha(alpha - deltaTime);
    SetScale(scale - deltaTime);
    if (GetAlpha() == 0) {
      Destroy();
      return;
    }
  } else if (animation == Animation::Fall) {
    Move(deltaTime);
    SetAlpha(alpha - deltaTime * 0.05f);
    if (GetAlpha() == 0) {
      Destroy();
      return;
    }
  }
}