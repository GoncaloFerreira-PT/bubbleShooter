#pragma once
#include "core/node/node.h"
#include "core/managers/textureManager.h"
#include "game/ball.h"
#include "game/bullet.h"
#include <any>
#include <core/utils/raycast.h>
#include "game/ballContainer.h"
#include "core/utils/math.h"


class Cannon : public Node
{
private:
    std::shared_ptr<BallContainer> ballContainer;


public:
    vec2 direction = {0.0f, -1.0f}; // Default to aiming up
    Bullet* currentBullet = nullptr;
    RayCast* rc = nullptr;
    bool blocked = false;

    Cannon(SDL_FRect rect);
    void OnAddedToScene() override;
    void Draw(SDL_Renderer *renderer) override;
    void HandleInput(SDL_Event *event) override;
    void Shoot();
    void LoadBullet(Color color);
    
    void OnBallCreatedCallback(std::any data);
};
