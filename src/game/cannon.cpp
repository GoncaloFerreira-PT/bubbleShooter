#include "cannon.h"
#include "game/bubbleScene.h"
#include "core/managers/eventManager.h"
#include "ball.h"
#include "core/node/node.h"
#include "core/modifiers/collisionBox.h"

std::vector<Node *> Node::collidables;

Cannon::Cannon(SDL_FRect rect) : Node(rect)
{
    rc = new RayCast();
    rc->SetCheckCondition([this](const vec2 &pos)
                          {
                            if (!ballContainer) return false;
                            vec2i gridPos = ballContainer->WorldToGrid(pos - vec2(16,16));
                            auto ball = ballContainer->GetBallAt(gridPos);
                            return (ball && !ball->IsPendingDestruction()); });

    EventManager::Instance().Subscribe("ballCreated", [this](std::any data)
                                       { OnBallCreatedCallback(data); });

    EventManager::Instance().Subscribe("ballHit", [this](std::any data)
                                       { this->blocked = false; });
}

void Cannon::OnBallCreatedCallback(std::any data)
{
    auto ball = std::any_cast<Ball *>(data);
    LoadBullet(ball->color);
}

void Cannon::OnAddedToScene()
{
    ballContainer = Scene::Instance().GetFirstNodeOfType<BallContainer>();
    LoadBullet(GetRandomColor());
}


void Cannon::Shoot()
{
    blocked = true;
    if (currentBullet && !currentBullet->active)
    {
        currentBullet->active = true;
        currentBullet->direction = direction;
        currentBullet = nullptr;
        EventManager::Instance().Emit("ballShot", currentBullet);
    }
}

void Cannon::LoadBullet(Color color)
{
    if (currentBullet)
        return;
    auto bullet = std::make_shared<Bullet>(SDL_FRect{rect.x + 16, rect.y + 16, 32, 32}, (Color)color);
    SDL_FRect collisionRect = rect;
    collisionRect.w *= 0.8f;
    collisionRect.h *= 0.8f;
    bullet->AddCollisionBox(new CollisionBox(collisionRect));
    bullet->collisionBox->UpdateCollision(bullet->rect);
    currentBullet = bullet.get();
    Scene::Instance().AddNode(bullet);
}

void Cannon::Draw(SDL_Renderer *renderer)
{
    // Draw the aiming raycast
    vec2 center = GetCenter();
    rc->Update(center, direction);
    rc->Draw(renderer);

    float angle = Math::RadToDeg(std::atan2(direction.y, direction.x)) + 90.0f;
    TextureManager::Instance().DrawRotated("img_cannon", rect, modulate, angle, SDL_FPoint{rect.w / 2, rect.h / 2}, SDL_FLIP_NONE);
}

void Cannon::HandleInput(SDL_Event *event)
{
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        SDL_Renderer *renderer = Scene::Instance().GetRenderer();

        float logicalMouseX, logicalMouseY;

        SDL_RenderCoordinatesFromWindow(renderer,
                                        event->motion.x,
                                        event->motion.y,
                                        &logicalMouseX,
                                        &logicalMouseY);

        vec2 center = GetCenter();
        vec2 targetVector = {logicalMouseX - center.x, logicalMouseY - center.y};
        direction = targetVector.normalized();
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (currentBullet && !blocked)
            Shoot();
    }
}
