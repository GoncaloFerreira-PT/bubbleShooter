#pragma once
#include "collisionManager.h"
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/utils/console.h"
#include "core/utils/debug.h"
#include <SDL3/SDL_rect.h>
#include <vector>

using namespace std;

void CollisionManager::AddCollisionBox(const shared_ptr<CollisionBox> &box) { collisionBoxes.push_back(box); }

void CollisionManager::RemoveCollisionBox(weak_ptr<Node> owner) {
  for (const auto &box : collisionBoxes) {
    if (!box->owner.expired() && box->owner.lock() == owner.lock()) {
      box->pending_destruction = true;
      needsCleanup = true;
    }
  }
}

void CollisionManager::Draw(SDL_Renderer *renderer) {
  if (!Debug::DRAW_COLLISIONS) return;
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (const auto &box : collisionBoxes) {
    if (box->pending_destruction) { continue; }
    SDL_RenderRect(renderer, &box->rect);
  }
}

void CollisionManager::Update() {
  for (const auto &box : collisionBoxes) {
    if (box->owner.expired()) {
      box->pending_destruction = true;
      needsCleanup = true;
    }
  }

  auto currentBoxes = collisionBoxes;
  for (size_t i = 0; i < currentBoxes.size(); ++i) {
    auto a = currentBoxes[i];
    if (a->pending_destruction) continue;

    for (size_t j = i + 1; j < currentBoxes.size(); ++j) {
      auto b = currentBoxes[j];
      if (b->pending_destruction) continue;

      CheckCollision(a, b);
    }
  }

  if (needsCleanup) {
    collisionBoxes.erase(
        std::remove_if(collisionBoxes.begin(), collisionBoxes.end(),
                       [](const std::shared_ptr<CollisionBox> &box) { return box->pending_destruction; }),
        collisionBoxes.end());
    needsCleanup = false;
  }
}

void CollisionManager::CheckCollision(const shared_ptr<CollisionBox> &a, const shared_ptr<CollisionBox> &b) {

  bool a_interests_b = (a->mask & b->category) != 0;
  bool b_interests_a = (b->mask & a->category) != 0;
  if (a_interests_b || b_interests_a) {
    if (SDL_HasRectIntersectionFloat(&a->rect, &b->rect)) {
      if (a_interests_b) NotifyCollision(a, b);
      if (b_interests_a) NotifyCollision(b, a);
    }
  }
}

void CollisionManager::NotifyCollision(const shared_ptr<CollisionBox> &a, const shared_ptr<CollisionBox> &b) {
  auto ownerA = a->owner.lock();
  auto ownerB = b->owner.lock();
  if (ownerA && ownerB) {
    CollisionData data = {ownerA.get(), ownerB.get()};
    EventManager::Instance().Emit("onCollision", data);
  }
}
