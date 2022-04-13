#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Tile.h"
#include "MyTexture.h"
#include "Renderable.h"
#include "MyWindow.h"
#include "SoundEffect.h"
#include <string>

class LGame;

class Player : public Renderable
{
public:
    // Initializes the variables

    Player(LTexture &myTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom);

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event &e);
    // void handleEventMoving(SDL_Event &e) ;
    // Moves the dot and check collision against tiles
    void move();

    // Centers the camera over the dot
    void setCamera(SDL_Rect &camera);

    void setVelocity(int vel);

    // Shows the dot on the screen
    int render(SDL_Renderer *renderer, SDL_Rect &camera);
    // int renderMoving(SDL_Renderer *renderer, SDL_Rect &camera);

    void cleanUp();

    SDL_Rect getBox();

private:
    // Collision box of the dot

    LGame &mGame;
    SDL_Rect mBox;
    LTexture &mTexture;
    SoundEffect wallCollisionMusic;
    // bool touchesWall(std::vector<Tile> tiles);
    int playerHeight, playerWidth;
    int right, top, left, bottom;
    int mframes;
    char direction;
    int numOfAnimationImages;
    int animationSpeed = 12;
    std::vector<SDL_Rect> playerImages;
    // Maximum axis velocity of the dot
    int velocity;
    // The velocity of the dot
    int mVelX, mVelY;
};

#endif