#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <SDL2/SDL.h>
#include "MyWindow.h"
#include <string>
#include <SDL2/SDL_ttf.h>

class TextInput
{
private:
    int x, y, w, h;
    bool centerX;
    LWindow &window;
    Text mText;
    SDL_Color bgColor;
    bool clicked = false;
    bool focused = false;

public:
    TextInput(int x, int y, int w, int h, SDL_Color bgColor, LWindow &window, std::string text, TTF_Font *font, SDL_Color textColor);
    void render(SDL_Renderer *renderer);
    void cleanUp();
    void setCenterX(bool centerX);
    void handleEvent(SDL_Event &e);
    std::string getText();
    void setX(int x);
    void setY(int y);
};
#endif