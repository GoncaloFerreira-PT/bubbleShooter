#include "core/node/button.h"
#include "core/managers/textureManager.h"

void Button::Draw(SDL_Renderer *renderer) {
  SDL_Color modulate = GetModulate();
  SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
  if (!textureIDDown.empty() && !textureIDUp.empty()) {
    if (hovered)
      TextureManager::Instance().Draw(textureIDDown, rect);
    else
      TextureManager::Instance().Draw(textureIDUp, rect);
  } else {

    SDL_RenderFillRect(renderer, &rect);
  }

  if (label) {
    // Center the label's texture inside the button
    label->rect.x = rect.x + (rect.w - label->rect.w) / 2.0f;
    label->rect.y = rect.y + (rect.h - label->rect.h) / 2.0f - 4.0f;
    label->Draw(renderer);
  }

  if (textureRect) textureRect->Draw(renderer);
}

bool Button::HandleInput(SDL_Event *event) {
  if (event->type == SDL_EVENT_MOUSE_MOTION || event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
      event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    float windowX, windowY;
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
      windowX = event->motion.x;
      windowY = event->motion.y;
    } else {
      windowX = event->button.x;
      windowY = event->button.y;
    }

    SDL_Renderer *renderer = GameScene::Instance().GetRenderer();
    float logicalMouseX, logicalMouseY;

    SDL_RenderCoordinatesFromWindow(renderer, windowX, windowY, &logicalMouseX, &logicalMouseY);

    SDL_FPoint mousePoint = {logicalMouseX, logicalMouseY};
    if (SDL_PointInRectFloat(&mousePoint, &rect)) {
      if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        OnClick();
      } else if (!hovered) {
        OnEnter();
      }
      return true;
    } else if (hovered) {
      OnLeave();
    }
  }
  return false;
}

void Button::SetText(const std::string &text, const std::string &fontPath, int fontSize) {

  label = std::make_shared<Label>(SDL_FRect{0, 0, 100, 30});
  label->SetFont(fontPath, fontSize);
  label->SetText(text);
}

void Button::SetTextureRect(const std::string &textureID) {
  textureRect = std::make_shared<TextureRect>(rect, textureID);
}

void Button::SetTextures(const std::string &textureIDUp, const std::string &textureIDDown) {
  this->textureIDUp = textureIDUp;
  this->textureIDDown = textureIDDown;
}

void Button::OnClick() {
  pressed = true;
  if (callback) callback(pressed);
  pressed = !pressed;
}

void Button::OnEnter() { hovered = true; }

void Button::OnLeave() { hovered = false; }