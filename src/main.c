#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Uint64 last_step;
} AppState;

const int SDL_WINDOW_WIDTH = 640;
const int SDL_WINDOW_HEIGHT = 480;
const Uint64 STEP_RATE_IN_MILLISECONDS = 125;


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_SetAppMetadata("Bubble Shooter by Gonçalo Ferreira", "1.0", ""))
    {
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState *appState = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!appState)
    {
        return SDL_APP_FAILURE;
    }

    *appstate = appState;

    if (!SDL_CreateWindowAndRenderer("Bubble Shooter by Gonçalo Ferreira", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &appState->window, &appState->renderer))
    {
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(appState->renderer, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    appState->last_step = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *appState = (AppState *)appstate;

    // run game logic if we're at or past the time to run it.
    // if we're _really_ behind the time to run it, run it
    // several times.
    const Uint64 now = SDL_GetTicks();
    while ((now - appState->last_step) >= STEP_RATE_IN_MILLISECONDS)
    {
        appState->last_step += STEP_RATE_IN_MILLISECONDS;
    }

    SDL_SetRenderDrawColor(appState->renderer, 128, 128, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appState->renderer);
    SDL_RenderPresent(appState->renderer);
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
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


void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL)
    {
        AppState *appState = (AppState *)appstate;
        SDL_DestroyRenderer(appState->renderer);
        SDL_DestroyWindow(appState->window);
        SDL_free(appState);
    }
}