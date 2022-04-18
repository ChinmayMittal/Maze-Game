#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "Tile.h"
#include "MyTexture.h"
#include "Renderable.h"
#include "MyWindow.h"
#include "SoundEffect.h"
#include "Timer.h"
#include "utilities.h"
#include <string>
#include "Animation.h"

class LGame;

class Player : public Renderable
{
public:
    // Initializes the variables

    Player(LTexture &myTexture, LTexture &yuluTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom);

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event &e);
    // void handleEventMoving(SDL_Event &e) ;
    // Moves the dot and check collision against tiles
    void move();

    void moveBy(int offsetX, int offsetY);

    // Centers the camera over the dot
    void setCamera(SDL_Rect &camera);

    void setVelocity(int vel);

    // Shows the dot on the screen
    int render(SDL_Renderer *renderer, SDL_Rect &camera);
    // int renderMoving(SDL_Renderer *renderer, SDL_Rect &camera);

    void cleanUp();

    SDL_Rect getBox();
    int getXVel();
    int getYVel();
    int getMoveFactor();
    int getHealth();
    int getMoney();
    int getPoints();
    void setHealth(float h);
    void setPoints(int p);
    void setMoney(int m);
    void setMoveFactor(int factor);
    void setCoords(int x, int y);
    void setVel(int velX, int velY);
    void update();
    void toggleYulu();
    LGame &getGame();
    void setLastTileType(int tileType);
    int getLastTileType();
    std ::string getHostelName();
    void resetHealth();
    bool hasYulu();
    LTimer &getCurrentTaskTimer();
    int getCurrentTaskTime();
    void setCurrentTaskTime(int t);
    bool isBusy();
    void updateStateParameters(playerStateUpdate s);
    void setUpdateStateParameters(playerStateUpdate s);
    void changeStateParameters(playerStateUpdate s);
    bool hadLunch();
    bool hadBreakFast();
    bool hadDinner();
    void setBreakfast(bool b);
    void setLunch(bool l);
    void setDinner(bool d);
    std ::string getTaskText();
    void setTaskText(std::string s);
    void setTaskAnimation(Animation *a);
    int music = 0;
    void resetPlayer();

private:
    // Collision box of the dot

    LGame &mGame;
    SDL_Rect mBox;
    LTexture &mTexture;
    LTexture &yuluTexture;
    Animation *taskAnimation;
    bool hasTaskAnimation;
    // bool touchesWall(std::vector<Tile> tiles);
    int playerHeight, playerWidth;
    int right, top, left, bottom;
    int mframes;
    char direction;
    int numOfAnimationImages;
    int animationSpeed = 12;
    int money, points;
    float health;
    std ::string hostelName;
    std::vector<SDL_Rect> playerImages;
    // Maximum axis velocity of the dot
    int velocity;
    int moveFactor;
    // The velocity of the dot
    int mVelX, mVelY;
    int lastTileType = -1;
    LTimer yuluTimer;
    LTimer healthTimer;
    LTimer currentTaskTimer;
    int currentTaskTime;
    playerStateUpdate updateState;
    bool breakfast, lunch, dinner;
    std::string taskText;
    Mix_Chunk *mCollisionMusic;
    Mix_Chunk *mMovementMusic;
    Mix_Chunk *mGrassMusic;
    Mix_Chunk *mTaskCompleteMusic;
};

#endif