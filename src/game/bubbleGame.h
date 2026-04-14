#pragma once
#include "core/gameScene.h"

class BubbleGame : public GameScene {
public:
  BubbleGame() : GameScene() {};
  void Initialize(SDL_Renderer *renderer) override;
  void OnStateChangedCallback(std::any data) override;
  void HandleInput(SDL_Event *event) override;

private:
  /** Constructs the main menu UI and background showcase. */
  void SetupMenuState();

  /** Initializes the active gameplay scene (Cannon, Inventory, Board). */
  void SetupGameState();

  /** Displays the final score and restart options. */
  void SetupEndGameState();

  void AddMuteButton();
};