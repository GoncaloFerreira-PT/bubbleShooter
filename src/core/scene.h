#pragma once
#include "core/node/node.h"
#include "core/utils/math.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <SDL3/SDL.h>
#include <any>
#include "core/managers/eventManager.h"

struct SceneTimer
{
    float time;
    std::function<void()> callback;
};

class Scene
{
public:
    Scene();
    virtual ~Scene();

    static Scene &Instance();

    const int WIDTH = 640;
    const int HEIGHT = 480;

    SDL_Renderer *renderer;

    // Prevent copying
    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    void AddNode(std::shared_ptr<Node> node);
    void RemoveNode(std::shared_ptr<Node> node);
    void ClearNodes();

    virtual void Initialize(SDL_Renderer *renderer);
    virtual void Update(float deltaTime);

    virtual void Draw(SDL_Renderer *renderer);
    virtual void HandleInput(SDL_Event *event);
    virtual void OnStateChangedCallback(std::any data) {};

    void SetRenderer(SDL_Renderer *renderer) { this->renderer = renderer; }
    SDL_Renderer *GetRenderer() { return renderer; }

    void CreateTimer(float seconds, std::function<void()> callback);
    void ProcessTimers();

    SDL_FRect GetRect()
    {
        return {0, 0, float(WIDTH), float(HEIGHT)};
    }

    bool IsInsideScreen(SDL_FRect rect)
    {
        SDL_FRect screenRect = GetRect();
        return rect.x >= 0 && rect.x + rect.w <= screenRect.w && rect.y >= 0 && rect.y + rect.h <= screenRect.h;
    }

    bool IsInsideScreen(vec2 position)
    {
        SDL_FRect rect = {position.x, position.y, 0, 0};
        return IsInsideScreen(rect);
    }

    template <typename T>
    std::shared_ptr<T> GetFirstNodeOfType()
    {
        for (auto &node : nodes)
        {
            std::shared_ptr<T> typedNode = std::dynamic_pointer_cast<T>(node);
            if (typedNode)
            {
                return typedNode;
            }
        }
        return nullptr;
    }

protected:
    static Scene *instance;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> pendingNodes;

    std::vector<SceneTimer> activeTimers;
    std::vector<SceneTimer> queuedTimers;
};
