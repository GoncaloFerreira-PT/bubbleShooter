#include "gameStateManager.h"
#include "core/managers/eventManager.h"

void GameStateManager::ChangeState(GameState state) {
  if (currentState == state) return;
  currentState = state;

  payload.Set<GameState>("currentState", currentState);
  EventManager::Instance().Emit("stateChanged", payload);
}
