#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Tile.h"
#include "collision.h"
#include "TileAtlas.h"
#include "Game.h"

Tile::Tile(LTexture &texture, LGame &game, int x, int y, int width, int height, int tileType) : mTexture(texture), mGame(game)
{
    // Get the offsets
    mBox.x = x;
    mBox.y = y;

    // Set the collision box
    mBox.w = width;
    mBox.h = height;

    // Get the tile type
    mType = tileType;
}

int Tile::render(SDL_Renderer *renderer, SDL_Rect &camera)
{
    // If the tile is on screen
    if (checkCollision(camera, mBox))
    {
        // Show the tile
        SDL_Rect clipRect = mGame.getTileClip(mType);
        mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &clipRect);
    }
    return 0;
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

void Tile::cleanUp()
{
    mTexture.free();
}