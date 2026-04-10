#pragma once
#include "core/node/node.h"
#include "game/ball.h"
#include "core/managers/textureManager.h"
#include "core/managers/eventManager.h"


class Bullet : public MovableNode
{
public:
    bool active = false;
    Color color;

    Bullet(SDL_FRect rect, Color color) : MovableNode(rect) {
        this->speed = 1.1f;
        this->color = color;
        this->SetModulate(GetModulateFromEnum(color));
    }
    void OnExitScreen() override;
    void Draw(SDL_Renderer *renderer) override;
    void Update(float deltaTime) override;
};