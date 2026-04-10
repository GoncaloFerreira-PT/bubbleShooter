#include "combo.h"

Combo::Combo()
{
    label = std::make_shared<Label>(SDL_FRect{230, 100, 200, 30});
    label->SetFont("resources/OpenSans-Regular.ttf", 64);
    Scene::Instance().AddNode(label);
    EventManager::Instance().Subscribe("ballsDestroyed", [this](std::any data)
                                       { SetValueCallback(data); });

    EventManager::Instance().Subscribe("ballShot", [this](std::any data)
                                       { StopAnimation();});
}

void Combo::SetValueCallback(std::any data)
{
    int count = std::any_cast<int>(data) - 1;
    value = std::to_string(BASE_VALUE * (count * count));
    label->SetText(value);

    float targetScale = Math::MapRange(3.0f, 12.0f, 1.0f, 2.0f, (float)count);
    StartAnimation(targetScale);
}

void Combo::StartAnimation(float targetScale)
{
    animation.targetScale = targetScale;
    animation.currentScale = 1.0f;
    animation.current_time_s = 0.0f;
    animation.enabled = true;
}

void Combo::Update(float deltaTime)
{
    if (!animation.enabled)
        return;

    if (animation.currentScale < animation.targetScale)
    {
        animation.currentScale += deltaTime * 10.0f;
        float percentageToTarget = animation.currentScale / animation.targetScale;
        label->SetAlpha(static_cast<int>(percentageToTarget * 255));
        label->SetScale({animation.currentScale, animation.currentScale});
    }

    if (animation.current_time_s < animation.target_time_s)
    {
        animation.current_time_s += deltaTime;
        return;
    }

    StopAnimation();
}

void Combo::StopAnimation()
{
    animation.enabled = false;
    label->SetAlpha(0);
}