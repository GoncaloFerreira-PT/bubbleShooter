#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "core/node/node.h"

class Node;

struct CollisionData
{
    Node* first;
    Node* second;
};  

class CollisionBox
{
private:
    const SDL_Color DEBUG_COLOR = {0, 191, 255, 120};

public:
    SDL_FRect rect;

    CollisionBox(SDL_FRect rect) : rect(rect) {}
    bool CheckCollision(const CollisionBox &other) const;
    void Draw(SDL_Renderer *renderer);
    void UpdateCollision(SDL_FRect parentRect);
};
