#include "core/modifiers/collisionBox.h"

void UpdateCollision(){

}


bool CollisionBox::CheckCollision(const CollisionBox &other) const
{
    return (rect.x < other.rect.x + other.rect.w &&
            rect.x + rect.w > other.rect.x &&
            rect.y < other.rect.y + other.rect.h &&
            rect.y + rect.h > other.rect.y);
}

void CollisionBox::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderRect(renderer, &rect);
}


void CollisionBox::UpdateCollision(SDL_FRect parentRect)
{
    float parentCenterX = parentRect.x + parentRect.w / 2;
    float parentCenterY = parentRect.y + parentRect.h / 2;

    rect.x = parentCenterX - rect.w / 2;
    rect.y = parentCenterY - rect.h / 2;
}