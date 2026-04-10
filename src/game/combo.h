#pragma once
#include <SDL3/SDL.h>
#include "core/node/node.h"
#include <any>
#include <memory>
#include "core/node/label.h"
#include "core/managers/eventManager.h"
#include "core/scene.h"
#include "core/utils/math.h"

struct ComboAnimation
{
    bool enabled = false;
    float targetScale = 1.0f;
    float currentScale = 1.0f;
    float target_time_s = 1.0f;
    float current_time_s = 0.0f;
};

class Combo : public Node
{

private:
    const int BASE_VALUE = 5;
    std::string value;
    std::shared_ptr<Label> label;
    ComboAnimation animation;

    void SetValueCallback(std::any data);
    void StartAnimation(float targetScale);
    void StopAnimation();

public:
    Combo();

    void Update(float deltaTime) override;
};