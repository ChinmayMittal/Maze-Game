#include "Text.h"
#include "MyWindow.h"
#include <iostream>
#include <string>

Text::Text(LWindow &window, std::string txt, TTF_Font *font, SDL_Color textColor) : mWindow(window), mText(txt), mTextColor(textColor), mFont(font)
{
    if (txt != "")
    {
        mWindow.loadText(mTexture, txt, font, textColor);
    }
}

int Text::getWidth()
{
    return mText != "" ? mTexture.getWidth() : 0;
}

int Text::getHeight()
{
    return mText != "" ? mTexture.getHeight() : 0;
}

void Text::render(SDL_Renderer *renderer, int x, int y)
{
    if (mText != "")
    {
        mTexture.render(renderer, x, y);
    }
}

void Text::cleanUp()
{
    if (mText != "")
    {
        mTexture.free();
    }
}

void Text::setText(std::string txt)
{
    mText = txt;
    mTexture.free();
    if (mText != "")
    {
        mWindow.loadText(mTexture, txt, mFont, mTextColor);
    }
}

int Text::length()
{
    return mText.length();
}

std::string Text::getText()
{
    return mText;
}

void Text::pop_back()
{
    if (mText != "")
    {
        mText.pop_back();
        mTexture.free();
        if (mText != "")
        {
            mWindow.loadText(mTexture, mText, mFont, mTextColor);
        }
    }
}

SDL_Color Text::getColor()
{
    return mTextColor;
}