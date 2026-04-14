#pragma once
#include "bubbleGameConfig.h"
#include "core/managers/gameStateManager.h"
#include "game/bubbleGameConfig.h"

class DifficultyController {
public:
  void Initialize() {
    auto payload = GameStateManager::Instance().GetPayload();
    payload->Set<int>("difficulty.shots_to_scroll", Game::Config::Rules::START_SHOTS_TO_SCROLL);
    payload->Set<int>("difficulty.max_colors", Game::Config::Rules::START_BALL_COLORS);
  }

  void EvaluateDifficulty() {
    auto payload = GameStateManager::Instance().GetPayload();
    int totalScrolls = payload->Get<int>("totalScrolls", 0);
    int totalPoints = payload->Get<int>("totalPoints", 0);

    // Aumenta as cores aos 2 e 5 scrolls
    if (totalScrolls >= 20)
      payload->Set<int>("difficulty.max_colors", 5);
    else if (totalScrolls >= 4)
      payload->Set<int>("difficulty.max_colors", 4);

    // O teto desce mais rápido aos 5000 pontos
    if (totalPoints >= 20000)
      payload->Set<int>("difficulty.shots_to_scroll", 2);
    else if (totalPoints >= 5000)
      payload->Set<int>("difficulty.shots_to_scroll", 3);
  }
};