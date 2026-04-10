#pragma once
#include <SDL3/SDL.h>
#include "core/utils/math.h"
#include "core/scene.h"

using namespace Math;

class RayCast
{
    vec2 origin;
    vec2 direction;
    SDL_Color modulate = {255, 0, 0, 255};
    std::function<bool(const vec2 &)> checkCondition;

    const float MAX_DISTANCE = 1000.0f;
    const float STEP_SIZE = 5.0f;
    const float VISUAL_STEP_SIZE = 30.0f;

public:
    RayCast() {}
    RayCast(vec2 _origin, vec2 _direction) : origin(_origin), direction(_direction) {}
    void Update(vec2 _origin, vec2 _direction)
    {
        origin = _origin;
        direction = _direction;
    }
    void SetCheckCondition(std::function<bool(const vec2 &)> _checkCondition) { checkCondition = std::move(_checkCondition); }
    void Draw(SDL_Renderer *renderer)
    {
        if (!checkCondition)
            return;

        vec2 currentPos = origin;
        float collisionDist = MAX_DISTANCE;
        for (float dist = 0; dist < MAX_DISTANCE; dist += STEP_SIZE)
        {
            currentPos = origin + (direction * dist);

            if (checkCondition(currentPos) || !Scene::Instance().IsInsideScreen(currentPos))
            {
                collisionDist = dist;
                break;
            }
        }

        for (float dist = 0; dist < collisionDist; dist += VISUAL_STEP_SIZE)
        {
            vec2 dotPos = origin + (direction * dist);
            SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
            SDL_FRect dotRect = {dotPos.x - 3, dotPos.y - 3, 6, 6};
            SDL_RenderFillRect(renderer, &dotRect);
        }
    }
};