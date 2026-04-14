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
#include "game/board/board.h"
#include "game/controllers/ballController.h"
#include "game/controllers/difficultyController.h"
#include "game/player/cannon.h"
#include "game/player/comboContainer.h"
#include "game/player/inventory.h"
#include <any>
#include <memory>
#include <string>

// --- Initialization & Input ---

void BubbleGame::Initialize(SDL_Renderer *renderer) {
  SetRenderer(renderer);
  ResourceManager::Instance().Load();

  // Start in Menu state and trigger background music
  GameStateManager::Instance().ChangeState(GameState::Menu);
  AudioManager::Instance().PlaySound("snd_background", true);
}

void BubbleGame::HandleInput(SDL_Event *event) {
  GameScene::HandleInput(event);

  // Toggle collision debug view globally
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_D) { Debug::DRAW_COLLISIONS = !Debug::DRAW_COLLISIONS; }
  }
}

// --- State Management ---

void BubbleGame::OnStateChangedCallback(std::any data) {
  auto payload = std::any_cast<GameStatePayload>(data);
  GameState state = payload.Get<GameState>("currentState");

  // All states start by clearing previous UI/Nodes
  ClearNodes();

  switch (state) {
  case GameState::Menu:
    SetupMenuState();
    break;
  case GameState::Game:
    SetupGameState();
    break;
  case GameState::EndGame:
    SetupEndGameState();
    break;
  default:
    Console::Error("Unknown state encountered in OnStateChangedCallback");
    break;
  }
}

// --- State Setup Helpers ---

void BubbleGame::SetupMenuState() {
  SDL_FRect screen = GameScene::GetRect();

  // Background: Board + Animated Ball Container (Showcase mode)
  AddNode(std::make_shared<Board>(screen));
  AddNode(std::make_shared<BallController>(SDL_FRect{52, 0.0f, 536, screen.h}, true));

  // Dim the background for UI clarity
  AddNode(std::make_shared<ColorRect>(screen, SDL_Color{0, 0, 0, 128}));

  // Logo
  AddNode(std::make_shared<TextureRect>(SDL_FRect{(screen.w - 256.0f) / 2.0f, 60.0f, 256, 256}, "img_logo"));

  // Play Button
  auto playBtn = std::make_shared<Button>(SDL_FRect{(screen.w - 100.0f) / 2.0f, 300.0f, 100, 30}, [](bool) {
    GameStateManager::Instance().ChangeState(GameState::Game);
    AudioManager::Instance().PlaySound("snd_click");
  });
  playBtn->SetTextures("img_button_up", "img_button_down");
  playBtn->SetText("Play", "resources/OpenSans-Regular.ttf", 18);
  AddNode(playBtn);

  AddMuteButton();
}

void BubbleGame::SetupGameState() {
  // Gameplay Board and Ball Logic
  AddNode(std::make_shared<Board>(SDL_FRect{0, 0, 640, 480}));

  DifficultyController *difficultyController = new DifficultyController();
  auto ballController = std::make_shared<BallController>(SDL_FRect{50, 0.0f, 536, 480.0f}, false);
  ballController->SetDifficultyController(difficultyController);
  AddNode(ballController);

  // Player Elements
  AddNode(std::make_shared<Inventory>(SDL_FRect{32, 418, 224, 64}));
  AddNode(std::make_shared<Cannon>(SDL_FRect{288, 420, 64, 64}));
  AddNode(std::make_shared<ComboContainer>());

  AddMuteButton();
}

void BubbleGame::SetupEndGameState() {
  SDL_FRect screen = GameScene::GetRect();

  // Static background elements
  AddNode(std::make_shared<Board>(screen));
  AddNode(std::make_shared<BallController>(SDL_FRect{50, 0.0f, 536, screen.h}, true));
  AddNode(std::make_shared<ColorRect>(screen, SDL_Color{0, 0, 0, 128}));

  // Game Over Header
  auto title = std::make_shared<Label>(SDL_FRect{0, 0, 0, 0});
  title->SetFont("resources/OpenSans-Regular.ttf", 24);
  title->SetText("Game Over!");
  title->SetPosition({(screen.w - title->rect.w) / 2.0f, 150.0f});
  AddNode(title);

  // Score Tracking
  auto payload = GameStateManager::Instance().GetPayload();
  int currentScore = payload->Get<int>("totalPoints", 0);
  int bestScore = payload->Get<int>("bestPoints", 0);

  if (currentScore > bestScore) { payload->Set<int>("bestPoints", currentScore); }

  auto scoreLabel = std::make_shared<Label>(SDL_FRect{0, 0, 0, 0});
  scoreLabel->SetFont("resources/OpenSans-Regular.ttf", 24);
  scoreLabel->SetText("Total Won: " + std::to_string(currentScore));
  scoreLabel->SetPosition({(screen.w - scoreLabel->rect.w) / 2.0f, 200.0f});
  AddNode(scoreLabel);

  // Restart Button
  auto restartBtn = std::make_shared<Button>(SDL_FRect{(screen.w - 100.0f) / 2.0f, 240.0f, 100, 30}, [](bool) {
    GameStateManager::Instance().ChangeState(GameState::Game);
    AudioManager::Instance().PlaySound("snd_click");
  });
  restartBtn->SetTextures("img_button_up", "img_button_down");
  restartBtn->SetText("Restart", "resources/OpenSans-Regular.ttf", 18);
  AddNode(restartBtn);

  AddMuteButton();
}

void BubbleGame::AddMuteButton() {
  SDL_FRect screen = GameScene::GetRect();
  SDL_FRect rect = {screen.w - 64.0f, screen.h - 42.0f, 32, 32};

  auto muteBtn = std::make_shared<ToggleButton>(rect, [](bool pressed) {
    AudioManager::Instance().SetMuted(pressed);
    AudioManager::Instance().PlaySound("snd_click");
  });
  muteBtn->SetTextures("img_unmuted", "img_muted");
  muteBtn->SetLayer(100); // Ensure it's on top of other UI elements
  AddNode(muteBtn);
}