#pragma once
#include "core/gameScene.h"
#include "core/managers/eventManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/utils/console.h"
#include "core/utils/math.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <memory>

using namespace Math;

class Node : public std::enable_shared_from_this<Node> {

private:
  bool pendingDestruction = false;
  int layer = 0;
  vec2 scale = {1, 1};
  SDL_Color modulate = {255, 255, 255, 255};

public:
  SDL_FRect rect;

  std::shared_ptr<CollisionBox> collisionBox = nullptr;

  Node() { Node(SDL_FRect{0, 0, 0, 0}); }

  Node(SDL_FRect rect) : rect(rect) {}

  virtual ~Node() {
    RemoveCollisionBox();
    EventManager::Instance().UnsubscribeAll(this);
  }

  virtual void Update(float deltaTime) {};
  virtual void Draw(SDL_Renderer *renderer) {};

  virtual bool HandleInput(SDL_Event *event) { return false; };

  virtual void OnAddedToScene() {};

  void Destroy() {
    pendingDestruction = true;
    OnDestroy();
  }

  virtual void OnDestroy() {};

  bool IsPendingDestruction() const { return pendingDestruction; }

  void SetLayer(int newLayer) {
    if (layer != newLayer) { layer = newLayer; }
  }

  int GetLayer() const { return layer; }

  void SetModulate(SDL_Color color);

  SDL_Color GetModulate() { return modulate; }

  void SetAlpha(int alpha);

  int GetAlpha() { return modulate.a; }

  void SetPosition(vec2 position);
  vec2 GetPosition();

  vec2 GetCenter();
  void SetScale(vec2 scale);

  vec2 GetScale() { return scale; }

  SDL_FRect GetScaledRect() const;

  void AddCollisionBox(CollisionLayer category, CollisionLayer mask, SDL_FRect customRect = {0, 0, 0, 0});
  void RemoveCollisionBox();
};

class MovableNode : public Node {

private:
  bool destroyOnExitScreen = true;

public:
  vec2 direction;
  float speed = 0.0f;

  MovableNode(SDL_FRect rect) : Node(rect) {}

  void Move(float deltaTime);
  void SetDestroyOnExitScreen(bool value);
  virtual void OnExitScreen() {};
};