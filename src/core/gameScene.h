#pragma once
#include "core/node/node.h"
#include "core/utils/math.h"
#include <SDL3/SDL.h>
#include <any>
#include <functional>
#include <memory>
#include <vector>

using namespace Math;
class Node;

struct SceneTimer {
  float remainingTime_s;
  std::function<void()> callback;
};

class GameScene {
private:
  bool nodesNeedSorting = false;

  void RequestSort() { nodesNeedSorting = true; }

public:
  GameScene();
  virtual ~GameScene();

  static GameScene &Instance();

  SDL_Renderer *renderer;

  // Prevent copying
  GameScene(const GameScene &) = delete;
  GameScene &operator=(const GameScene &) = delete;

  void AddNode(std::shared_ptr<Node> node);
  void RemoveNode(std::shared_ptr<Node> node);
  void ClearNodes();

  virtual void Initialize(SDL_Renderer *renderer);
  virtual void Update(float deltaTime);

  virtual void Draw(SDL_Renderer *renderer);
  virtual void HandleInput(SDL_Event *event);
  virtual void OnStateChangedCallback(std::any data) {};

  void SetRenderer(SDL_Renderer *renderer) { this->renderer = renderer; }

  SDL_Renderer *GetRenderer() { return renderer; }

  void CreateTimer(float seconds, std::function<void()> callback);
  void ProcessTimers(float deltaTime);

  SDL_FRect GetRect();

  bool IsInsideScreen(SDL_FRect rect) {
    SDL_FRect screenRect = GetRect();
    return rect.x >= 0 && rect.x + rect.w <= screenRect.w && rect.y >= 0 && rect.y + rect.h <= screenRect.h;
  }

  bool IsInsideScreen(vec2 position) {
    SDL_FRect rect = {position.x, position.y, 0, 0};
    return IsInsideScreen(rect);
  }

  template <typename T> std::shared_ptr<T> GetFirstNodeOfType() {
    for (const auto &node : nodes) {
      std::shared_ptr<T> typedNode = std::dynamic_pointer_cast<T>(node);
      if (typedNode) { return typedNode; }
    }
    return nullptr;
  }

protected:
  static GameScene *instance;
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<std::shared_ptr<Node>> pendingNodes;

  std::vector<SceneTimer> activeTimers;
  std::vector<SceneTimer> queuedTimers;
};
