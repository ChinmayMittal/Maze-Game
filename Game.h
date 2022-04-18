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
#include <vector>
#include "NPC.h"

typedef std::pair<int, std::string> task;

class LGame : public LScreen
{
private:
    const std::vector<std::pair<int, std::string>> allTasks = {
        task(13, "Play Volleyball"),
        task(14, "PLay Tennis"),
        task(15, "Swim"),
        task(22, "Buy stationery from the shop"),
        task(25, "Attend dance practice in SAC"),
        task(26, "PLay Football"),
        task(27, "Play Basketball"),
        task(28, "Do Athletics"),
        task(29, "Play Cricket"),
        task(30, "Attend class in LHC"),
        task(31, "Take ID Card from security office"),
        task(32, "Get documents verified from main building"),
        task(33, "Meet professor in biotech building"),
        task(34, "Go to the library to study"),
        task(38, "Buy vegetables"),
        task(39, "Buy bread"),
        task(40, "Buy tools"),
        task(41, "Get a haircut")};

    std::vector<std::pair<int, std::string>> currTasks;
    int tasksNum = 5;

    // std::string serverIp;
    std::string playerName, opponentName;

    int &sockfd;
    struct sockaddr_in &theirAddr;
    char buf[512];
    char recBuf[512];
    void initSocket();

    std::vector<Tile> tiles;
    std::vector<Player> players;
    std::vector<Entity> entities;
    std::vector<NPC> NPCs;
    SDL_Rect camera;
    // for instructions on object collision
    std::string displayText;
    std::vector<Renderable *> renderables;

    LTexture tilesTexture;
    LTexture ashTexture;
    LTexture dogTexture;
    LTexture profTexture;
    LTexture ash2Texture;
    LTexture robberTexture ; 
    LTexture yuluTexture ; 

    TileAtlas tileAtlas;

    int mTilesX;
    int mTilesY;
    int mTileWidth;
    int mTileHeight;
    int mMaxPlayerHealth;
    bool initObjs();
    void initTasks();
    bool setTiles();
    void initEntities();
    void gameEnd();

    LTimer globalTime;
    Text *nameText;
    Text *oppText;
    Text *timeText;
    Text *healthText;
    Text *moneyText;
    Text *pointsText;
    Text *moneyTextOpp;
    Text *pointsTextOpp;
    // for instructions on object collision
    Text *prompText;
    Text *tasksText;
    Text *winText;
    Text *loseText;
    Text *pressAnyKey;

    std::vector<Text *> taskTexts;

    bool gameEnded = false;
    bool waitingEnd = false;
    bool result = false;

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
    Animation *footballAnimation;
    LTimer getTimer();
    Mix_Music *backGroundMusic;
    Mix_Chunk *introMusic;
    bool hasTask(std::pair<int, std::string> task);
    bool hasTask(int task);
    void replaceTask(std::pair<int, std::string> task);
    void replaceTask(int task);
    int getTileType(int x, int y);
};

#endif