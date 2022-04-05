#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <SDL_image.h>

#include "MyTexture.h"
#include "MyWindow.h"
#include "Renderable.h"

// The tile
class Tile : public Renderable
{
public:
    // Initializes position and type
    Tile(LTexture &myTexture, LWindow &window, int x, int y, int width, int height, int tileType);

    // Shows the tile
    int render(SDL_Renderer *renderer, SDL_Rect &camera);

    // Get the tile type
    int getType();

    // Get the collision box
    SDL_Rect getBox();

    void cleanUp();

private:
    // The attributes of the tile
    SDL_Rect mBox;

    LTexture &mTexture;

    LWindow &mWindow;

    // The tile type
    int mType;
};

#endif