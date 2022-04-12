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
    // Initializes position and id
    Tile(LTexture &myTexture, LGame &game, int x, int y, int width, int height, int tileID);

    // Shows the tile
    int render(SDL_Renderer *renderer, SDL_Rect &camera);

    // Get the tile id
    int getID();
    
    void setType( int type ) ; 

    int getType( ) ; 
    // get the tile 
    // Get the collision box
    SDL_Rect getBox();

    void cleanUp();

private:
    // The attributes of the tile
    SDL_Rect mBox;

    LTexture &mTexture;

    LGame &mGame;

    // The tile id from map 
    int mID;
    // tells the type of each tile ie which entity does it belong to 
    int mType ; 
};

#endif