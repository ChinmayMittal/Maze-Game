#include "TextInput.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "MyWindow.h"
#include <string>

TextInput::TextInput(int x, int y, int w, int h, SDL_Color bgColor, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor) : x(x), y(y), w(w), h(h), window(window), mText(window, text, font, textColor), bgColor(bgColor), centerX(false)
{
}

void TextInput::render(SDL_Renderer *renderer)
{
    if (centerX)
    {
        x = (window.getWidth() - w) / 2;
    }
    SDL_Rect fillRect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &fillRect);
    int textX = x + (w - mText.getWidth()) / 2;
    int textY = y + (h - mText.getHeight()) / 2;
    mText.render(renderer, textX, textY);
    if (focused)
    {
        SDL_Rect indicatorRect = {x + (w + mText.getWidth()) / 2 + 5,
                                  y + (h + mText.getHeight()) / 2,
                                  10,
                                  4};
        SDL_Color txtColor = mText.getColor();
        SDL_SetRenderDrawColor(renderer, txtColor.r, txtColor.g, txtColor.b, txtColor.a);
        SDL_RenderFillRect(renderer, &indicatorRect);
    }
}

void TextInput::cleanUp()
{
    mText.cleanUp();
}

void TextInput::setCenterX(bool centerX)
{
    this->centerX = centerX;
}

void TextInput::handleEvent(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_MOUSEMOTION:
        if (!(e.motion.x >= x && e.motion.x <= x + w && e.motion.y >= y && e.motion.y <= y + h))
        {
            clicked = false;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.x >= x && e.button.x <= x + w && e.button.y >= y && e.button.y <= y + h && e.button.button == SDL_BUTTON_LEFT)
        {
            clicked = true;
        }
        else
        {
            focused = false;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (e.button.x >= x && e.button.x <= x + w && e.button.y >= y && e.button.y <= y + h && e.button.button == SDL_BUTTON_LEFT && clicked)
        {
            focused = true;
            clicked = false;
        }
        break;
    case SDL_KEYDOWN:
        if (focused && e.key.repeat == 0)
        {
            if (e.key.keysym.sym == SDLK_BACKSPACE && mText.length() > 0)
            {
                // lop off character
                mText.pop_back();
            }
            // Handle copy
            else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                SDL_SetClipboardText(mText.getText().c_str());
            }
            // Handle paste
            else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
            {
                mText.setText(SDL_GetClipboardText());
            }
        }
        break;
    case SDL_TEXTINPUT:
        if (focused && !(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
        {
            // Append character
            mText.setText(mText.getText() + e.text.text);
        }
        break;
    default:
        break;
    }
}

void TextInput::setX(int x)
{
    this->x = x;
}

void TextInput::setY(int y)
{
    this->y = y;
}

std::string TextInput::getText()
{
    return mText.getText();
}