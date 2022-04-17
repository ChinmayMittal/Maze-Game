#include "Button.h"
#include "MyWindow.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <functional>

Button::Button(int x, int y, LTexture *tex, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor) : x(x), y(y), w(tex->getWidth()), h(tex->getHeight()), mText(window, text, font, textColor), window(window), tex(tex), bgColor({255, 255, 255, 255}), centerX(false)
{
}

Button::Button(int x, int y, int w, int h, SDL_Color bgColor, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor) : x(x), y(y), w(w), h(h), window(window), mText(window, text, font, textColor), tex(NULL), bgColor(bgColor), centerX(false)
{
}

void Button::render(SDL_Renderer *renderer)
{
    bool renderDefault = true;
    // std::cout << "IN RENDER" << std::endl;

    if (centerX)
    {
        x = (window.getWidth() - w) / 2;
    }

    if (clicked && clickTexture != NULL)
    {
        int newW = clickTexture->getWidth();
        int newH = clickTexture->getHeight();
        int newX = x - (newW - w) / 2;
        int newY = y - (newH - h) / 2;
        clickTexture->render(renderer, newX, newY);
        renderDefault = false;
        // std::cout << "HERE2" << std::endl;
    }

    else if (highlighted)
    {
        if (highlightTexture != NULL)
        {
            int newW = highlightTexture->getWidth();
            int newH = highlightTexture->getHeight();
            int newX = x - (newW - w) / 2;
            int newY = y - (newH - h) / 2;
            highlightTexture->render(renderer, newX, newY);
            renderDefault = false;
        }
        else
        {
            SDL_Rect fillRect = {x - highlightWidth, y - highlightWidth, w + 2 * highlightWidth, h + 2 * highlightWidth};
            SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a);
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }

    if (renderDefault)
    {
        if (tex != NULL)
        {
            tex->render(renderer, x, y);

            // std::cout << x << " " << y << std::endl;
        }
        else
        {
            SDL_Rect fillRect = {x, y, w, h};
            SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }
    int textX = x + (w - mText.getWidth()) / 2;
    int textY = y + (h - mText.getHeight()) / 2;
    mText.render(renderer, textX, textY);
}

void Button::cleanUp()
{
    if (tex != NULL)
    {
        tex->free();
    }
    mText.cleanUp();
}

void Button::setCenterX(bool centerX)
{
    this->centerX = centerX;
}

void Button::handle(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_MOUSEMOTION:
        if (e.motion.x >= x && e.motion.x <= x + w && e.motion.y >= y && e.motion.y <= y + h)
        {
            highlighted = true;
        }
        else
        {
            highlighted = false;
            clicked = false;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.x >= x && e.button.x <= x + w && e.button.y >= y && e.button.y <= y + h && e.button.button == SDL_BUTTON_LEFT)
        {
            clicked = true;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (e.button.x >= x && e.button.x <= x + w && e.button.y >= y && e.button.y <= y + h && e.button.button == SDL_BUTTON_LEFT && clicked)
        {
            clickListener(window);
        }
        break;
    default:
        break;
    }
}

void Button::setHighlightTexture(LTexture &texture)
{
    highlightTexture = &texture;
}

void Button::setOnClickListener(std::function<void(LWindow &window)> f)
{
    clickListener = f;
}

void Button::setClickTexture(LTexture &texture)
{
    clickTexture = &texture;
}

int Button::getX()
{
    return x;
}

int Button::getY()
{
    return y;
}

int Button::getWidth()
{
    return w;
}

int Button::getHeight()
{
    return h;
}

void Button::setX(int x)
{
    this->x = x;
}

void Button::setY(int y)
{
    this->y = y;
}