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

/**
 * Manages lifecycle of SceneTimers, used for callbacks after a certain ammount of time in seconds
 */
struct SceneTimer {
  float remainingTime_s;
  std::function<void()> callback;
};

class GameScene {
public:
  // --- Constructor / Destructor ---
  GameScene();
  virtual ~GameScene();

  // --- Singleton Access ---
  static GameScene &Instance();

  // Disable copy and assignment for the Singleton/Base scene
  GameScene(const GameScene &) = delete;
  GameScene &operator=(const GameScene &) = delete;

  // --- Core Lifecycle Overrides ---
  virtual void Initialize(SDL_Renderer *renderer);
  virtual void Update(float deltaTime);
  virtual void Draw(SDL_Renderer *renderer);
  virtual void HandleInput(SDL_Event *event);
  virtual void OnStateChangedCallback(std::any data) {};

  // --- Node Management ---
  /** Adds a node to the scene. Usually processed at the end of the frame. */
  void AddNode(std::shared_ptr<Node> node);

  /** Removes a specific node from the scene. */
  void RemoveNode(std::shared_ptr<Node> node);

  /** Completely clears all nodes from the current scene. */
  void ClearNodes();

  /** Returns the first node found that matches type T. */
  template <typename T> std::shared_ptr<T> GetFirstNodeOfType() {
    for (const auto &node : nodes) {
      std::shared_ptr<T> typedNode = std::dynamic_pointer_cast<T>(node);
      if (typedNode) { return typedNode; }
    }
    return nullptr;
  }

  // --- Timer System ---
  /** Schedules a callback to be executed after a delay. */
  void CreateTimer(float seconds, std::function<void()> callback);

  /** Updates and triggers scheduled timers. */
  void ProcessTimers(float deltaTime);

  // --- Getters & Setters ---
  void SetRenderer(SDL_Renderer *renderer) { this->renderer = renderer; }

  SDL_Renderer *GetRenderer() { return renderer; }

  /** Returns the dimensions of the game screen/window. */
  SDL_FRect GetRect();

  // --- Utility Methods ---
  bool IsInsideScreen(SDL_FRect rect) {
    SDL_FRect screenRect = GetRect();
    return rect.x >= 0 && rect.x + rect.w <= screenRect.w && rect.y >= 0 && rect.y + rect.h <= screenRect.h;
  }

  bool IsInsideScreen(vec2 position) { return IsInsideScreen({position.x, position.y, 0, 0}); }

protected:
  static GameScene *instance;

  // Node Storage
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<std::shared_ptr<Node>> pendingNodes;

  // Timer Storage
  std::vector<SceneTimer> activeTimers;
  std::vector<SceneTimer> queuedTimers;

  SDL_Renderer *renderer;

private:
  bool nodesNeedSorting = false;

  void RequestSort() { nodesNeedSorting = true; }
};