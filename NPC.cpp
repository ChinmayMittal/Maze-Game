#include"NPC.h"
#include "Game.h"
#include <stdlib.h>
#include<time.h>
#include<iostream>
#include<utility>
#include<vector>



NPC :: NPC(LTexture &myTexture, std::string nameOfNPC , LGame &game, int NPCHeight, int NPCWidth, int right, int left, int top, int bottom) : mTexture(myTexture) , mGame(game)
{
    std::vector<std::pair<int,int>> Exits{ {7,0} , { 0 , 53} , {0,92} ,{0,115} , {18,127} , {55,126} , {68,8} , {67,53} } ;
    for( auto &point : Exits)
    {
        point = { point.first*32 , point.second*32} ; 
    }
    name = nameOfNPC ; 
    int exit = rand()%(Exits.size()) ; 
    mBox.x = Exits[exit].first ; 
    mBox.y = Exits[exit].second ; 
    changeSpeed = 500 ; 
    current = 0 ; 
    mBox.w = NPCWidth ; 
    mBox.h = NPCHeight ; 
    this -> NPCHeight = NPCHeight ; 
    this -> NPCWidth = NPCWidth ; 
    velocity = 1 ; 
    direction = 'D' ; 
    mVelX = 0 ; 
    mVelY = +velocity ; 
    this -> right = right ; 
    this -> left = left ; 
    this -> top = top ; 
    this -> bottom = bottom ; 
    mframes = 0 ; 
    this -> numOfAnimationImages = myTexture.getWidth() / NPCWidth ;
    NPCImages.resize( numOfAnimationImages*4); 
    coolDownTime = 0 ; 
    coolDownTimer = LTimer() ; 
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

    SDL_Rect fillRect = { mBox.x - camera.x -32,mBox.y - camera.y-32 , 32*3, 32*3 };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if(!isBusy())SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0x30 );
    else SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0x30 );         
    SDL_RenderFillRect( renderer, &fillRect );
    mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &NPCImages[dimension + offset]);
    if(!isBusy()) mframes = (mframes + 1) % (numOfAnimationImages * animationSpeed);
    return 0;
}

void NPC::move()
{
    // std :: cout << mBox.x << " " << mBox.y << " "<< mVelX << " " << mVelY << "\n"  ; 
    if( isBusy()) return ; 
    // std::cout << coolDownTimer.isStarted() << "\n" ;
    mBox.x += mVelX ;
    bool boxFlipped = false ; 
    if ((mBox.x < 0) || (mBox.x + NPCWidth > mGame.getLevelWidth()) )
    {
        // move back
        boxFlipped = true ; 
        bool positive ; 
        if( mBox.x < 0 ){
            positive = true ; 
        }else{
            positive = false ;  
        }
        mBox.x -= mVelX ; 
        mVelX = (positive) ? ( abs(mVelX)) : -(abs(mVelX)) ; 
    }


    mBox.y += mVelY ;


    if ((mBox.y < 0) || (mBox.y + NPCHeight > mGame.getLevelHeight()) )
    {
        // move back
        boxFlipped = true ; 
        bool positive ; 
        if( mBox.y < 0 ){
            positive = true ; 
        }else{
            positive = false ;  
        }
        mBox.y -= mVelY ; 
        mVelY = (positive) ? ( abs(mVelY)) : -(abs(mVelY)) ;     
    }
    if( mGame.getTileType( (mBox.x) + mBox.w/2 ,  mBox.y + ( mBox.h)/2 ) == 3 and not boxFlipped ){
        switchDirection() ; 
    }
    current ++ ; 
    if( current == changeSpeed ) {
        current = 0 ; 
        int rem = rand()%4 ;
        mframes = 0 ;  
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

void NPC::switchDirection()
{
    switch ( direction )
    {
    case 'D':
        direction = 'U' ;
        mVelY *= -1 ; 
        break;
    case 'U':
        direction = 'D' ; 
        mVelY *= -1 ; 
        break ; 
    case 'R':
        mVelX *=-1 ; 
        direction = 'L' ; 
        break ; 
    case 'L' : 
        mVelX *= -1 ; 
        direction = 'R' ; 
        break ; 
    }
}

std::string NPC::getName()
{
    return name ; 
}

bool NPC::isBusy()
{
    if( coolDownTimer.isStarted() and coolDownTimer.getTicks() <= coolDownTime){
        return true ;
    }else{
        return false ; 
    }
}

void NPC::setCoolDown( int t)
{
    coolDownTime = t ;
    coolDownTimer.start() ;
    mframes = 0 ; 
    direction = 'D' ; 
    mVelX = 0 ; 
    mVelY = velocity ;   
}

void NPC::update()
{
    if( coolDownTimer.isStarted() and coolDownTimer.getTicks() > coolDownTime){
        coolDownTime = 0 ; 
        coolDownTimer.stop() ; 
    }
}