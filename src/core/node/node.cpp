#include "core/node/node.h"


void Node::SetModulate(SDL_Color color)
{
    modulate = color;
}


void Node::SetAlpha(int alpha)
{
    modulate.a = std::clamp(alpha,0,255);
}


vec2 Node::GetPosition()
{
    return {rect.x, rect.y};
}


void Node::SetPosition(vec2 position)
{
    rect.x = position.x;
    rect.y = position.y;
}


vec2 Node::GetCenter()
{
    return {rect.x + rect.w / 2, rect.y + rect.h / 2};
}


void Node::SetScale(vec2 scale)
{
    this->scale.x = std::max(scale.x, 0.0f);
    this->scale.y = std::max(scale.y, 0.0f);
}


SDL_FRect Node::GetScaledRect() const
{
    SDL_FRect scaledRect = rect;
    scaledRect.w *= scale.x;
    scaledRect.h *= scale.y;
    scaledRect.x -= (scaledRect.w - rect.w) / 2.0f;
    scaledRect.y -= (scaledRect.h - rect.h) / 2.0f;
    return scaledRect;
}


void Node::AddCollisionBox(CollisionBox *_collisionBox)
{
    if (collisionBox)
    {
        Console::Log("Node already has a collision box!");
        return;
    }

    if (_collisionBox)
        collisionBox = _collisionBox;
    else
        collisionBox = new CollisionBox(rect);

    collidables.push_back(this);
}


void Node::RemoveCollisionBox()
{
    if (collisionBox)
    {
        delete collisionBox;
        collisionBox = nullptr;
    }
}


void MovableNode::Move()
{
    float deltaX = direction.x * speed;
    float deltaY = direction.y * speed;

    rect.x += deltaX;
    rect.y += deltaY;

    if (collisionBox)
    {
        collisionBox->UpdateCollision(rect);
    }

    if (destroyOnExitScreen)
    {
        SDL_FRect screenRect = Scene::Instance().GetRect();
        if (rect.x + rect.w < 0 || rect.x > screenRect.w || rect.y + rect.h < 0 || rect.y > screenRect.h)
        {
            OnExitScreen();
            Destroy();
        }
    }
}


void MovableNode::SetDestroyOnExitScreen(bool value)
{
    destroyOnExitScreen = value;
}