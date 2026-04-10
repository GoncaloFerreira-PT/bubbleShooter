#pragma once
#include "core/node/node.h"
#include "core/managers/eventManager.h"
#include <vector>

enum class GameState {
    Menu,
    Game,
};

class GameStateManager
{
private:
    std::vector<GameState *> states;
    GameState currentState;

public:
    static GameStateManager &Instance()
    {
        static GameStateManager instance;
        return instance;
    }

    void ChangeState(GameState state);
};
