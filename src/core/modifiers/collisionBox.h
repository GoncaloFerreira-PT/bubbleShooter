#pragma once
#include <SDL3/SDL.h>
#include <memory>

class Node;
using CollisionLayer = uint32_t;

namespace CollisionLayers {
  static const uint32_t NONE = 0;
  static const uint32_t LAYER_1 = 1 << 0;
  static const uint32_t LAYER_2 = 1 << 1;
  static const uint32_t LAYER_3 = 1 << 2;
} // namespace CollisionLayers

class CollisionBox {

public:
  SDL_FRect rect;
  std::weak_ptr<Node> owner;
  CollisionLayer category = CollisionLayers::NONE;
  CollisionLayer mask = CollisionLayers::NONE;
  bool pending_destruction = false;
  CollisionBox(std::weak_ptr<Node> owner, CollisionLayer category, CollisionLayer mask);

  void UpdateRect(SDL_FRect parentRect);
};
