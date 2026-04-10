#pragma once
#include "core/node/node.h"


class Board: public Node
{

public:
    Board(const SDL_FRect &rect) : Node(rect) {}
    void Draw(SDL_Renderer *renderer) override{
        SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};