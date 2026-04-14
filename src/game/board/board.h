#pragma once
#include "core/managers/collisionManager.h"
#include "core/managers/eventManager.h"
#include "core/managers/gameStateManager.h"
#include "core/managers/textureManager.h"
#include "core/modifiers/collisionBox.h"
#include "core/node/node.h"
#include <SDL3/SDL.h>

class Board : public Node {

private:
  SDL_FRect limitRect;

public:
  Board(const SDL_FRect &rect) : Node(rect) { limitRect = {0, 400, 680, 12}; }

  void OnAddedToScene() override {
    AddCollisionBox(CollisionLayers::LAYER_3, CollisionLayers::NONE, limitRect);
    collisionBox->UpdateRect(limitRect);
    EventManager::Instance().Subscribe("onCollision", this, [this](std::any data) {
      CollisionData collisionData = std::any_cast<CollisionData>(data);
      if (collisionData.first == this || collisionData.second == this) {
        collisionData.second->Destroy();
        GameStateManager::Instance().ChangeState(GameState::EndGame);
      }
    });
  }

  void Draw(SDL_Renderer *renderer) override { TextureManager::Instance().Draw("img_background", rect); }
};