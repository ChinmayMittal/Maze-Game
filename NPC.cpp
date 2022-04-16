#include"NPC.h"
#include "Game.h"
#include <stdlib.h>
#include<time.h>

NPC :: NPC(LTexture &myTexture, LGame &game, int NPCHeight, int NPCWidth, int right, int left, int top, int bottom) : mTexture(myTexture) , mGame(game)
{
    srand(time(0)) ; 
    mBox.x = 32*7 ; 
    changeSpeed = 500 ; 
    current = 0 ; 
    mBox.y = 0 ; 
    mBox.w = NPCHeight ; 
    mBox.h = NPCWidth ; 
    velocity = 1 ; 
    direction = 'U' ; 
    mVelX = 0 ; 
    mVelY = -velocity ; 
    this -> right = right ; 
    this -> left = left ; 
    this -> top = top ; 
    this -> bottom = bottom ; 
    mframes = 0 ; 
    this -> numOfAnimationImages = myTexture.getWidth() / NPCWidth ;
    NPCImages.resize( numOfAnimationImages*4); 
    for (int col = 0; col < numOfAnimationImages; col++)
    {
        // right image in column col
        NPCImages[col] = {
            col * NPCWidth,
            right * NPCHeight,
            NPCWidth,
            NPCHeight};
        // left image in column col
        NPCImages[numOfAnimationImages * 1 + col] = {
            col * NPCWidth,
            left * NPCHeight,
            NPCWidth,
            NPCHeight};
        // top image in column col
        NPCImages[numOfAnimationImages * 2 + col] = {
            col * NPCWidth,
            top * NPCHeight,
            NPCWidth,
            NPCHeight};
        // bottom bottom in column col
        NPCImages[numOfAnimationImages * 3 + col] = {
            col * NPCWidth,
            bottom * NPCHeight,
            NPCWidth,
            NPCHeight};
    }
}

SDL_Rect NPC::getBox()
{
    return mBox ; 
}

int NPC::render(SDL_Renderer *renderer, SDL_Rect &camera)
{
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


    mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &NPCImages[dimension + offset]);

    mframes = (mframes + 1) % (numOfAnimationImages * animationSpeed);
    return 0;
}

void NPC::move()
{
        
    mBox.x += mVelX ;

    if ((mBox.x < 0) || (mBox.x + NPCWidth > mGame.getLevelWidth()) )
    {
        // move back
        mBox.x -= mVelX ; 
        mVelX *= -1 ; 
    }


    mBox.y += mVelY ;


    if ((mBox.y < 0) || (mBox.y + NPCHeight > mGame.getLevelHeight()) )
    {
        // move back
        mBox.y -= mVelY  ; 
        mVelY *= -1 ;         
    }

    current ++ ; 
    if( current == changeSpeed ) {
        current = 0 ; 
        int rem = rand()%4 ; 
        switch (rem)
        {
        case 0:
            mVelX = velocity  , mVelY = 0 ; 
            break;
        case 1:
            mVelX = -velocity  , mVelY = 0 ;         
            break;
        case 2:
            mVelX = 0  , mVelY = velocity ;         
            break;
        case 3:
            mVelX = 0  , mVelY = -velocity ;         
            break;
        }
    }
}

void NPC::cleanUp()
{
    mTexture.free(); 
}