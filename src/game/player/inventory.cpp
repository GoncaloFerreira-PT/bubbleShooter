#include "game/player/inventory.h"
#include "core/managers/eventManager.h"
#include "core/managers/textureManager.h"
#include "core/utils/console.h"
#include <memory>

Inventory::Inventory(SDL_FRect rect) : MovableNode(rect) {
  for (int i = 0; i < size; i++) {
    CreateBall();
  }
  direction = {1, 0};
  speed = animationData.speed;
  EventManager::Instance().Subscribe("ballShot", this, [this](std::any data) { OnBallShotCallback(data); });
}

void Inventory::OnBallShotCallback(std::any _data) {
  std::shared_ptr<Ball> toBeShot = Pop();
  if (!toBeShot) return;
  EventManager::Instance().Emit("ballCreated", toBeShot);
  animationData.animating = true;
}

void Inventory::Draw(SDL_Renderer *renderer) {
  if (deque.size() == 0) return;

  const float width = deque[0]->rect.w;
  for (size_t i = 0; i < deque.size(); i++) {
    Ball *ball = deque[i].get();
    ball->SetPosition({(float)rect.x + (width * i) + ball->rect.w / 2.0f, rect.y + rect.h / 4.0f});
    ball->Draw(renderer);
  }
  // Draw the inventory texture at its final, fixed position.
  SDL_FRect texture_rect = rect;
  texture_rect.x = animationData.offset;
  TextureManager::Instance().Draw("img_inventory", texture_rect, GetModulate());
}

void Inventory::Update(float deltaTime) {
  if (animationData.animating) {
    const float width = deque[0]->rect.w;
    if (rect.x < animationData.offset + width) {
      Move(deltaTime);
    } else {
      // When the animation finishes, snap to final position and reset state.
      rect.x = animationData.offset;
      animationData.animating = false;
      CreateBall();
    }
  }
}

void Inventory::Push(std::shared_ptr<Ball> ball) { deque.push_front(std::move(ball)); }

void Inventory::CreateBall() {
  auto ball = std::make_shared<Ball>(vec2i{-1, -1});
  ball->color = GetRandomColor();
  ball->SetModulate(GetModulateFromEnum(ball->color));
  Push(std::move(ball));
}

std::shared_ptr<Ball> Inventory::Pop() {
  if (deque.empty()) {
    Console::Log("Deque is empty!");
    return nullptr;
  }
  auto ball = deque.back();
  deque.pop_back();
  return ball;
}