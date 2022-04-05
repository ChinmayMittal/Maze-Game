#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include "Tile.h"
#include "MyTexture.h"
#include "Renderable.h"
#include "MyWindow.h"

class Dot : public Renderable
{
public:
    // The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    // Maximum axis velocity of the dot
    static const int DOT_VEL = 10;

    // Initializes the variables
    Dot(LTexture &myTexture, LWindow &window);

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event &e);

    // Moves the dot and check collision against tiles
    void move();

    // Centers the camera over the dot
    void setCamera(SDL_Rect &camera);

    // Shows the dot on the screen
    int render(SDL_Renderer *renderer, SDL_Rect &camera);

    void cleanUp();

    SDL_Rect mBox;
    LTexture &mTexture;

private:
    // Collision box of the dot

    LWindow &mWindow;

    // bool touchesWall(std::vector<Tile> tiles);

    // The velocity of the dot
    int mVelX, mVelY;
};

#endif