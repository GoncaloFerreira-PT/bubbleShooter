#include "ball.h"
#include "core/managers/textureManager.h"

void Ball::Animate(Ball::Animation type)
{
    animation = type;
    RemoveCollisionBox();
    if (type == Animation::Pop)
    {
        speed = 0.0f;
    }
    else if (type == Animation::Fall)
    {
        speed = 0.5f;
    }
}

vec2i Ball::GetGridPosition()
{
    return gridPosition;
}

void Ball::SetGridPosition(vec2i position)
{
    gridPosition = position;
}

void Ball::Draw(SDL_Renderer *renderer)
{
    SDL_FRect scaledRect = GetScaledRect();
    TextureManager::Instance().Draw("img_bubble", scaledRect, modulate);
    if (Debug::DRAW_COLLISIONS && collisionBox)
    {
        collisionBox->Draw(renderer);
    }
}

void Ball::Update(float deltaTime)
{
    if (animation == Animation::Pop)
    {
        SetAlpha(modulate.a -= deltaTime);
        SetScale(scale -= deltaTime);
        if (modulate.a == 0)
        {
            Destroy();
            return;
        }
    }
    else if (animation == Animation::Fall)
    {
        Move();
        SetAlpha(modulate.a -= deltaTime*0.2f);
        if (modulate.a == 0)
        {
            Destroy();
            return;
        }
    }
}