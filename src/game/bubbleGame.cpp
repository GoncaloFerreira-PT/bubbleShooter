#include "game/bubbleGame.h"
#include "core/gameScene.h"
#include "core/managers/audioManager.h"
#include "core/managers/gameStateManager.h"
#include "core/managers/resourceManager.h"
#include "core/node/button.h"
#include "core/node/colorRect.h"
#include "core/node/label.h"
#include "core/node/toggleButton.h"
#include "core/utils/console.h"
#include "core/utils/debug.h"
#include "difficultyController.h"
#include "game/ballContainer.h"
#include "game/board.h"
#include "game/cannon.h"
#include "game/comboContainer.h"
#include "game/inventory.h"
#include <any>
#include <memory>
#include <string>

void BubbleGame::Initialize(SDL_Renderer *renderer) {
  SetRenderer(renderer);
  ResourceManager::Instance().Load();
  GameStateManager::Instance().ChangeState(GameState::Menu);
  AudioManager::Instance().PlaySound("snd_background");
}

void BubbleGame::HandleInput(SDL_Event *event) {
  GameScene::HandleInput(event);
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_D) { Debug::DRAW_COLLISIONS = !Debug::DRAW_COLLISIONS; }
  }
}

void BubbleGame::OnStateChangedCallback(std::any data) {
  auto payload = std::any_cast<GameStatePayload>(data);
  GameState state = payload.Get<GameState>("currentState");

  SDL_FRect screenSize = GameScene::GetRect();
  const float screenWidth = screenSize.w;
  const float screenHeight = screenSize.h;

  SDL_FRect muteButtonRect = {screenWidth - 64.0f, screenHeight - 42, 32, 32};

  switch (state) {
  case GameState::Menu: {
    ClearNodes();
    GameStateManager::Instance().GetPayload()->Clear();

    SDL_FRect boardRect = {0, 0, screenWidth, screenHeight};
    auto board = std::make_shared<Board>(boardRect);
    AddNode(board);

    auto ballContainer = std::make_shared<BallContainer>(SDL_FRect{52, 0.0f, 536, screenHeight}, true);
    AddNode(ballContainer);

    auto colorRect = std::make_shared<ColorRect>(SDL_FRect{0, 0, screenWidth, screenHeight}, SDL_Color{0, 0, 0, 128});
    AddNode(colorRect);

    auto logo = std::make_shared<TextureRect>(SDL_FRect{(screenWidth - 256.0f) / 2.0f, 60.0f, 256, 256}, "img_logo");
    AddNode(logo);

    auto button = std::make_shared<Button>(SDL_FRect{(screenWidth - 100.0f) / 2.0f, 300.0f, 100, 30}, [](bool pressed) {
      GameStateManager::Instance().ChangeState(GameState::Game);
    });
    button->SetTextures("img_button_up", "img_button_down");
    button->SetText("Play", "resources/OpenSans-Regular.ttf", 18);
    AddNode(button);

    auto muteButton = std::make_shared<ToggleButton>(muteButtonRect,
                                                     [](bool pressed) { AudioManager::Instance().SetMuted(pressed); });
    muteButton->SetTextures("img_unmuted", "img_muted");
    AddNode(muteButton);

  } break;

  case GameState::Game: {
    ClearNodes();

    SDL_FRect boardRect = {0, 0, 640, 480};
    auto board = std::make_shared<Board>(boardRect);
    AddNode(board);

    auto ballContainer = std::make_shared<BallContainer>(SDL_FRect{50, 0.0f, 536, 480.0f}, false);
    ballContainer->SetDifficultyController(new DifficultyController());
    AddNode(ballContainer);

    auto inventory = std::make_shared<Inventory>(SDL_FRect{32, 418, 224, 64});
    AddNode(inventory);

    auto cannon = std::make_shared<Cannon>(SDL_FRect{288, 420, 64, 64});
    AddNode(cannon);

    auto muteButton = std::make_shared<ToggleButton>(muteButtonRect,
                                                     [](bool pressed) { AudioManager::Instance().SetMuted(pressed); });
    muteButton->SetTextures("img_unmuted", "img_muted");
    AddNode(muteButton);

    auto combo = std::make_shared<ComboContainer>();
    AddNode(combo);

  } break;

  case GameState::EndGame: {
    ClearNodes();

    SDL_FRect boardRect = {0, 0, screenWidth, screenHeight};
    auto board = std::make_shared<Board>(boardRect);
    AddNode(board);

    auto ballContainer = std::make_shared<BallContainer>(SDL_FRect{50, 0.0f, 536, screenHeight}, true);
    AddNode(ballContainer);

    auto colorRect = std::make_shared<ColorRect>(SDL_FRect{0, 0, screenWidth, screenHeight}, SDL_Color{0, 0, 0, 128});
    AddNode(colorRect);

    auto textLabel = std::make_shared<Label>(SDL_FRect{0, 0, 0, 0});
    textLabel->SetFont("resources/OpenSans-Regular.ttf", 24);
    textLabel->SetText("Game Over!");
    textLabel->SetPosition({(screenWidth - textLabel->rect.w) / 2.0f, 150.0f});
    AddNode(textLabel);

    auto payload = GameStateManager::Instance().GetPayload();
    int totalWon = payload->Get<int>("totalPoints", 0);
    int bestWon = payload->Get<int>("bestPoints", 0);
    if (totalWon > bestWon) { payload->Set<int>("bestPoints", totalWon); }
    auto totalWonLabel = std::make_shared<Label>(SDL_FRect{0, 0, 0, 0});
    totalWonLabel->SetFont("resources/OpenSans-Regular.ttf", 24);
    totalWonLabel->SetText("Total Won: " + std::to_string(totalWon));
    totalWonLabel->SetPosition({(screenWidth - totalWonLabel->rect.w) / 2.0f, 200.0f});
    AddNode(totalWonLabel);

    auto button = std::make_shared<Button>(SDL_FRect{(screenWidth - 100.0f) / 2.0f, 240.0f, 100, 30}, [](bool pressed) {
      GameStateManager::Instance().ChangeState(GameState::Game);
    });
    button->SetTextures("img_button_up", "img_button_down");
    button->SetText("Restart", "resources/OpenSans-Regular.ttf", 18);
    AddNode(button);

    auto muteButton = std::make_shared<ToggleButton>(muteButtonRect,
                                                     [](bool pressed) { AudioManager::Instance().SetMuted(pressed); });
    muteButton->SetTextures("img_unmuted", "img_muted");
    muteButton->SetLayer(100);
    AddNode(muteButton);
  } break;
  default:
    Console::Error("Unknown state");
    break;
  }
}
