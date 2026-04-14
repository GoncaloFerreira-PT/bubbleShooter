#pragma once
#include "core/managers/gameStateManager.h"
#include "game/bubbleGameConfig.h"

class DifficultyController {
public:
  DifficultyController() {
    auto payload = GameStateManager::Instance().GetPayload();
    if (!payload) return;
    payload->Set<int>("difficulty.shots_to_scroll", Game::Config::Rules::START_SHOTS_TO_SCROLL);
    payload->Set<int>("difficulty.max_colors", Game::Config::Rules::START_BALL_COLORS);
  }

  // These conditions try to make the game escalate its difficulty, creating a sense of urgency and adreneline for the
  // player.
  void EvaluateDifficulty() {
    auto payload = GameStateManager::Instance().GetPayload();
    int totalScrolls = payload->Get<int>("totalScrolls", 0);
    int totalPoints = payload->Get<int>("totalPoints", 0);

    // Increse number of possible colors depending on how many scrolls have happened
    if (totalScrolls >= 20)
      payload->Set<int>("difficulty.max_colors", 5);
    else if (totalScrolls >= 4)
      payload->Set<int>("difficulty.max_colors", 4);

    // Make the scrolls happen more often the more points the player has
    if (totalPoints >= 20000)
      payload->Set<int>("difficulty.shots_to_scroll", 2);
    else if (totalPoints >= 5000)
      payload->Set<int>("difficulty.shots_to_scroll", 3);
  }
};