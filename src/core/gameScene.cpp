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

void GameScene::AddNode(std::shared_ptr<Node> node) {
  pendingNodes.push_back(node);
  RequestSort();
}

void GameScene::RemoveNode(std::shared_ptr<Node> node) {
  node->Destroy();
  RequestSort();
}

void GameScene::ClearNodes() {
  pendingNodes.clear();
  for (const auto &node : nodes) {
    node->Destroy();
  }
  CollisionManager::Instance().Clear();
}

void GameScene::Update(float deltaTime) {
  // Update nodes
  for (const auto &node : nodes) {
    if (!node->IsPendingDestruction()) { node->Update(deltaTime); }
  }

  CollisionManager::Instance().Update();

  // Delete nodes that are pending destruction at the end of this frame
  std::erase_if(nodes, [](const auto &node) { return node->IsPendingDestruction(); });

  // Add pending nodes
  while (!pendingNodes.empty()) {
    auto toAdd = std::move(pendingNodes);
    pendingNodes.clear();

    for (const auto &node : toAdd) {
      nodes.push_back(node);
      node->OnAddedToScene();
    }
  }

  // Sort layers if needed
  if (!pendingNodes.empty() || nodesNeedSorting) {
    std::stable_sort(nodes.begin(), nodes.end(), [](const auto &a, const auto &b) {
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

  for (const auto &node : nodes) {
    if (!node->IsPendingDestruction()) { node->Draw(renderer); }
  }

  CollisionManager::Instance().Draw(renderer);
  SDL_RenderPresent(renderer);
}

void GameScene::HandleInput(SDL_Event *event) {
  // Traverse nodes in reverse order for input handling (topmost first)
  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    if ((*it)->HandleInput(event)) break;
  }
}

SDL_FRect GameScene::GetRect() {
  return {0, 0, float(Core::Config::Window::WIDTH), float(Core::Config::Window::HEIGHT)};
}

void GameScene::ProcessTimers(float deltaTime) {
  // Process active timers
  std::erase_if(activeTimers, [deltaTime](auto &timer) {
    timer.remainingTime_s -= deltaTime;
    if (timer.remainingTime_s <= 0) {
      timer.callback();
      return true;
    }
    return false;
  });

  // Add queued timers at the end of the frame
  if (!queuedTimers.empty()) {
    activeTimers.insert(activeTimers.end(), queuedTimers.begin(), queuedTimers.end());
    queuedTimers.clear();
  }
}

void GameScene::CreateTimer(float seconds, std::function<void()> callback) {
  queuedTimers.push_back({seconds, callback});
}