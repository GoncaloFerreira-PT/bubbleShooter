#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <functional>
#include "core/utils/math.h"
#include "core/utils/console.h"
#include "core/utils/debug.h"
#include "core/modifiers/collisionBox.h"
#include "core/scene.h"

using namespace Math;

class Scene;
class CollisionBox;

class Node
{

private:
    bool pendingDestruction = false;

public:
    SDL_FRect rect;
    vec2 scale = {1, 1};
    SDL_Color modulate = {255, 255, 255, 255};

    CollisionBox *collisionBox = nullptr;
    static std::vector<Node *> collidables;

    Node() { rect = {0, 0, 0, 0}; }
    Node(SDL_FRect rect) : rect(rect) {}

    virtual ~Node()
    {
        if (collisionBox)
            delete collisionBox;
    }
    virtual void Update(float deltaTime) {};
    virtual void Draw(SDL_Renderer *renderer) {};
    virtual void HandleInput(SDL_Event *event) {};

    virtual void OnAddedToScene() {};

    void Destroy()
    {
        pendingDestruction = true;
        OnDestroy();
    }
    virtual void OnDestroy() {};
    bool IsPendingDestruction() const { return pendingDestruction; }

    void SetModulate(SDL_Color color);
    void SetAlpha(int alpha);
    vec2 GetPosition();
    void SetPosition(vec2 position);
    vec2 GetCenter();
    void SetScale(vec2 scale);
    SDL_FRect GetScaledRect() const;
    void AddCollisionBox(CollisionBox *_collisionBox = nullptr);
    void RemoveCollisionBox();
    
};


class MovableNode : public Node
{

private:
    bool destroyOnExitScreen = true;

public:
    vec2 direction;
    float speed = 1.0f;

    MovableNode(SDL_FRect rect) : Node(rect) {}
    void Move();
    void SetDestroyOnExitScreen(bool value);
    virtual void OnExitScreen() {};
};