#ifndef GAME_H
#define GAME_H

#include "Screen.h"

#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "Tile.h"
#include "Player.h"
#include "MyTexture.h"
#include "TileAtlas.h"
#include "Renderable.h"
#include "Entity.h"
#include "Timer.h"
#include "Text.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

class LGame : public LScreen
{
private:
    std::string serverIp;
    std::string playerName, opponentName;

    int sockfd;
    struct sockaddr_in theirAddr;
    char buf[512];
    char recBuf[512];
    void initSocket();

    bool firstTime = true;

    std::vector<Tile> tiles;
    std::vector<Player> players;
    std::vector<Entity> entities;
    SDL_Rect camera;
    // for instructions on object collision
    std::string displayText;
    std::vector<Renderable *> renderables;

    LTexture tilesTexture;
    LTexture ashTexture;

    TileAtlas tileAtlas;

    int mTilesX;
    int mTilesY;
    int mTileWidth;
    int mTileHeight;
    int mMaxPlayerHealth;
    bool initObjs();
    bool setTiles();
    void initEntities();

    LTimer globalTime;
    Text *timeText;
    Text *healthText;
    Text *moneyText;
    Text *pointsText;
    // for instructions on object collision
    Text *prompText;

public:
    LGame(LWindow &window, std::string playerName, std::string opponentName, int &sockfd, sockaddr_in &theiraddr);
    void handleEvent(SDL_Event &e);
    void update();
    void render(SDL_Renderer *renderer);
    void cleanUp();
    SDL_Rect getTileClip(int tileType);
    int getLevelWidth();
    int getLevelHeight();
    int getWindowWidth();
    int getWindowHeight();
    Animation *sleepingAnimation;
    Animation *burgerAnimation;
    Animation *icecreamAnimation;
    Animation *hotDogAnimation;
    Animation *tennisAnimation;
    Animation *basketballAnimation;
    LTimer getTimer();
    Mix_Music *backGroundMusic;
    Mix_Chunk *introMusic;
};

#endif