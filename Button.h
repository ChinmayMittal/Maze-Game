#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <SDL2/SDL.h>
#include "MyTexture.h"
#include "MyWindow.h"
#include "Text.h"
#include <functional>

class Button
{
private:
    LTexture *tex;
    LTexture *highlightTexture = NULL;
    LTexture *clickTexture = NULL;
    LWindow &window;
    Text mText;
    SDL_Color bgColor;
    SDL_Color highlightColor = {0, 0, 0, 255};
    int highlightWidth = 2;
    int x, y, w, h;
    bool centerX;
    bool highlighted = false;
    bool clicked = false;
    std::function<void(LWindow &window)> clickListener = [](LWindow &window) {};

public:
    Button(int x, int y, LTexture *tex, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor);
    Button(int x, int y, int w, int h, SDL_Color bgColor, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor);
    void handle(SDL_Event &e);
    void setCenterX(bool centerX);
    void setHighlightTexture(LTexture &texture);
    void setClickTexture(LTexture &texture);
    void render(SDL_Renderer *renderer);
    void cleanUp();
    void setOnClickListener(std::function<void(LWindow &window)> f);
    int getX();
    int getY();
    void setX(int x);
    void setY(int y);
    int getWidth();
    int getHeight();
};

#endif