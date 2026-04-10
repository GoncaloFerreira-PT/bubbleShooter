#include "core/node/label.h"
#include "core/utils/console.h"


void Label::SetText(const std::string &newText)
{
    if (text != newText)
    {
        text = newText;
        dirty = true;
    }
}


void Label::SetFont(const std::string &fontPath, const int fontSize)
{
    auto newFont = FontManager::Instance().GetFont(fontPath, fontSize);
    if (font != newFont)
    {
        font = newFont;
        dirty = true;
    }
}


void Label::RegenerateTexture(SDL_Renderer *renderer)
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (font == nullptr || text.empty()){
        dirty = false;
        return;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font.get(), text.c_str(), text.length(), {255, 255, 255, 255});
    if (!surface)
    {
        Console::Error("TTF_RenderText_Blended() Error: " + std::string(SDL_GetError()));
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        Console::Error("SDL_CreateTextureFromSurface() Error: " + std::string(SDL_GetError()));
        SDL_DestroySurface(surface);
        return;
    }

    rect.w = (float)surface->w;
    rect.h = (float)surface->h;

    SDL_DestroySurface(surface);
    dirty = false;
}


void Label::Draw(SDL_Renderer *renderer)
{
    if (dirty)
    {
        RegenerateTexture(renderer);
    }

    if (texture)
    {
        SDL_SetTextureColorMod(texture, modulate.r, modulate.g, modulate.b);
        SDL_SetTextureAlphaMod(texture, modulate.a);
        SDL_FRect scaledRect = GetScaledRect();
        SDL_RenderTexture(renderer, texture, NULL, &scaledRect);
    }
}