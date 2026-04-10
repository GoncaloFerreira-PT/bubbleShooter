#include "scene.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_assert.h>
#include <vector>
#include <functional>

Scene *Scene::instance = nullptr;

Scene::Scene()
{
    SDL_assert(instance == nullptr);
    instance = this;

    EventManager::Instance().Subscribe("stateChanged", [this](std::any data)
                                       { OnStateChangedCallback(data); });
}

Scene::~Scene()
{
    nodes.clear();
    pendingNodes.clear();
    instance = nullptr;
}

Scene &Scene::Instance()
{
    SDL_assert(instance != nullptr);
    return *instance;
}

void Scene::Initialize(SDL_Renderer *renderer) {}

void Scene::AddNode(std::shared_ptr<Node> node)
{
    pendingNodes.push_back(node);
}

void Scene::ClearNodes()
{
    for (const auto &node : nodes)
    {
        node->Destroy();
    }
}

void Scene::Update(float deltaTime)
{
    // Update nodes
    for (const auto &node : nodes)
    {
        if (!node->IsPendingDestruction())
        {
            node->Update(deltaTime);
        }
    }

    // Delete nodes that are pending destruction at the end of this frame
    std::erase_if(nodes, [](const auto &node)
                  { return node->IsPendingDestruction(); });

    // Add pending nodes
    while (!pendingNodes.empty())
    {
        auto toAdd = std::move(pendingNodes);
        pendingNodes.clear();

        for (const auto &node : toAdd)
        {
            nodes.push_back(node);
            node->OnAddedToScene();
        }
    }

    ProcessTimers();
}

void Scene::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (const auto &node : nodes)
    {
        if (!node->IsPendingDestruction())
        {
            node->Draw(renderer);
        }
    }

    SDL_RenderPresent(renderer);
}

void Scene::HandleInput(SDL_Event *event)
{
    for (const auto &node : nodes)
    {
        node->HandleInput(event);
    }
}

void Scene::ProcessTimers()
{

    // Process active timers
    float currentTime = SDL_GetTicks();
    std::erase_if(activeTimers, [currentTime](auto &a)
                  {
        if (currentTime >= a.time) {
            a.callback();
            return true;
        }
        return false; });

    // Add queued timers at the end of the frame
    if (!queuedTimers.empty())
    {
        activeTimers.insert(activeTimers.end(), queuedTimers.begin(), queuedTimers.end());
        queuedTimers.clear();
    }
}

void Scene::CreateTimer(float seconds, std::function<void()> callback)
{
    float time_s = (float)SDL_GetTicks() + (seconds * 1000.0f);
    queuedTimers.push_back({time_s, callback});
}