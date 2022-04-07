#ifndef TILE_ATLAS_H
#define TILE_ATLAS_H

#include <SDL2/SDL.h>

#include <vector>

class TileAtlas
{
public:
    std::vector<SDL_Rect> tileClips;
    void init(int uniqueTilesX, int uniqueTilesY, int tileWidth, int tileHeight);
};

#endif