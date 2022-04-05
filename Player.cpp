#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"
#include "Tile.h"
#include "MyTexture.h"

#include <iostream>

Dot::Dot(LTexture &texture, LWindow &window) : mTexture(texture), mWindow(window)
{
    // Initialize the collision box
    mBox.x = 0;
    mBox.y = 0;
    mBox.w = DOT_WIDTH;
    mBox.h = DOT_HEIGHT;

    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

// bool Dot::touchesWall(std::vector<Tile> tiles)
// {
//     // Go through the tiles
//     for (int i = 0; i < tiles.size(); ++i)
//     {
//         // If the tile is a wall type tile
//         if ((tiles[i].getType() >= TILE_CENTER) && (tiles[i]->getType() <= TILE_TOPLEFT))
//         {
//             // If the collision box touches the wall tile
//             if (checkCollision(box, tiles[i]->getBox()))
//             {
//                 return true;
//             }
//         }
//     }

//     // If no wall tiles were touched
//     return false;
// }

void Dot::handleEvent(SDL_Event &e)
{
    // If a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY -= DOT_VEL;
            break;
        case SDLK_DOWN:
            mVelY += DOT_VEL;
            break;
        case SDLK_LEFT:
            mVelX -= DOT_VEL;
            break;
        case SDLK_RIGHT:
            mVelX += DOT_VEL;
            break;
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY += DOT_VEL;
            break;
        case SDLK_DOWN:
            mVelY -= DOT_VEL;
            break;
        case SDLK_LEFT:
            mVelX += DOT_VEL;
            break;
        case SDLK_RIGHT:
            mVelX -= DOT_VEL;
            break;
        }
    }
}

void Dot::move()
{
    // Move the dot left or right
    mBox.x += mVelX;

    // If the dot went too far to the left or right or touched a wall
    if ((mBox.x < 0) || (mBox.x + DOT_WIDTH > mWindow.getLevelWidth()) /* || touchesWall(mBox, tiles) */)
    {
        // move back
        mBox.x -= mVelX;
    }

    // Move the dot up or down
    mBox.y += mVelY;

    // If the dot went too far up or down or touched a wall
    if ((mBox.y < 0) || (mBox.y + DOT_HEIGHT > mWindow.getLevelHeight()) /* || touchesWall(mBox, tiles) */)
    {
        // move back
        mBox.y -= mVelY;
    }
}

void Dot::setCamera(SDL_Rect &camera)
{
    // Center the camera over the dot
    camera.x = (mBox.x + DOT_WIDTH / 2) - mWindow.getWidth() / 2;
    camera.y = (mBox.y + DOT_HEIGHT / 2) - mWindow.getHeight() / 2;

    // std::cout << camera.x << std::endl;
    //  Keep the camera in bounds
    if (camera.x < 0)
    {
        camera.x = 0;
    }
    else if (camera.x > mWindow.getLevelWidth() - camera.w)
    {
        camera.x = mWindow.getLevelWidth() - camera.w;
    }

    if (camera.y < 0)
    {
        camera.y = 0;
    }
    else if (camera.y > mWindow.getLevelHeight() - camera.h)
    {
        camera.y = mWindow.getLevelHeight() - camera.h;
    }
}

int Dot::render(SDL_Renderer *renderer, SDL_Rect &camera)
{
    // Show the dot
    // std::cout << camera.x << std::endl;
    mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y);
    return 0;
}

void Dot::cleanUp()
{
    mTexture.free();
}