#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Player.h"
#include "Tile.h"
#include "MyTexture.h"
#include "Game.h"
#include "Timer.h"
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "constants.h"
#include <algorithm>
#include <map>
#include <utility>

std ::vector<std ::string> hostelNames{"nilgiri", "kara", "aravali", "jwala", "kumaon", "vindy", "satpura", "udai_girnar", "himadri", "kailash"};

Player ::Player(LTexture &myTexture, LTexture &yuluTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom) : mTexture(myTexture), yuluTexture(yuluTexture), mGame(game)
{
    // Initialize the collision box
    mCollisionMusic = Mix_LoadWAV("resources/collision.wav");
    mMovementMusic = Mix_LoadWAV("resources/collision.wav");
    mGrassMusic = Mix_LoadWAV("resources/grass_sound.wav");
    mTaskCompleteMusic = Mix_LoadWAV("resources/taskComplete.wav");
    mBox.w = playerWidth;
    mBox.h = playerHeight;
    taskAnimation = NULL;
    std::map<std::string, std::pair<int, int>> hostels;
    hostels["nilgiri"] = {15 * 32, 7 * 32};
    hostels["aravali"] = {47 * 32, 7 * 32};
    hostels["kara"] = {31 * 32, 7 * 32};
    hostels["jwala"] = {64 * 32, 7 * 32};
    hostels["kumaon"] = {63 * 32, 18 * 32};
    hostels["vindy"] = {63 * 32, 29 * 32};
    hostels["satpura"] = {63 * 32, 40 * 32};
    hostels["shivalik"] = {45 * 32, 51 * 32};
    hostels["zanskar"] = {27 * 32, 51 * 32};
    hostels["himadri"] = {16 * 32, 104 * 32};
    hostels["kailash"] = {16 * 32, 104 * 32};
    hostels["udai_girnar"] = {63 * 32, 51 * 32};
    hostelName = hostelNames[rand() % hostelNames.size()];
    // hostelName = "nilgiri"; // for testing
    mBox.x = hostels[hostelName].first;
    mBox.y = hostels[hostelName].second;
    currentTaskTime = 0;
    currentTaskTimer = LTimer();
    updateState = {0.0, 0, 0, 0};
    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;
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
    this->money = gMaxPlayerMoney;
    this->points = 0;
    breakfast = lunch = dinner = false;
    taskText = "";
    hasTaskAnimation = false;
    int numberOfimages = myTexture.getWidth() / playerWidth;
    playerImages.resize(4 * numberOfimages);
    this->numOfAnimationImages = numberOfimages;
    healthTimer.start();
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

void Player ::resetPlayer()
{
    mVelX = 0;
    mVelY = 0;
    moveFactor = 1;
    mframes = 0;
    // mBox = {0, 0};
    currentTaskTime = 0;
    currentTaskTimer.stop();
    direction = 'D';
    updateState = {0, 0, 0, 0};
}

void Player::handleEvent(SDL_Event &e)
{
    if (isBusy())
    {

        mVelX = mVelY = 0;
        mframes = 0;
        direction = 'D';
        return;
    }
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
    if (!isBusy())
    {
        // Move the dot left or right
        mBox.x += mVelX * moveFactor;
        // std::cout << "After move: " << mBox.x << std::endl;
        bool movement = true;
        if (mVelX == 0 and mVelY == 0)
        {
            movement = false;
        }
        // If the dot went too far to the left or right or touched a wall
        if ((mBox.x < 0) || (mBox.x + playerWidth > mGame.getLevelWidth()) /* || touchesWall(mBox, tiles) */)
        {
            // move back
            mBox.x -= mVelX * moveFactor;
            Mix_PlayChannel(-1, mCollisionMusic, 0);
            movement = false;
        }

        // Move the dot up or down
        mBox.y += mVelY * moveFactor;

        // If the dot went too far up or down or touched a wall
        if ((mBox.y < 0) || (mBox.y + playerHeight > mGame.getLevelHeight()) /* || touchesWall(mBox, tiles) */)
        {
            // move back
            mBox.y -= mVelY * moveFactor;
            Mix_PlayChannel(-1, mCollisionMusic, 0);
            movement = false;
        }
        Mix_Volume(-1, 3);

        if (movement)
        {
            music += 1;

            int tileType = mGame.getTileType(mBox.x + mBox.w / 2, mBox.y + mBox.h / 2);
            if (tileType == 1)
            {
                music = music % 30;
                if (music == 0)
                {
                    // Mix_PlayChannel(-1, mGrassMusic, 0);
                }
            }
            else
            {
                music = music % 5;
                if (music == 0)
                {
                    // Mix_PlayChannel(-1, mMovementMusic, 0);
                }
            }
        }

        Mix_Volume(-1, MIX_MAX_VOLUME);
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
    if (isBusy() and hasTaskAnimation)
    {
        taskAnimation->setBox(mBox.x, mBox.y);
        taskAnimation->render(renderer, camera);
    }
    else
    {
        if (hasYulu())
            yuluTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &playerImages[dimension + offset]);
        else
            mTexture.render(renderer, mBox.x - camera.x, mBox.y - camera.y, &playerImages[dimension + offset]);
    }
    mframes = (mframes + 1) % (numOfAnimationImages * animationSpeed);
    if (isBusy())
        mframes = 0;
    return 0;
}

void Player::cleanUp()
{
    mTexture.free();
    Mix_FreeChunk(mCollisionMusic);
    Mix_FreeChunk(mMovementMusic);
    Mix_FreeChunk(mGrassMusic);
    Mix_FreeChunk(mTaskCompleteMusic);
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
    if (health > gMaxPlayerHealth)
        health = gMaxPlayerHealth;
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
    return mVelX;
}

int Player::getYVel()
{
    return mVelY;
}

int Player::getMoveFactor()
{
    return moveFactor;
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

std::string Player ::getHostelName()
{

    return hostelName;
}

void Player::update()
{

    if (yuluTimer.isStarted())
    {
        moveFactor = 2;
        if (yuluTimer.getTicks() / 1000 >= 2)
        {
            // std::cout << ((yuluTimer.getTicks() / 1000) - 50) << std::endl;
            money -= 4;
            yuluTimer.stop();
            yuluTimer.start();
            // std::cout << money << std::endl;
        }
    }
    else
    {
        moveFactor = 1;
    }

    if (healthTimer.isStarted() and hasYulu())
    {
        if (healthTimer.getTicks() / 1000 >= secondPerHealthDecreasewithYulu)
        {
            health -= 1;
            healthTimer.stop();
            healthTimer.start();
        }
    }
    else if (healthTimer.isStarted())
    {
        if (healthTimer.getTicks() / 1000 >= secondPerHealthDecrease)
        {
            health -= 1;
            healthTimer.stop();
            healthTimer.start();
        }
    }

    if (currentTaskTimer.getTicks() > currentTaskTime)
    {
        currentTaskTime = 0;
        currentTaskTimer.stop();
        updateStateParameters(updateState);
        updateState = {0.0, 0, 0, 0};
        taskText = "";
        hasTaskAnimation = false;
        // update player stats
    }
    if (health < 0)
    {
        resetPlayer();
        // send to hospital
        mBox.x = 26 * 32;
        mBox.y = 64 * 32;
        health = gMaxPlayerHealth;
        points -= 10;
    }
    if (money < 0)
    {
        points -= 10;
        money = gMaxPlayerMoney;
    }
}

void Player::toggleYulu()
{
    if (yuluTimer.isStarted())
    {
        yuluTimer.stop();
        health -= ((healthTimer.getTicks() / 1000.0) / secondPerHealthDecreasewithYulu);
    }
    else
    {
        yuluTimer.start();
        health -= ((healthTimer.getTicks() / 1000.0) / secondPerHealthDecrease);
    }
    healthTimer.stop();
    healthTimer.start();
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

LGame &Player ::getGame()
{
    return mGame;
}

void Player::resetHealth()
{
    health = gMaxPlayerHealth;
}

LTimer &Player ::getCurrentTaskTimer()
{
    return currentTaskTimer;
}

int Player ::getCurrentTaskTime()
{
    return currentTaskTime;
}

void Player::setCurrentTaskTime(int t)
{
    currentTaskTime = t;
}

bool Player::isBusy()
{
    if (currentTaskTimer.isStarted() and (currentTaskTimer.getTicks() < currentTaskTime))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Player::changeStateParameters(playerStateUpdate s)
{
    setHealth(getHealth() + s.health);
    setPoints(getPoints() + s.points);
    setMoney(getMoney() + s.money);
}

void Player::updateStateParameters(playerStateUpdate s)
{
    setHealth(getHealth() + s.health);
    setPoints(getPoints() + s.points);
    setMoney(getMoney() + s.money);
    if (mGame.hasTask(lastTileType))
    {
        setPoints(getPoints() + s.pointsIfTask);
        mGame.replaceTask(lastTileType);
        Mix_PlayChannel(-1, mTaskCompleteMusic, 0);
    }
}

void Player::setUpdateStateParameters(playerStateUpdate s)
{
    updateState = s;
}

bool Player ::hadBreakFast()
{
    return breakfast;
}

void Player ::setBreakfast(bool b)
{
    breakfast = b;
}

bool Player ::hadLunch()
{
    return lunch;
}

void Player ::setLunch(bool l)
{
    lunch = l;
}

bool Player ::hadDinner()
{
    return dinner;
}

void Player ::setDinner(bool d)
{
    dinner = d;
}

std::string Player ::getTaskText()
{
    return taskText;
}

void Player ::setTaskText(std ::string s)
{
    taskText = s;
}

void Player::setTaskAnimation(Animation *a)
{
    taskAnimation = a;
    hasTaskAnimation = true;
}

void Player::setCoords(int x, int y)
{
    mBox.x = x;
    mBox.y = y;
}

void Player::setVel(int velX, int velY)
{
    mVelX = velX;
    mVelY = velY;
}