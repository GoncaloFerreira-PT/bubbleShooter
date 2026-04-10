#include "gameStateManager.h"

void GameStateManager::ChangeState(GameState state)
{
    currentState = state;
    EventManager::Instance().Emit("stateChanged", state);
}
