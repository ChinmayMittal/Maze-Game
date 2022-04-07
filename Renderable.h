#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Renderable
{
public:
    virtual int render(SDL_Renderer *renderer, SDL_Rect &camera) = 0;
};

#endif