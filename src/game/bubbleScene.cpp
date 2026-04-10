#include "bubbleScene.h"
#include "game/ballContainer.h"
#include "core/node/label.h"
#include "core/node/button.h"
#include "game/inventory.h"
#include "game/cannon.h"
#include "combo.h"


void BubbleScene::Initialize(SDL_Renderer *renderer)
{
    SetRenderer(renderer);
    ResourceManager::Instance().Load();
    GameStateManager::Instance().ChangeState(GameState::Menu);
}


void BubbleScene::HandleInput(SDL_Event *event)
{
    Scene::HandleInput(event);
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.scancode == SDL_SCANCODE_D)
        {
            Debug::DRAW_COLLISIONS = !Debug::DRAW_COLLISIONS;
        }
    }
}


void BubbleScene::OnStateChangedCallback(std::any data)
{
    GameState state = std::any_cast<GameState>(data);

    switch (state)
    {
    case GameState::Menu:
    {
        ClearNodes();
        auto textLabel = std::make_shared<Label>(SDL_FRect{10, 10, 200, 30});
        textLabel->SetFont("resources/OpenSans-Regular.ttf", 24);
        textLabel->SetText("Bubble Shooter");
        AddNode(textLabel);

        auto buttonLabel = std::make_shared<Label>(SDL_FRect{10, 50, 100, 30});
        buttonLabel->SetFont("resources/OpenSans-Regular.ttf", 18);
        buttonLabel->SetText("Play");

        auto button = std::make_shared<Button>(SDL_FRect{10, 50, 100, 30}, []()
                                               { GameStateManager::Instance().ChangeState(GameState::Game); });

        button->SetModulate(SDL_Color{255, 0, 0, 255});
        button->SetLabel(buttonLabel);
        AddNode(button);
    }
    break;

    case GameState::Game:
    {
        ClearNodes();

        SDL_FRect boardRect = {0, 0, 640, 480};
        auto board = std::make_shared<Board>(boardRect);
        AddNode(board);
        // The grid of balls is 10 columns wide, with 36px spacing. A ball is 32px wide.
        // The total width is (COLUMNS - 1) * SPACING + ball_width
        const float ballGridWidth = (10 - 1) * 36.0f + 32.0f;
        const float ballContainerX = boardRect.x + (boardRect.w - ballGridWidth) / 2.0f;
        auto ballContainer = std::make_shared<BallContainer>(SDL_FRect{ballContainerX, 0.0f, ballGridWidth, 480.0f});
        AddNode(ballContainer);

        auto inventory = std::make_shared<Inventory>(SDL_FRect{32, 418, 224, 64});
        AddNode(inventory);

        const auto cannonRect = SDL_FRect{0, 420, 64, 64};
        const float cannonX = boardRect.x + (boardRect.w - cannonRect.w) / 2.0f;
        auto cannon = std::make_shared<Cannon>(SDL_FRect{cannonX, cannonRect.y, cannonRect.w, cannonRect.h});
        AddNode(cannon);

        auto combo = std::make_shared<Combo>();
        AddNode(combo);
    }
    break;
    default:
        Console::Error("Unknown state");
        break;
    }
}
