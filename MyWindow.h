#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

#include "MyTexture.h"
#include "Renderable.h"
#include "Text.h"

class LScreen;

class LWindow
{
public:
    // Intializes internals
    LWindow(int width, int height);

    void begin();

    // Handles window events
    void handleEvent(SDL_Event &e);

    // Window dimensions
    int getWidth();
    int getHeight();

    // Window focii
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();

    bool loadTexture(LTexture &texture, std::string path);
    bool loadText(LTexture &texture, std::string txt, TTF_Font *font, SDL_Color textColor);

    void setCurrScreen(LScreen *screen);
    LScreen *getCurrScreen();

    void maximize();
    void restore();

private:
    bool initLibs();
    void cleanUp();

    // Window data
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;

    LScreen *mCurrScreen;

    // Window dimensions
    int mWidth;
    int mHeight;
    int FPS;

    // Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullScreen;
    bool mMinimized;
};

#endif