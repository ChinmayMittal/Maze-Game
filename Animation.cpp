#include "Animation.h"
#include "MyTexture.h"
#include <SDL2/SDL.h>
#include<iostream>

Animation::Animation( LTexture &texture  , int width , int height) : animationTexture(texture)
{
    mWidth = width ; 
    mHeight = height ; 
    mFrames = 0 ; 
    mBox = { 0 , 0 } ; 
    animationSpeed = 20 ; 
    int numberOfColumns = texture.getWidth() / width;
    int numberOfRows = texture.getHeight() / height ; 
    numberOfAnimationImages = ( numberOfColumns*numberOfRows);
    animationImages.resize( numberOfColumns*numberOfRows);
    for( int row = 0 ; row < numberOfRows ; row ++ ){
        for( int col = 0 ; col < numberOfColumns ; col ++ ){
            animationImages[row*numberOfColumns + col ] = {
                 col*width,
                 row*height ,  
                 width , 
                 height 
            } ; 
        }
    }
}

Animation::~Animation() 
{
    cleanup() ; 
}

void Animation::cleanup()
{
    animationTexture.free() ; 
}

void Animation::setBox( int x , int y )
{
    mBox = { x , y } ; 
}
int Animation::render(SDL_Renderer *renderer, SDL_Rect &camera)
{
    animationTexture.render( renderer , mBox.x - camera.x, mBox.y - camera.y , &animationImages[mFrames/(animationSpeed)] ) ; 
    mFrames = (mFrames + 1) % (numberOfAnimationImages * animationSpeed);
    return 0 ; 
}