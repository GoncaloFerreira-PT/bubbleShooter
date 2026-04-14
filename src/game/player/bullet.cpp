#include "bullet.h"
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include "core/managers/textureManager.h"
#include "core/modifiers/collisionBox.h"
#include "game/bubbleGameConfig.h"

Bullet::Bullet(SDL_FRect rect, Color color) : MovableNode(rect) {
  this->active = false;
  this->speed = Game::Config::Rules::BULLET_SPEED;
  this->color = color;
  this->SetModulate(GetModulateFromEnum(color));

  EventManager::Instance().Subscribe("onCollision", this, [this](std::any data) {
    if (!active) { return; }
    CollisionData collisionData = std::any_cast<CollisionData>(data);
    if (collisionData.first == this) {
      active = false;
      RemoveCollisionBox();
      EventManager::Instance().Emit("ballHit", collisionData);
    }
  });
}

void Bullet::OnAddedToScene() {
  SDL_FRect collisionRect = rect;
  collisionRect.w *= Game::Config::Rules::COLLIDER_SCALE;
  collisionRect.h *= Game::Config::Rules::COLLIDER_SCALE;
  AddCollisionBox(CollisionLayers::LAYER_2, CollisionLayers::LAYER_1, collisionRect);
}

void Bullet::OnExitScreen() {
  active = false;
  CollisionData data = {(Node *)this, nullptr};
  EventManager::Instance().Emit("ballHit", data);
}

void Bullet::Update(float deltaTime) {
  if (active) Move(deltaTime);
}

void Bullet::Draw(SDL_Renderer *renderer) { TextureManager::Instance().Draw("img_bubble", rect, GetModulate()); }
