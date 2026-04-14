// node.h
#pragma once
#include "core/gameScene.h"
#include "core/managers/eventManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/utils/math.h"
#include <SDL3/SDL.h>
#include <memory>

using namespace Math;

class Node : public std::enable_shared_from_this<Node> {
protected:
  int layer = 0;
  vec2 scale = {1.0f, 1.0f};
  SDL_Color modulate = {255, 255, 255, 255};
  bool pendingDestruction = false;

public:
  SDL_FRect rect{};
  std::shared_ptr<CollisionBox> collisionBox = nullptr;

  Node() = default;

  Node(SDL_FRect rect) : rect(rect) {}

  virtual ~Node() {
    RemoveCollisionBox();
    EventManager::Instance().UnsubscribeAll(this);
  }

  virtual void Update(float deltaTime) {}

  virtual void Draw(SDL_Renderer *renderer) {}

  virtual bool HandleInput(SDL_Event *event) { return false; }

  virtual void OnAddedToScene() {}

  virtual void OnDestroy() {}

  void Destroy() {
    pendingDestruction = true;
    OnDestroy();
  }

  bool IsPendingDestruction() { return pendingDestruction; }

  // Transform logic
  void SetPosition(vec2 pos);
  vec2 GetPosition();

  vec2 GetCenter();

  void SetScale(vec2 newScale);

  vec2 GetScale() { return scale; }

  SDL_FRect GetScaledRect();

  // Appearance
  void SetLayer(int newLayer) { layer = newLayer; }

  int GetLayer() { return layer; }

  void SetModulate(SDL_Color color);

  SDL_Color GetModulate() { return modulate; }

  void SetAlpha(int alpha);

  int GetAlpha() { return modulate.a; }

  // Physics
  void AddCollisionBox(CollisionLayer category, CollisionLayer mask, SDL_FRect customRect = {0, 0, 0, 0});
  void RemoveCollisionBox();
};

class MovableNode : public Node {
protected:
  bool destroyOnExitScreen = true;

public:
  vec2 direction{0, 0};
  float speed = 0.0f;

  MovableNode(SDL_FRect rect) : Node(rect) {}

  void Move(float deltaTime);

  void SetDestroyOnExitScreen(bool value) { destroyOnExitScreen = value; }

  virtual void OnExitScreen() {}
};