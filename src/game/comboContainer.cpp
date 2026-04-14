#include "comboContainer.h"
#include "core/managers/eventManager.h"
#include "core/managers/gameStateManager.h"
#include "core/utils/math.h"
#include "game/bubbleGameConfig.h"

ComboContainer::ComboContainer() {
  total = 0;
  label = std::make_shared<Label>(SDL_FRect{220, 200, 200, 80});
  label->SetLayer(100);
  label->SetFont("resources/OpenSans-Regular.ttf", 64);
  GameScene::Instance().AddNode(label);

  totalLabel = std::make_shared<Label>(SDL_FRect{440, 435, 200, 80});
  totalLabel->SetText("Points: " + std::to_string(total));
  totalLabel->SetLayer(100);
  totalLabel->SetFont("resources/OpenSans-Regular.ttf", 16);
  GameScene::Instance().AddNode(totalLabel);

  int bestWon = GameStateManager::Instance().GetPayload()->Get<int>("bestPoints", 0);
  if (bestWon != 0) {
    bestTotalLabel = std::make_shared<Label>(SDL_FRect{440, 455, 200, 80});
    bestTotalLabel->SetText("    Best: " + std::to_string(bestWon));
    bestTotalLabel->SetLayer(100);
    bestTotalLabel->SetFont("resources/OpenSans-Regular.ttf", 16);
    GameScene::Instance().AddNode(bestTotalLabel);
  }

  EventManager::Instance().Subscribe("ballsDestroyed", this, [this](std::any data) { SetValueCallback(data); });
  EventManager::Instance().Subscribe("ballShot", this, [this](std::any data) { StopAnimation(); });
}

void ComboContainer::SetValueCallback(std::any data) {
  int count = std::any_cast<int>(data);
  currentValue = Game::Config::Rules::BASE_POINTS * (count * count);
  label->SetText(std::to_string(currentValue));
  label->UpdateSizeOnly();
  float screenCenterX = 640.0f / 2.0f;
  float screenCenterY = 480.0f / 2.0f;
  label->SetPosition({screenCenterX - (label->rect.w / 2.0f), screenCenterY - (label->rect.h / 2.0f)});
  float targetScale = Math::MapRange(3.0f, 12.0f, 1.0f, 1.2f, (float)count);
  StartAnimation(targetScale);
}

void ComboContainer::StartAnimation(float targetScale) {
  animation.targetScale = targetScale;
  animation.currentScale = 0.01f;
  animation.current_time_s = 0.0f;
  animation.enabled = true;
}

void ComboContainer::Update(float deltaTime) {
  if (!animation.enabled) return;

  if (animation.currentScale < animation.targetScale) {
    animation.currentScale += deltaTime * 10.0f;
    float percentageToTarget = animation.currentScale / animation.targetScale;
    label->SetAlpha(static_cast<int>(percentageToTarget * 255));
    label->SetScale({animation.currentScale, animation.currentScale});
  }

  animation.current_time_s += deltaTime;

  if (animation.current_time_s >= animation.target_time_s) { StopAnimation(); }
}

void ComboContainer::StopAnimation() {
  if (currentValue != 0) {
    total += currentValue;
    totalLabel->SetText("Points: " + std::to_string(total));
    totalLabel->UpdateSizeOnly();
    GameStateManager::Instance().GetPayload()->Set<int>("totalPoints", total);
  }
  currentValue = 0;
  animation.enabled = false;
  label->SetAlpha(0);
  label->SetPosition({220, 200});
}