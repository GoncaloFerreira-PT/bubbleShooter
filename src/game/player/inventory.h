#pragma once
#include "core/node/node.h"
#include "game/board/ball.h"
#include "game/bubbleGameConfig.h"
#include <SDL3/SDL.h>
#include <any>
#include <deque>

class Inventory : public MovableNode {
private:
  struct AnimationData {
    bool animating = false;
    float offset = Game::Config::Grid::BALL_SIZE.x;
    float speed = 200.0f;
  };

  AnimationData animationData;

  int size = 5;
  std::deque<std::shared_ptr<Ball>> deque;

public:
  Inventory(SDL_FRect rect);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void CreateBall();
  void Push(std::shared_ptr<Ball> ball);
  std::shared_ptr<Ball> Pop();
  void OnBallShotCallback(std::any data);
};