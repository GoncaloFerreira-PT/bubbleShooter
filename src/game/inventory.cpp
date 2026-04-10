#include "game/inventory.h"
#include <memory>
#include "core/managers/eventManager.h"

Inventory::Inventory(SDL_FRect rect) : MovableNode(rect)
{
    for (int i = 0; i < size; i++)
    {
        CreateBall();
    }
    EventManager::Instance().Subscribe("ballShot", [this](std::any data)
                                       { OnBallShotCallback(data); });
}


void Inventory::OnBallShotCallback(std::any _data)
{
    Ball* toBeShot = Pop();
    EventManager::Instance().Emit("ballCreated", toBeShot);
    animationData.animating = true;
}


void Inventory::Draw(SDL_Renderer *renderer)
{
    if (deque.size() == 0)
        return;

    const float width = deque[0]->rect.w;
    for (int i = 0; i < deque.size(); i++)
    {
        Ball *ball = deque[i];
        ball->SetPosition({(float)rect.x + (width * i) + ball->rect.w / 2.0f, rect.y + rect.h / 4.0f});
        ball->Draw(renderer);
    }
    // Draw the inventory texture at its final, fixed position.
    SDL_FRect texture_rect = rect;
    texture_rect.x = animationData.offset;
    TextureManager::Instance().Draw("img_inventory", texture_rect, modulate);
}


void Inventory::Update(float deltaTime){
    if(animationData.animating){
        const float width = deque[0]->rect.w;
        if(rect.x < animationData.offset + width){
            direction = {1, 0};
            speed = animationData.offset * deltaTime;
            Move();
        } else {
            // When the animation finishes, snap to final position and reset state.
            rect.x = animationData.offset;
            animationData.animating = false;
            CreateBall();
            deque.back()->Destroy();
            deque.pop_back();
        }
    }
}


void Inventory::Push(Ball *ball)
{
    deque.push_front(ball);
}


Ball* Inventory::CreateBall()
{
    Ball *ball = new Ball({-1,-1});
    ball->color = GetRandomColor();
    ball->SetModulate(GetModulateFromEnum(ball->color));
    Push(ball);
    return ball;
}


Ball *Inventory::Pop()
{
    if (deque.empty())
    {
        Console::Log("Deque is empty!");
        return nullptr;
    }
    Ball *ball = deque.back();
    return ball;
}