#ifndef TILE_ATLAS_H
#define TILE_ATLAS_H

#include <SDL.h>

#include <vector>

class TileAtlas
{
public:
    static std::vector<SDL_Rect> tileClips;
    static void init(int uniqueTilesX, int uniqueTilesY, int tileWidth, int tileHeight);
};

#endif