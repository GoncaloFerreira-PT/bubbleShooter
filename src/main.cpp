#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "core/managers/textureManager.h"
#include "core/utils/console.h"
#include "game/bubbleScene.h"
#include <memory>


struct AppState
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    BubbleScene *scene = nullptr;
    Uint64 lastTime = 0;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    Console::Log("Starting application: Bubble Shooter by Gonçalo Ferreira");
    if (!SDL_SetAppMetadata("Bubble Shooter by Gonçalo Ferreira", "1.0", ""))
    {
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        Console::Error("Couldn't initialize SDL");
        return SDL_APP_FAILURE;
    }

    Console::Log("SDL initialized successfully");

    if(!TTF_Init())
    {
        Console::Error("Couldn't initialize SDL_ttf");
        return SDL_APP_FAILURE;
    }

    AppState *appState = new AppState();
    *appstate = appState;
    if (!appState)
    {
        Console::Error("Failed to allocate AppState.");
        return SDL_APP_FAILURE;
    }
    appState->lastTime = SDL_GetTicksNS();
    appState->scene = new BubbleScene();

    if (!SDL_CreateWindowAndRenderer("Bubble Shooter by Gonçalo Ferreira", appState->scene->WIDTH, appState->scene->HEIGHT, SDL_WINDOW_RESIZABLE, &appState->window, &appState->renderer))
    {
        Console::Error("Couldn't create window and renderer: " + std::string(SDL_GetError()));
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(appState->renderer, appState->scene->WIDTH, appState->scene->HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    TextureManager::Instance().SetRenderer(appState->renderer);
    appState->scene->Initialize(appState->renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *appState = (AppState *)appstate;

    // Calculate deltaTime
    Uint64 currentTime = SDL_GetTicksNS();
    Uint64 deltaNS = currentTime - appState->lastTime;
    float deltaTime = (float)deltaNS / 1000000000.0f;
    appState->lastTime = currentTime;

    appState->scene->Update(deltaTime);
    appState->scene->Draw(appState->renderer); 
    return SDL_APP_CONTINUE;
}

static SDL_AppResult handle_key_event_(SDL_Scancode key_code)
{
    switch (key_code)
    {
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
        return SDL_APP_SUCCESS;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *appState = (AppState *)appstate;
    if (appState && appState->scene)
    {
        appState->scene->HandleInput(event);
    }

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        return handle_key_event_(event->key.scancode);
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL)
    {
        AppState *appState = (AppState *)appstate;
        if (appState->scene) {
            //delete appState->scene; 
            appState->scene = nullptr;
        }
        if (appState->renderer) SDL_DestroyRenderer(appState->renderer);
        if (appState->window) SDL_DestroyWindow(appState->window);
        delete appState;
    }
    TTF_Quit();
    SDL_Quit();
}