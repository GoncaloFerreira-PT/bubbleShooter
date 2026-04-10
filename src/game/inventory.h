#pragma once
#include <deque>
#include "core/node/node.h"
#include <SDL3/SDL.h>
#include "game/ball.h"
#include <any>
#include "core/managers/textureManager.h"


class Inventory: public MovableNode
{
    private:

        struct AnimationData{
            bool animating = false;
            float offset = 32.0f;
        };

        AnimationData animationData;

        int size = 5;
        std::deque<Ball*> deque;

    public:
        Inventory(SDL_FRect rect);
        void Update(float deltaTime) override;
        void Draw(SDL_Renderer *renderer) override;
        void Animate();
        Ball* CreateBall();
        void Push(Ball* ball);
        Ball* Pop();
        void OnBallShotCallback(std::any data);
};