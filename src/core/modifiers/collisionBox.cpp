#include "core/modifiers/collisionBox.h"
#include "core/node/node.h"

CollisionBox::CollisionBox(std::weak_ptr<Node> owner, CollisionLayer category, CollisionLayer mask) {
  this->owner = owner;
  this->category = category;
  this->mask = mask;
  rect = owner.lock()->rect;
}

void CollisionBox::UpdateRect(SDL_FRect parentRect) {
  float parentCenterX = parentRect.x + parentRect.w / 2;
  float parentCenterY = parentRect.y + parentRect.h / 2;

  rect.x = parentCenterX - rect.w / 2;
  rect.y = parentCenterY - rect.h / 2;
}