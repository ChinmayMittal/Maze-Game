#include <vector>
#include <SDL2/SDL.h>

#include "TileAtlas.h"

void TileAtlas::init(int uniqueTilesX, int uniqueTilesY, int tileWidth, int tileHeight)
{
    for (int i = 0; i < uniqueTilesY; i++)
    {
        for (int j = 0; j < uniqueTilesX; j++)
        {
            tileClips.push_back({tileWidth * j, tileHeight * i, tileWidth, tileHeight});
        }
    }
}