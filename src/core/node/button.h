#pragma once
#include "core/node/node.h"
#include "core/node/label.h"
#include <SDL3/SDL.h>


class Button: public Node
{
    private:
        std::shared_ptr<Label> label;
        bool hovered = false;
        std::function<void()> callback;

    public:
       Button(SDL_FRect rect, std::function<void()> callback) : Node(rect), callback(std::move(callback)) {}
       void Draw(SDL_Renderer *renderer) override;
       void HandleInput(SDL_Event *event) override;
       void SetLabel(std::shared_ptr<Label> newLabel);
       void OnClick();
       void OnEnter();
       void OnLeave();

};