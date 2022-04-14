#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>
#include "MyTexture.h"

class LWindow;

class Text
{
private:
    LWindow &mWindow;
    LTexture mTexture;
    std::string mText;
    TTF_Font *mFont;
    SDL_Color mTextColor;

public:
    Text(LWindow &window, std::string txt, TTF_Font *font, SDL_Color textColor);
    int getWidth();
    int getHeight();
    void render(SDL_Renderer *renderer, int x, int y);
    void cleanUp();
    void setText(std::string txt);
    int length();
    std::string getText();
    void pop_back();
    SDL_Color getColor();
};

#endif