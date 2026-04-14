#pragma once
#include "core/gameScene.h"
#include "core/node/label.h"
#include "core/node/node.h"
#include <SDL3/SDL.h>
#include <any>
#include <memory>

struct ComboAnimation {
  bool enabled = false;
  float targetScale = 1.0f;
  float currentScale = 1.0f;
  float target_time_s = 1.0f;
  float current_time_s = 0.0f;
};

class ComboContainer : public Node {

private:
  int currentValue = 0;
  std::shared_ptr<Label> label;

  int total;
  std::shared_ptr<Label> totalLabel;

  std::shared_ptr<Label> bestTotalLabel;

  ComboAnimation animation;

  void SetValueCallback(std::any data);
  void StartAnimation(float targetScale);
  void StopAnimation();

public:
  ComboContainer();

  void Update(float deltaTime) override;
};