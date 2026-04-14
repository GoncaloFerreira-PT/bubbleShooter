#include "cannon.h"
#include "ball.h"
#include "core/managers/eventManager.h"
#include "core/managers/textureManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/node/node.h"
#include "game/bubbleGameConfig.h"

Cannon::Cannon(SDL_FRect rect) : Node(rect) {
  rc = std::make_unique<RayCast>();
  rc->SetCheckCondition([this](const vec2 &pos) {
    if (!ballContainer) return false;

    vec2 ballSize = Game::Config::Grid::BALL_SIZE;
    vec2i gridPos = ballContainer->WorldToGrid(pos - vec2(ballSize.x / 2, ballSize.y / 2));
    auto ball = ballContainer->GetBallAt(gridPos);
    return (ball && !ball->IsPendingDestruction());
  });

  EventManager::Instance().Subscribe("ballCreated", this, [this](std::any data) { OnBallCreatedCallback(data); });

  EventManager::Instance().Subscribe("ballHit", this, [this](std::any data) { this->blocked = false; });

  EventManager::Instance().Subscribe("stateChanged", this, [this](std::any data) { this->blocked = true; });
}

void Cannon::OnBallCreatedCallback(std::any data) {
  auto ball = std::any_cast<std::shared_ptr<Ball>>(data);
  LoadBullet(ball->color);
}

void Cannon::OnAddedToScene() {
  ballContainer = GameScene::Instance().GetFirstNodeOfType<BallContainer>();
  LoadBullet(GetRandomColor());
}

void Cannon::Shoot() {
  blocked = true;
  if (currentBullet && !currentBullet->active) {
    currentBullet->active = true;
    currentBullet->direction = direction;
    currentBullet = nullptr;
    EventManager::Instance().Emit("ballShot", currentBullet);
  }
}

void Cannon::LoadBullet(Color color) {
  if (currentBullet) return;
  vec2 ballSize = Game::Config::Grid::BALL_SIZE;
  auto bullet = std::make_shared<Bullet>(
      SDL_FRect{rect.x + ballSize.x / 2, rect.y + ballSize.y / 2, ballSize.x, ballSize.y}, (Color)color);

  GameScene::Instance().AddNode(bullet);
  currentBullet = bullet;
}

void Cannon::Draw(SDL_Renderer *renderer) {
  // Draw the aiming raycast
  vec2 center = GetCenter();
  rc->Update(center, direction);
  rc->Draw(renderer);

  float angle = Math::RadToDeg(std::atan2(direction.y, direction.x)) + 90.0f;
  TextureManager::Instance().DrawRotated("img_cannon", rect, GetModulate(), angle, SDL_FPoint{rect.w / 2, rect.h / 2},
                                         SDL_FLIP_NONE);
}

bool Cannon::HandleInput(SDL_Event *event) {
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    SDL_Renderer *renderer = GameScene::Instance().GetRenderer();

    float logicalMouseX, logicalMouseY;

    SDL_RenderCoordinatesFromWindow(renderer, event->motion.x, event->motion.y, &logicalMouseX, &logicalMouseY);

    vec2 center = GetCenter();
    vec2 targetVector = {logicalMouseX - center.x, logicalMouseY - center.y};
    direction = targetVector.normalized();
  }

  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    if (currentBullet && !blocked) {
      Shoot();
      return true;
    }
  }
  return false;
}
