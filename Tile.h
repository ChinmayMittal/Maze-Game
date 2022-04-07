#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "MyTexture.h"
#include "Renderable.h"

class LGame;

// The tile
class Tile : public Renderable
{
public:
    // Initializes position and type
    Tile(LTexture &myTexture, LGame &game, int x, int y, int width, int height, int tileType);

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

    LGame &mGame;

    // The tile type
    int mType;
};

#endif