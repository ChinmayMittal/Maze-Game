#include "Text.h"
#include "MyWindow.h"
#include <iostream>

Text::Text(LWindow &window, std::string txt, TTF_Font *font, SDL_Color textColor) : mWindow(window), mText(txt), mTextColor(textColor), mFont(font)
{
    mWindow.loadText(mTexture, txt, font, textColor);
}

int Text::getWidth()
{
    return mTexture.getWidth();
}

int Text::getHeight()
{
    return mTexture.getHeight();
}

void Text::render(SDL_Renderer *renderer, int x, int y)
{
    mTexture.render(renderer, x, y);
}

void Text::cleanUp()
{
    mTexture.free();
}

void Text::setText(std::string txt){
    mText = txt;
    mTexture.free();
    mWindow.loadText(mTexture, txt, mFont, mTextColor);
}
