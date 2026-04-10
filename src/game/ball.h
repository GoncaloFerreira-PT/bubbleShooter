#pragma once
#include <SDL3/SDL.h>
#include "core/node/node.h"

class Ball : public MovableNode
{

public:
    enum Animation
    {
        Pop,
        Fall,
    };

private:
    vec2i gridPosition;
    Animation animation;

public:
    Color color;

    Ball(vec2i gridPosition)
        : MovableNode({0, 0, 32, 32})
    {
        speed = 0.0f;
        direction = {0, 1};
        this->gridPosition = gridPosition;
    }
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer *renderer) override;
    vec2i GetGridPosition();
    void SetGridPosition(vec2i position);
    void Animate(Animation type);
};