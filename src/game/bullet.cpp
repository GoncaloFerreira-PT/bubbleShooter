#include "bullet.h"

void Bullet::OnExitScreen(){
    active = false;
    CollisionData data = {(Node *)this, nullptr};
    EventManager::Instance().Emit("ballHit", data);
}


void Bullet::Update(float deltaTime)
{
    if (active)
    {
        Move();
        // Check collision with bubbles
        for (auto node : collidables)
        {
            if (!node || node == this || node->IsPendingDestruction())
                continue;

            Ball *ball = dynamic_cast<Ball *>(node);
            if (ball && collisionBox && collisionBox->CheckCollision(*ball->collisionBox))
            {
                CollisionData data = {(Node *)this, (Node *)ball};
                EventManager::Instance().Emit("ballHit", data);
                return;
            }
        }
    }
    
    std::erase_if(collidables, [](Node *n)
                  { return !n || n->IsPendingDestruction(); });
}

void Bullet::Draw(SDL_Renderer *renderer)
{
    TextureManager::Instance().Draw("img_bubble", rect, modulate);
    if (Debug::DRAW_COLLISIONS && collisionBox)
    {
        collisionBox->Draw(renderer);
    }
}
