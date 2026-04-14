#include "gameScene.h"
#include "core/config/config.h"
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_assert.h>
#include <algorithm>
#include <functional>
#include <vector>

GameScene *GameScene::instance = nullptr;

GameScene::GameScene() {
  SDL_assert(instance == nullptr);
  instance = this;

  // Listen for state changes to trigger UI/Scene rebuilds
  EventManager::Instance().Subscribe("stateChanged", this, [this](std::any data) { OnStateChangedCallback(data); });
}

GameScene::~GameScene() {
  nodes.clear();
  pendingNodes.clear();
  EventManager::Instance().UnsubscribeAll(this);
  instance = nullptr;
}

GameScene &GameScene::Instance() {
  SDL_assert(instance != nullptr);
  return *instance;
}

void GameScene::Initialize(SDL_Renderer *renderer) {}

// --- Node Management ---

void GameScene::AddNode(std::shared_ptr<Node> node) {
  pendingNodes.push_back(node);
  RequestSort();
}

void GameScene::RemoveNode(std::shared_ptr<Node> node) {
  // Flag for destruction; the actual memory cleanup happens in Update()
  node->Destroy();
  RequestSort();
}

void GameScene::ClearNodes() {
  pendingNodes.clear();
  for (const auto &node : nodes) {
    node->Destroy();
  }
  // Ensure collision data is wiped immediately to prevent stale callbacks
  CollisionManager::Instance().Clear();
}

// --- Main Loop (Update & Draw) ---
void GameScene::Update(float deltaTime) {
  // 1. Process Logic for active nodes
  for (const auto &node : nodes) {
    if (!node->IsPendingDestruction()) { node->Update(deltaTime); }
  }

  CollisionManager::Instance().Update();

  // 2. Cleanup: Remove nodes flagged for destruction
  std::erase_if(nodes, [](const auto &node) { return node->IsPendingDestruction(); });

  // 3. Integration: Move nodes from the pending queue to the active scene
  while (!pendingNodes.empty()) {
    auto toAdd = std::move(pendingNodes);
    pendingNodes.clear();

    for (const auto &node : toAdd) {
      nodes.push_back(node);
      node->OnAddedToScene();
    }
  }

  // 4. Sorting: Ensure nodes are drawn in the correct Z-order
  if (!pendingNodes.empty() || nodesNeedSorting) {
    std::stable_sort(nodes.begin(), nodes.end(), [](const auto &a, const auto &b) {
      // Keep nodes scheduled for removal at the bottom logic-wise
      if (a->IsPendingDestruction() && !b->IsPendingDestruction()) return false;
      return a->GetLayer() < b->GetLayer();
    });
    nodesNeedSorting = false;
  }

  ProcessTimers(deltaTime);
}

void GameScene::Draw(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  // Draw active nodes in sorted order
  for (const auto &node : nodes) {
    if (!node->IsPendingDestruction()) { node->Draw(renderer); }
  }

  CollisionManager::Instance().Draw(renderer);

  SDL_RenderPresent(renderer);
}

void GameScene::HandleInput(SDL_Event *event) {
  /** * Traverse nodes in reverse order (back to front).
   * This ensures the topmost UI elements/nodes intercept input first.
   */
  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    if ((*it)->HandleInput(event)) break;
  }
}

void GameScene::CreateTimer(float seconds, std::function<void()> callback) {
  queuedTimers.push_back({seconds, callback});
}

void GameScene::ProcessTimers(float deltaTime) {
  // Tick active timers and execute callbacks upon expiration
  std::erase_if(activeTimers, [deltaTime](auto &timer) {
    timer.remainingTime_s -= deltaTime;
    if (timer.remainingTime_s <= 0) {
      timer.callback();
      return true;
    }
    return false;
  });

  // Merge new timers created during this frame
  if (!queuedTimers.empty()) {
    activeTimers.insert(activeTimers.end(), queuedTimers.begin(), queuedTimers.end());
    queuedTimers.clear();
  }
}

SDL_FRect GameScene::GetRect() {
  return {0, 0, float(Core::Config::Window::WIDTH), float(Core::Config::Window::HEIGHT)};
}