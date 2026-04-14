#pragma once
#include "core/modifiers/collisionBox.h"
#include <SDL3/SDL_rect.h>
#include <memory>
#include <vector>
using namespace std;

struct CollisionData {
  Node *first;
  Node *second;
};

class CollisionManager {
private:
  bool needsCleanup = false;
  const SDL_Color DEBUG_COLOR = {0, 191, 255, 120};

public:
  static CollisionManager &Instance() {
    static CollisionManager instance;
    return instance;
  }

  void AddCollisionBox(const shared_ptr<CollisionBox> &box);
  void RemoveCollisionBox(weak_ptr<Node> owner);
  void UpdateCollisions();
  void Draw(SDL_Renderer *renderer);
  void UpdateCollision(SDL_FRect parentRect);
  void Update();
  void CheckCollision(const shared_ptr<CollisionBox> &a, const shared_ptr<CollisionBox> &b);
  void NotifyCollision(const shared_ptr<CollisionBox> &a, const shared_ptr<CollisionBox> &b);

  void Clear() {
    collisionBoxes.clear();
    needsCleanup = false;
  }

private:
  vector<shared_ptr<CollisionBox>> collisionBoxes;
};