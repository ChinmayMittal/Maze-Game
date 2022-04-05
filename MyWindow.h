#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>

#include "MyTexture.h"
#include "Renderable.h"

class LWindow
{
public:
    // Intializes internals
    LWindow();

    // Creates window
    bool init(int width, int height);
    bool loadTileset(std::string path);
    bool initGameObjs();

    // Creates renderer from internal window
    int render(std::vector<Renderable *> objects, SDL_Rect &camera);

    // Handles window events
    void handleEvent(SDL_Event &e);

    // Deallocates internals
    void free();

    // Window dimensions
    int getWidth();
    int getHeight();

    int getLevelWidth();
    int getLevelHeight();

    // Window focii
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();

    bool loadTexture(LTexture &texture, std::string path);

    void cleanUp();
    void setParams(int tilesX, int tilesY, int tileWidth, int tileHeight);

private:
    // Window data
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;

    // Window dimensions
    int mWidth;
    int mHeight;

    // Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullScreen;
    bool mMinimized;

    int mTilesX;
    int mTilesY;
    int mTileWidth;
    int mTileHeight;
};

#endif