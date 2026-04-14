#include "core/node/node.h"
#include "core/managers/collisionManager.h"
#include "core/utils/console.h"
#include <algorithm>
#include <memory>

using std::make_shared;

void Node::SetModulate(SDL_Color color) { modulate = color; }

void Node::SetAlpha(int alpha) { modulate.a = std::clamp(alpha, 0, 255); }

vec2 Node::GetPosition() { return {rect.x, rect.y}; }

void Node::SetPosition(vec2 position) {
  rect.x = position.x;
  rect.y = position.y;
}

vec2 Node::GetCenter() { return {rect.x + rect.w / 2, rect.y + rect.h / 2}; }

void Node::SetScale(vec2 newScale) {
  float baseWidth = rect.w / scale.x;
  float baseHeight = rect.h / scale.y;

  newScale.x = std::max(newScale.x, 0.0f);
  newScale.y = std::max(newScale.y, 0.0f);

  float nextWidth = baseWidth * newScale.x;
  float nextHeight = baseHeight * newScale.y;

  // Scale around center pivot
  rect.x -= (nextWidth - rect.w) * 0.5f;
  rect.y -= (nextHeight - rect.h) * 0.5f;

  rect.w = nextWidth;
  rect.h = nextHeight;

  this->scale = newScale;
}

SDL_FRect Node::GetScaledRect() {
  SDL_FRect scaledRect = rect;
  scaledRect.w *= scale.x;
  scaledRect.h *= scale.y;
  scaledRect.x -= (scaledRect.w - rect.w) / 2.0f;
  scaledRect.y -= (scaledRect.h - rect.h) / 2.0f;
  return scaledRect;
}

void Node::AddCollisionBox(CollisionLayer category, CollisionLayer mask, SDL_FRect customRect) {
  if (this->weak_from_this().expired()) {
    Console::Log("Erro: AddCollisionBox chamado antes do Node estar num shared_ptr!");
    return;
  }

  collisionBox = make_shared<CollisionBox>(weak_from_this(), category, mask);

  if (customRect.w > 0 && customRect.h > 0) {
    collisionBox->rect.w = customRect.w;
    collisionBox->rect.h = customRect.h;
  }

  collisionBox->UpdateRect(this->rect);
  CollisionManager::Instance().AddCollisionBox(collisionBox);
}

void Node::RemoveCollisionBox() { CollisionManager::Instance().RemoveCollisionBox(weak_from_this()); }

void MovableNode::Move(float deltaTime) {
  float deltaX = direction.x * speed * deltaTime;
  float deltaY = direction.y * speed * deltaTime;

  rect.x += deltaX;
  rect.y += deltaY;

  if (collisionBox) collisionBox->UpdateRect(rect);
  if (destroyOnExitScreen) {
    SDL_FRect screenRect = GameScene::Instance().GetRect();
    if (rect.x + rect.w < 0 || rect.x > screenRect.w || rect.y + rect.h < 0 || rect.y > screenRect.h) {
      OnExitScreen();
      Destroy();
    }
  }
}