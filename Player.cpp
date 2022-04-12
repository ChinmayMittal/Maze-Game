#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Player.h"
#include "Tile.h"
#include "MyTexture.h"
#include "Game.h"
#include <iostream>

Player ::Player(LTexture &myTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom, int setVelocity) : mTexture(myTexture), mGame(game)
{
    // Initialize the collision box
    mBox.x = 0;
    mBox.y = 0;
    mBox.w = playerWidth;
    mBox.h = playerHeight;

    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    this->DOT_VEL = setVelocity;
    this->direction = 'D';
    this->playerHeight = playerHeight;
    this->playerWidth = playerWidth;
    this->right = right;
    this->left = left;
    this->top = top;
    this->bottom = bottom;
    this->mframes = 0;
    int numberOfimages = myTexture.getWidth() / playerWidth;
    playerImages.resize(4 * numberOfimages);
    this->numOfAnimationImages = numberOfimages;
    for (int col = 0; col < numberOfimages; col++)
    {
        // right image in column col
        playerImages[col] = {
            col * playerWidth,
            right * playerHeight,
            playerWidth,
            playerHeight};
        // left image in column col
        playerImages[numberOfimages * 1 + col] = {
            col * playerWidth,
            left * playerHeight,
            playerWidth,
            playerHeight};
        // top image in column col
        playerImages[numberOfimages * 2 + col] = {
            col * playerWidth,
            top * playerHeight,
            playerWidth,
            playerHeight};
        // bottom bottom in column col
        playerImages[numberOfimages * 3 + col] = {
            col * playerWidth,
            bottom * playerHeight,
            playerWidth,
            playerHeight};
    }
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
void Player::handleEvent(SDL_Event &e)
{
    // If a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY -= DOT_VEL;
            mVelX = 0;
            break;
        case SDLK_DOWN:
            mVelY += DOT_VEL;
            mVelX = 0;
            break;
        case SDLK_LEFT:
            mVelX -= DOT_VEL;
            mVelY = 0;
            break;
        case SDLK_RIGHT:
            mVelX += DOT_VEL;
            mVelY = 0;
            break;
        }
        if (mVelX < 0)
        {
            direction = 'L';
        }
        else if (mVelX > 0)
        {
            direction = 'R';
        }
        else if (mVelY > 0)
        {
            direction = 'D';
        }
        else if (mVelY < 0)
        {
            direction = 'U';
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            if (mVelY != 0)
                mVelY += DOT_VEL;
            break;
        case SDLK_DOWN:
            if (mVelY != 0)
                mVelY -= DOT_VEL;
            break;
        case SDLK_LEFT:
            if (mVelX != 0)
                mVelX += DOT_VEL;
            break;
        case SDLK_RIGHT:
            if (mVelX != 0)
                mVelX -= DOT_VEL;
            break;
        }
    }
}

void Player::move()
{
    // Move the dot left or right
    mBox.x += mVelX;

    // If the dot went too far to the left or right or touched a wall
    if ((mBox.x < 0) || (mBox.x + playerWidth > mGame.getLevelWidth()) /* || touchesWall(mBox, tiles) */)
    {
        // move back
        mBox.x -= mVelX;
    }

    // Move the dot up or down
    mBox.y += mVelY;

    // If the dot went too far up or down or touched a wall
    if ((mBox.y < 0) || (mBox.y + playerHeight > mGame.getLevelHeight()) /* || touchesWall(mBox, tiles) */)
    {
        // move back
        mBox.y -= mVelY;
    }
      
      
}

void Player::setCamera(SDL_Rect &camera)
{
    // Center the camera over the dot
    camera.x = (mBox.x + playerWidth / 2) - mGame.getWindowWidth() / 2;
    camera.y = (mBox.y + playerHeight / 2) - mGame.getWindowHeight() / 2;

    // std::cout << camera.x << std::endl;
    //  Keep the camera in bounds
    if (camera.x < 0)
    {
        camera.x = 0;
    }
    else if (camera.x > mGame.getLevelWidth() - camera.w)
    {
        camera.x = mGame.getLevelWidth() - camera.w;
    }

    if (camera.y < 0)
    {
        camera.y = 0;
    }
    else if (camera.y > mGame.getLevelHeight() - camera.h)
    {
        camera.y = mGame.getLevelHeight() - camera.h;
    }
}

// int Dot::render(SDL_Renderer *renderer, SDL_Rect &camera)
// {
//     // Show the dot
//     // std::cout << camera.x << std::endl;
//     mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y);
//     return 0;
// }

int Player::render(SDL_Renderer *renderer, SDL_Rect &camera)
{
    int dimension = 0;
    switch (direction)
    {
    case 'D':
        dimension = numOfAnimationImages * 3;
        break;
    case 'U':
        dimension = numOfAnimationImages * 2;
        break;
    case 'L':
        dimension = numOfAnimationImages * 1;
        break;
    case 'R':
        dimension = 0;
        break;
    default:
        break;
    }
    int offset = 0;
    if (mVelX != 0 or mVelY != 0)
    {
        offset = mframes / animationSpeed;
    }
    mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &playerImages[dimension + offset]);
    mframes = (mframes + 1) % (numOfAnimationImages * animationSpeed);

    return 0;
}

void Player::cleanUp()
{
    mTexture.free();
}