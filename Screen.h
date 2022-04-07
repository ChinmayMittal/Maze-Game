#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>

#include "MyWindow.h"

class LScreen
{
protected:
    LWindow &window;

public:
    LScreen(LWindow &window);
    virtual void handleEvent(SDL_Event &e) = 0;
    virtual void update() = 0;
    virtual void render(SDL_Renderer *renderer) = 0;
    virtual void cleanUp() = 0;
};

#endif