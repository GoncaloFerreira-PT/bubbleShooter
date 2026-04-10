#pragma once
#include "core/scene.h"
#include "core/managers/resourceManager.h"
#include "core/managers/textureManager.h"
#include "core/managers/gameStateManager.h"
#include "core/managers/audioManager.h"
#include <memory>
#include "game/ballContainer.h"
#include "game/board.h"



class BubbleScene : public Scene
{
public:
    BubbleScene() : Scene() {};
    void Initialize(SDL_Renderer* renderer) override;
    void OnStateChangedCallback(std::any data) override;
    void HandleInput(SDL_Event *event) override;
};