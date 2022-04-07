#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Tile.h"
#include "MyTexture.h"
#include "Renderable.h"
#include "MyWindow.h"
#include <string>

class LGame;

class Dot : public Renderable
{
public:
    int playerHeight, playerWidth;
    int right, top, left, bottom;
    int mframes;
    char direction;
    int numOfAnimationImages;
    int animationSpeed = 12;
    std::vector<SDL_Rect> playerImages;
    // Maximum axis velocity of the dot
    int DOT_VEL = 10;

    // Initializes the variables
    // Dot(LTexture &myTexture, LWindow &window);

    Dot(LTexture &myTexture, LGame &game, int playerHeight, int playerWidth, int right, int left, int top, int bottom, int setVelocity);

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event &e);
    // void handleEventMoving(SDL_Event &e) ;
    // Moves the dot and check collision against tiles
    void move();

    // Centers the camera over the dot
    void setCamera(SDL_Rect &camera);

    // Shows the dot on the screen
    int render(SDL_Renderer *renderer, SDL_Rect &camera);
    // int renderMoving(SDL_Renderer *renderer, SDL_Rect &camera);

    void cleanUp();

    SDL_Rect mBox;
    LTexture &mTexture;

private:
    // Collision box of the dot

    LGame &mGame;

    // bool touchesWall(std::vector<Tile> tiles);

    // The velocity of the dot
    int mVelX, mVelY;
};

#endif