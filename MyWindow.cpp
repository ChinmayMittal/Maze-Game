#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "MyWindow.h"
#include "Renderable.h"
#include "Screen.h"
#include "Text.h"
#include "constants.h"

#include "Timer.h"

LWindow::LWindow(int width, int height)
{
    initLibs();

    // Create window
    mWindow = SDL_CreateWindow("MAZE GAME", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (mWindow != NULL)
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = width;
        mHeight = height;
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (mRenderer == NULL)
        {
            printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            // Initialize renderer color
            SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    mCurrScreen = NULL;
}

void LWindow::begin()
{
    bool quit = false;

    // Event handler
    SDL_Event e;

    LTimer updateTimer;
    int MILLIS_PER_FRAME = 1000 / 60;
    int loopTimes = 0;

    // While application is running
    while (!quit)
    {
        updateTimer.start();
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {

            // User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            handleEvent(e);

            if (mCurrScreen != NULL)
            {
                mCurrScreen->handleEvent(e);
            }
        }

        if (mCurrScreen != NULL)
        {
            mCurrScreen->update();
        }
        if (!isMinimized())
        {
            SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(mRenderer);

            if (mCurrScreen != NULL)
            {
                mCurrScreen->render(mRenderer);
            }
            SDL_RenderPresent(mRenderer);
        }
        int frameTicks = updateTimer.getTicks();
        // std::cout << "Time taken: " << frameTicks << std::endl;
        if (frameTicks < MILLIS_PER_FRAME)
        {
            // Wait remaining time
            SDL_Delay(MILLIS_PER_FRAME - frameTicks);
        }
        FPS = 1000 / updateTimer.getTicks();
        std::stringstream caption;
        loopTimes = (loopTimes + 1) % FPSupdatespeed;
        if (loopTimes == 0)
        {
            caption << "MAZEGAME FPS:" << std::to_string(FPS);
            SDL_SetWindowTitle(mWindow, caption.str().c_str());
        }
    }

    mCurrScreen->cleanUp();
    cleanUp();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void LWindow::handleEvent(SDL_Event &e)
{
    // Window event occured
    if (e.type == SDL_WINDOWEVENT)
    {
        // Caption update flag
        bool updateCaption = false;

        switch (e.window.event)
        {
        // Get new dimensions and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            mWidth = e.window.data1;
            mHeight = e.window.data2;
            SDL_RenderPresent(mRenderer);
            // std::cout << "HERE" << std::endl;
            break;

        // Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(mRenderer);
            break;

        // Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            updateCaption = true;
            break;

        // Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            updateCaption = true;
            break;

        // Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            updateCaption = true;
            break;

        // Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            updateCaption = true;
            break;

        // Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

        // Window maxized
        case SDL_WINDOWEVENT_MAXIMIZED:
            mMinimized = false;
            break;

        // Window restored
        case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            break;
        }
    }
    // Enter exit full screen on return key
    else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
    {
        if (mFullScreen)
        {
            SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
            mFullScreen = false;
        }
        else
        {
            SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
            mFullScreen = true;
            mMinimized = false;
        }
    }
}

bool LWindow::initLibs()
{
    // Initialization flag

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        printf("Warning: Linear texture filtering not enabled!");
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    Mix_AllocateChannels(32);


    return true;
}

void LWindow::setCurrScreen(LScreen *screen)
{
    if (mCurrScreen != NULL)
    {
        mCurrScreen->cleanUp();
    }
    delete mCurrScreen;
    mCurrScreen = screen;
}

bool LWindow::loadTexture(LTexture &texture, std::string path)
{
    return texture.loadFromFile(mRenderer, path);
}

bool LWindow::loadText(LTexture &texture, std::string txt, TTF_Font *font, SDL_Color textColor)
{
    return texture.loadFromRenderedText(mRenderer, txt, font, textColor);
}

int LWindow::getWidth()
{
    return mWidth;
}

int LWindow::getHeight()
{
    return mHeight;
}

bool LWindow::hasMouseFocus()
{
    return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
    return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
    return mMinimized;
}

void LWindow::cleanUp()
{
    delete mCurrScreen;

    SDL_DestroyRenderer(mRenderer);
    if (mWindow != NULL)
    {
        SDL_DestroyWindow(mWindow);
    }

    mMouseFocus = false;
    mKeyboardFocus = false;
    mWidth = 0;
    mHeight = 0;
}

LScreen *LWindow::getCurrScreen()
{
    return mCurrScreen;
}

void LWindow::maximize()
{
    SDL_MaximizeWindow(mWindow);
}

void LWindow::restore()
{
    SDL_RestoreWindow(mWindow);
}