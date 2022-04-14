#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Player.h"
#include "Tile.h"
#include "MyTexture.h"
#include "Game.h"
#include "Timer.h"
#include <iostream>
#include <string>
#include <vector>
#include<time.h>
#include<stdlib.h>
#include"constants.h"


std :: vector < std :: string >  hostelNames{ "nilgiri" , "kara" , "aravali" , "jwala" , "kumaon" , "vindy" , "satpura" , "udai_girnar" , "himadri" , "kailash" } ;

Player ::Player(LTexture &myTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom) : mTexture(myTexture), mGame(game), wallCollisionMusic(std::string("collision.wav"))
{
    // Initialize the collision box
    mBox.x = 32 * 7;
    mBox.y = 0;
    mBox.w = playerWidth;
    mBox.h = playerHeight;

    srand(time(0));
    hostelName = hostelNames[rand()%hostelNames.size()]; 
    currentTaskTime = 0 ; 
    currentTaskTimer = LTimer() ; 
    updateState = { 0.0 , 0 , 0 } ; 
    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;
    // wallCollisionMusic.play() ;
    // SDL_Delay(1000) ;
    // wallCollisionMusic.play() ;
    // SDL_Delay(2000) ;
    this->velocity = 10;
    this->moveFactor = 1;
    this->direction = 'D';
    this->playerHeight = playerHeight;
    this->playerWidth = playerWidth;
    this->right = right;
    this->left = left;
    this->top = top;
    this->bottom = bottom;
    this->mframes = 0;
    this->health = 80;
    this->money = 100;
    this->points = 0;
    int numberOfimages = myTexture.getWidth() / playerWidth;
    playerImages.resize(4 * numberOfimages);
    this->numOfAnimationImages = numberOfimages;
    healthTimer.start() ; 
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

void Player::handleEvent(SDL_Event &e)
{

    // If a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY = -velocity;
            mVelX = 0;
            break;
        case SDLK_DOWN:
            mVelY = velocity;
            mVelX = 0;
            break;
        case SDLK_LEFT:
            mVelX = -velocity;
            mVelY = 0;
            break;
        case SDLK_RIGHT:
            mVelX = velocity;
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
        case SDLK_DOWN:
            mVelY = 0;
            break;
        case SDLK_LEFT:
        case SDLK_RIGHT:
            mVelX = 0;
            break;
        }
    }
}

void Player::move()
{
    if( !isBusy())
    {
        // Move the dot left or right
        mBox.x += mVelX * moveFactor;
        // std::cout << "After move: " << mBox.x << std::endl;

        // If the dot went too far to the left or right or touched a wall
        if ((mBox.x < 0) || (mBox.x + playerWidth > mGame.getLevelWidth()) /* || touchesWall(mBox, tiles) */)
        {
            // move back
            mBox.x -= mVelX * moveFactor;
            // wallCollisionMusic.play() ;
        }

        // Move the dot up or down
        mBox.y += mVelY * moveFactor;

        // If the dot went too far up or down or touched a wall
        if ((mBox.y < 0) || (mBox.y + playerHeight > mGame.getLevelHeight()) /* || touchesWall(mBox, tiles) */)
        {
            // move back
            mBox.y -= mVelY * moveFactor;
            // wallCollisionMusic.play() ;
        }
    }
}

void Player::moveBy(int offsetX, int offsetY)
{
    mBox.x += offsetX;
    mBox.y += offsetY;
    // std::cout << "New Player Y: " << mBox.y << std::endl;
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

void Player::setVelocity(int vel)
{
    mVelX /= velocity;
    mVelY /= velocity;
    velocity = vel;
    mVelX *= velocity;
    mVelY *= velocity;
}

void Player::setHealth(float h)
{
    health = h;
    if( health > gMaxPlayerHealth ) health = gMaxPlayerHealth ; 
}

void Player ::setMoney(int m)
{
    money = m;
}

void Player ::setPoints(int p)
{
    points = p;
}

void Player::setMoveFactor(int factor)
{
    moveFactor = factor;
}

SDL_Rect Player::getBox()
{
    return mBox;
}

int Player::getXVel()
{
    return mVelX * moveFactor;
}

int Player::getYVel()
{
    return mVelY * moveFactor;
}

int Player ::getHealth()
{
    return (int)health;
}

int Player ::getMoney()
{
    return money;
}

int Player ::getPoints()
{
    return points;
}

std::string Player :: getHostelName(){

    return hostelName ; 
}

void Player::update()
{
    // std::cout << yuluTimer.isStarted() << std::endl;
    // std :: cout << health << "\n" ; 
    if (yuluTimer.isStarted())
    {
        moveFactor = 2;
        if (yuluTimer.getTicks() / 1000 >= 2)
        {
            // std::cout << ((yuluTimer.getTicks() / 1000) - 50) << std::endl;
            money -= 2;
            yuluTimer.stop();
            yuluTimer.start();
            // std::cout << money << std::endl;
        }
    }
    else
    {
        moveFactor = 1;
    }

    if( healthTimer.isStarted() and hasYulu()){
        if (healthTimer.getTicks() / 1000 >= secondPerHealthDecreasewithYulu ){
            health -= 1 ; 
            healthTimer.stop() ; 
            healthTimer.start() ; 
        }
    }else if( healthTimer.isStarted() ) {
        if (healthTimer.getTicks() / 1000 >= secondPerHealthDecrease ){
            health -= 1 ; 
            healthTimer.stop() ; 
            healthTimer.start() ; 
        }
    }

    if( currentTaskTimer.getTicks() > currentTaskTime){
        currentTaskTime = 0  ; 
        currentTaskTimer.stop() ; 
        updateStateParameters( updateState) ; 
        updateState = { 0.0 , 0 , 0 } ; 
        // update player stats 
    }
}

void Player::toggleYulu()
{
    if (yuluTimer.isStarted())
    {
        yuluTimer.stop();
        health -= ( ( healthTimer.getTicks()/1000.0)/secondPerHealthDecreasewithYulu) ; 
    }
    else
    {
        yuluTimer.start();
        health -= ( ( healthTimer.getTicks()/1000.0)/secondPerHealthDecrease) ; 
    }
    healthTimer.stop() ; 
    healthTimer.start() ; 
}

void Player::setLastTileType(int tileType)
{
    lastTileType = tileType;
}

int Player::getLastTileType()
{
    return lastTileType;
}

bool Player::hasYulu()
{
    return yuluTimer.isStarted();
}

LGame& Player :: getGame( )
{
    return mGame ; 
}

void Player::resetHealth()
{
    health = gMaxPlayerHealth ; 
}


LTimer& Player :: getCurrentTaskTimer()
{
    return currentTaskTimer ; 
}

int Player :: getCurrentTaskTime()
{
    return currentTaskTime ; 
}

void  Player::setCurrentTaskTime( int t )
{
    currentTaskTime = t ; 
}

bool Player::isBusy()
{
    if(  currentTaskTimer.isStarted() and (currentTaskTimer.getTicks() < currentTaskTime)){
        return true ; 
    }else{
        return false ; 
    }
}

void Player::updateStateParameters( playerStateUpdate s)
{
    setHealth( getHealth() + s.health ) ; 
    setPoints( getPoints() + s.points) ; 
    setMoney( getMoney() + s.money ) ; 
}

void Player::setUpdateStateParameters( playerStateUpdate s ) 
{
    updateState = s ; 
}