#include "core/node/button.h"
#include "core/utils/console.h"

void Button::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
    SDL_RenderFillRect(renderer, &rect);
    if (label)
    {
        // Center the label's texture inside the button
        label->rect.x = rect.x + (rect.w - label->rect.w) / 2.0f;
        label->rect.y = rect.y + (rect.h - label->rect.h) / 2.0f;
        label->Draw(renderer);
    }
}

void Button::HandleInput(SDL_Event *event)
{
    float mouseX = event->button.x;
    float mouseY = event->button.y;
    SDL_FPoint mousePoint = {mouseX, mouseY};
    if (SDL_PointInRectFloat(&mousePoint, &rect))
    {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            OnClick();
        }else if (!hovered){
            OnEnter();
        }
    }else if(hovered)
    {
        OnLeave();
    }
}

void Button::SetLabel(std::shared_ptr<Label> newLabel)
{
    label = newLabel;
}

void Button::OnClick()
{
    if(callback)
        callback();
}

void Button::OnEnter()
{
    hovered = true;
    Console::Log("Button Enter!");
}

void Button::OnLeave()
{
    hovered = false;
    Console::Log("Button Leave!");
}