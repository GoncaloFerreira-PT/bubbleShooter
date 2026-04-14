#pragma once
#include "core/managers/gameStateManager.h"
#include "core/node/node.h"
#include "core/utils/math.h"
#include "game/bubbleGameConfig.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <array>
#include <cstddef>
#include <string>

enum Color : int { Red, Green, Blue, Yellow, Orange, Purple, Cyan, COUNT };

inline SDL_Color GetModulateFromEnum(Color col) {
  static constexpr std::array<SDL_Color, (size_t)Color::COUNT> colors = {{
      {249, 65, 68, 255},  // Red
      {106, 192, 80, 255}, // Green
      {45, 110, 240, 255}, // Blue
      {249, 199, 79, 255}, // Yellow
      {248, 150, 30, 255}, // Orange
      {155, 89, 182, 255}, // Purple
      {0, 188, 212, 255}   // Cyan
  }};

  size_t index = static_cast<size_t>(col);
  return (index < colors.size()) ? colors[index] : SDL_Color{255, 255, 255, 255};
}

inline Color GetRandomColor() {
  int currentMaxBallColors = GameStateManager::Instance().GetPayload()->Get<int>(
      "difficulty.max_colors", Game::Config::Rules::START_BALL_COLORS);
  return static_cast<Color>(SDL_rand(static_cast<int>(currentMaxBallColors)));
}

class Ball : public MovableNode {

public:
  enum Animation {
    Pop,
    Fall,
  };

private:
  vec2i gridPosition;
  Animation animation;

public:
  Color color;

  Ball(vec2i gridPosition) : MovableNode({0, 0, Game::Config::Grid::BALL_SIZE.x, Game::Config::Grid::BALL_SIZE.y}) {
    speed = 0.0f;
    direction = {0, 1};
    this->gridPosition = gridPosition;
  }

  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  vec2i GetGridPosition();
  void SetGridPosition(vec2i position);
  void Animate(Animation type);
};