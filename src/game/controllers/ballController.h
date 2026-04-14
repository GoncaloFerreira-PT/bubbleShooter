#pragma once
#include "core/gameScene.h"
#include "core/node/node.h"
#include "game/board/ball.h"
#include "game/controllers/difficultyController.h"
#include "game/player/bullet.h"
#include <SDL3/SDL.h>
#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

class BallController : public Node {
private:
  std::unordered_map<vec2i, std::shared_ptr<Ball>> balls;

  DifficultyController *difficultyController = nullptr;

  int shots = 0;
  int scrolls = 0;
  bool isShowcase = false;

public:
  BallController(SDL_FRect rect, bool showcase);
  Ball *AddBallAt(Color color, vec2i position);
  std::shared_ptr<Ball> GetBallAt(vec2i position);
  void OnBallHitCallback(std::any data);

  void CreateNewLine();
  void GetSameColorBalls(Ball *ball, std::vector<std::shared_ptr<Ball>> &result);
  void GetIsolatedBalls(std::vector<std::shared_ptr<Ball>> &result);
  std::unordered_map<vec2i, Ball *> GetNeighbors(vec2i position);
  vec2 GridToWorld(vec2i gridPosition);
  vec2i WorldToGrid(vec2 worldPosition);
  vec2i GetBestPlacementPosition(Bullet *bullet, Ball *ball);

  void SetDifficultyController(DifficultyController *difficultyController) {
    if (this->difficultyController != nullptr) delete this->difficultyController;
    this->difficultyController = difficultyController;
  }
};
