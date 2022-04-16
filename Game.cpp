#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "rapidxml.hpp"
#include "constants.h"
#include "Game.h"
#include "MyWindow.h"
#include "Screen.h"
#include "Entity.h"
#include <functional>
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
#include "MessageStructs.h"

LGame::LGame(LWindow &window, std::string playerName, std::string opponentName, int &sockfd, sockaddr_in &theiraddr) : LScreen(window), playerName(playerName), opponentName(opponentName), sockfd(sockfd), theirAddr(theiraddr)
{
    std::cout << "Name: " << playerName << " Opponent: " << opponentName << std::endl;
    backGroundMusic = Mix_LoadMUS("resources/music.mpeg");
    introMusic = Mix_LoadWAV("resources/intro.wav");
    Mix_PlayChannel(-1, introMusic, 0);
    Mix_PlayMusic(backGroundMusic, -1);
    initObjs();
    // initSocket();
}

void LGame::initSocket()
{
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // memset(&myAddr, 0, sizeof(myAddr));
    memset(&theirAddr, 0, sizeof(theirAddr));

    // myAddr.sin_family = AF_INET; // IPv4
    // myAddr.sin_addr.s_addr = INADDR_ANY;
    // myAddr.sin_port = htons(2000);

    theirAddr.sin_family = AF_INET;
    theirAddr.sin_port = htons(8080);
    // theirAddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, serverIp.c_str(), &(theirAddr.sin_addr.s_addr));
}

void LGame::handleEvent(SDL_Event &e)
{
    players[0].handleEvent(e);
    int lastTileType = players[0].getLastTileType();
    if (lastTileType != -1)
    {
        entities[lastTileType].handleEvent(e, players[0]);
    }
}

void LGame::update()
{
    camera = {camera.x, camera.y, window.getWidth(), window.getHeight()};
    // Move the dot
    players[0].move();
    players[1].move();

    SDL_Rect playerBox = players[0].getBox();
    // std::cout << "After move " << playerBox.y << std::endl;
    int tileX = (playerBox.x + playerBox.w / 2) / mTileWidth;
    int tileY = (playerBox.y + playerBox.h / 2) / mTileHeight;
    int tileType = tiles[tileY * mTilesX + tileX].getType();
    // std::cout << "Tiletype " << tileType << std::endl;
    // stores the prompt text to be displayed

    entities[tileType].collided(players[0], displayText);

    // std::cout << "After collide " << playerBox.y << std::endl;
    players[0].setCamera(camera);

    players[0].update();
    SDL_Rect playerBoxNew = players[0].getBox();
    int newTileX = (playerBoxNew.x + playerBoxNew.w / 2) / mTileWidth;
    int newTileY = (playerBoxNew.y + playerBoxNew.h / 2) / mTileHeight;
    int newTileType = tiles[newTileY * mTilesX + newTileX].getType();
    // std::cout << newTileType << std::endl;
    players[0].setLastTileType(newTileType);

    int secs = globalTime.getTicks() / 1000;
    std::string mins = std::to_string(3 * (secs % 20));
    std::string hours = std::to_string((secs / 20));
    if (mins.size() <= 1)
        mins = "0" + mins;
    if (hours.size() <= 1)
        hours = "0" + hours;
    timeText->setText("TIME: " + hours + ":" + mins);
    pointsText->setText("POINTS: " + std::to_string(players[0].getPoints()));
    moneyText->setText("MONEY: " + std::to_string(players[0].getMoney()));
    healthText->setText("HEALTH:");
    if (displayText == "")
        displayText = std::string("INSTRUCTIONS WILL APPEAR HERE, YOUR HOSTEL IS ") + players[0].getHostelName();
    if (players[0].isBusy())
    {
        prompText->setText(players[0].getTaskText());
    }
    else
    {
        prompText->setText(displayText);
    }

    GameUpdateMessage gameUpdateMsg;
    gameUpdateMsg.x = players[0].getBox().x;
    gameUpdateMsg.y = players[0].getBox().y;
    gameUpdateMsg.velX = players[0].getXVel();
    gameUpdateMsg.velY = players[0].getYVel();
    gameUpdateMsg.moveFactor = players[0].getMoveFactor();
    gameUpdateMsg.money = players[0].getMoney();
    gameUpdateMsg.points = players[0].getPoints();
    // gameUpdateMsg.health = players[0].getHealth();
    int bytesUsed = serialize(&gameUpdateMsg, buf);

    sendto(sockfd, buf, bytesUsed, 0, (const struct sockaddr *)&theirAddr,
           sizeof(theirAddr));
    // std::cout << "SENT" << std::endl;

    unsigned int len = sizeof(theirAddr);
    int n = recvfrom(sockfd, (char *)recBuf, 512, MSG_WAITALL, (struct sockaddr *)&theirAddr, &len);

    // std::cout << n << std::endl;
    if (n != -1)
    {
        std::cout << "Received!" << std::endl;
        Message *msg = deserialize(recBuf);
        if (msg->type == 2)
        {
            GameUpdateMessage *updateMsg = dynamic_cast<GameUpdateMessage *>(msg);
            players[1].setCoords(updateMsg->x, updateMsg->y);
            players[1].setVel(updateMsg->velX, updateMsg->velY);
            players[1].setMoveFactor(updateMsg->moveFactor);
            players[1].setPoints(updateMsg->points);
            players[1].setMoney(updateMsg->money);
            // players[1].setHealth(updateMsg->health);
        }
        delete msg;
    }
    else
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
    }
}

void LGame::render(SDL_Renderer *renderer)
{
    SDL_Rect viewport;
    viewport.x = 0;
    viewport.y = gyRenderOffset;
    viewport.w = window.getWidth();
    viewport.h = window.getHeight() - gyRenderOffset - gyRenderOffset;
    SDL_RenderSetViewport(renderer, &viewport);

    for (size_t i = 0; i < renderables.size(); i++)
    {

        int res = renderables[i]->render(renderer, camera);

        if (res != 0)
        {
            // Handle Error
        }
    }

    viewport.x = 0;
    viewport.y = 0;
    viewport.w = window.getWidth();
    viewport.h = gyRenderOffset;
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

    int offset = timeText->getWidth() + 32;
    timeText->render(renderer, window.getWidth() - offset, gyTextOffset);
    offset += gMaxPlayerHealth + gxTextSpacing;
    SDL_Rect fillRect = {window.getWidth() - offset, gyTextOffset, (players[0].getHealth() * (gMaxPlayerHealth) / mMaxPlayerHealth), 32};
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_Rect outlineRect = {window.getWidth() - offset, gyTextOffset, gMaxPlayerHealth, 32};

    SDL_RenderDrawRect(renderer, &outlineRect);

    offset += healthText->getWidth() + 10;
    healthText->render(renderer, window.getWidth() - offset, gyTextOffset);

    offset += moneyText->getWidth() + gxTextSpacing;
    moneyText->render(renderer, window.getWidth() - offset, gyTextOffset);

    offset += pointsText->getWidth() + gxTextSpacing;
    pointsText->render(renderer, window.getWidth() - offset, gyTextOffset);

    viewport.x = 0;
    viewport.y = window.getHeight() - gyRenderOffset;
    viewport.w = window.getWidth();
    viewport.h = gyRenderOffset;
    SDL_RenderSetViewport(renderer, &viewport);
    prompText->render(renderer, 0, 0);
    if (players[0].isBusy())
    {
        SDL_SetRenderDrawColor(renderer, 0xFd, 0xb3, 0x36, 0xFF);
        offset = taskStatusBarWidth + 10;
        SDL_Rect fillRect = {window.getWidth() - offset, gyTextOffset, ((int)players[0].getCurrentTaskTimer().getTicks() * (taskStatusBarWidth)) / (players[0].getCurrentTaskTime()), gyRenderOffset};
        SDL_RenderFillRect(renderer, &fillRect);
        SDL_Rect outlineRect = {window.getWidth() - offset, gyTextOffset, taskStatusBarWidth, gyRenderOffset};
        SDL_RenderDrawRect(renderer, &outlineRect);
    }
}

void LGame::cleanUp()
{
    players[0].cleanUp();
    players[1].cleanUp();
    tiles[0].cleanUp();
    delete timeText;
    delete sleepingAnimation;
    Mix_FreeMusic(backGroundMusic);
    Mix_FreeChunk(introMusic);
    backGroundMusic = NULL;
    close(sockfd);
}

bool LGame::initObjs()
{

    if (!window.loadTexture(ashTexture, "resources/ash.bmp"))
    {
        printf("Failed to load ash texture!\n");
        return false;
    }
    LTexture *sleepingAnimationTexture = new LTexture();
    LTexture *burgerAnimationTexture = new LTexture();
    LTexture *hotdogAnimationTexture = new LTexture();
    LTexture *icecreamAnimationTexture = new LTexture();
    LTexture *tennisAnimationTexture = new LTexture();
    LTexture *basketBallAnimationTexture = new LTexture();
    if (!window.loadTexture(*sleepingAnimationTexture, "resources/sleeping.png"))
    {
        printf("Failed to load sleeping texture!\n");
        return false;
    }
    if (!window.loadTexture(*burgerAnimationTexture, "resources/burger.png"))
    {
        printf("Failed to load burger texture!\n");
        return false;
    }
    if (!window.loadTexture(*hotdogAnimationTexture, "resources/hotdog.png"))
    {
        printf("Failed to load hotdog texture!\n");
        return false;
    }
    if (!window.loadTexture(*icecreamAnimationTexture, "resources/icecream.png"))
    {
        printf("Failed to load icecream texture!\n");
        return false;
    }
    if (!window.loadTexture(*tennisAnimationTexture, "resources/tennis.png"))
    {
        printf("Failed to load tennis texture!\n");
        return false;
    }
    if (!window.loadTexture(*basketBallAnimationTexture, "resources/basketball.png"))
    {
        printf("Failed to load basketball texture!\n");
        return false;
    }
    sleepingAnimation = new Animation(*sleepingAnimationTexture, 32, 32);
    burgerAnimation = new Animation(*burgerAnimationTexture, 32, 32);
    hotDogAnimation = new Animation(*hotdogAnimationTexture, 32, 32);
    icecreamAnimation = new Animation(*icecreamAnimationTexture, 32, 32);
    basketballAnimation = new Animation(*basketBallAnimationTexture, 32, 32);
    tennisAnimation = new Animation(*tennisAnimationTexture, 32, 32);

    Player ash(ashTexture, *this, 32, 32, 3, 1, 2, 0);
    players.push_back(ash);

    Player opponent(ashTexture, *this, 32, 32, 3, 1, 2, 0);
    players.push_back(opponent);

    camera = {0, 0, window.getWidth(), window.getHeight()};

    if (!window.loadTexture(tilesTexture, "resources/tileset.png"))
    {
        printf("Failed to load tile set texture!\n");
        return false;
    }

    // Load tile map
    if (!setTiles())
    {
        printf("Failed to load tile set!\n");
        return false;
    }

    int uniqueTilesX = tilesTexture.getWidth() / tiles[0].getBox().w;
    int uniqueTilesY = tilesTexture.getHeight() / tiles[0].getBox().h;

    tileAtlas.init(uniqueTilesX, uniqueTilesY, tiles[0].getBox().w, tiles[0].getBox().h);

    initEntities();

    SDL_Color txtColor = {0, 0, 0, 255};
    TTF_Font *font = TTF_OpenFont("resources/FrostbiteBossFight-dL0Z.ttf", 28);
    timeText = new Text(window, "00:00", font, txtColor);
    healthText = new Text(window, "HEALTH :: ", font, txtColor);
    moneyText = new Text(window, "MONEY:: ", font, txtColor);
    pointsText = new Text(window, "POINTS:: ", font, txtColor);
    displayText = "YOU HAVE BEEN ASSIGNED HOSTEL " + players[0].getHostelName();
    prompText = new Text(window, displayText, font, txtColor);

    for (int i = 0; i < tiles.size(); i++)
    {
        renderables.push_back(&tiles[i]);
    }
    for (int i = 0; i < players.size(); i++)
    {
        renderables.push_back(&players[i]);
    }

    globalTime.start();
    return true;
}

bool LGame::setTiles()
{
    // Open the map
    std::ifstream mapStream("resources/mapFinal.tmx");
    std::ifstream mapTypeStream("resources/mapTypes.tmx");
    // If the map couldn't be loaded
    if (mapStream.fail())
    {
        printf("Unable to load map file!\n");
        return false;
    }
    if (mapTypeStream.fail())
    {
        printf("Unable to load type map file!\n");
        return false;
    }

    std::stringstream buffer, mapTypeBuffer;
    buffer << mapStream.rdbuf();
    mapTypeBuffer << mapTypeStream.rdbuf();

    std::string xmlText = buffer.str();
    std ::string mapTypeXMLText = mapTypeBuffer.str();

    rapidxml::xml_document<> doc, typedoc; // character type defaults to char
    doc.parse<0>(const_cast<char *>(xmlText.c_str()));
    typedoc.parse<0>(const_cast<char *>(mapTypeXMLText.c_str()));

    rapidxml::xml_node<> *mapNode = doc.first_node("map");
    rapidxml::xml_node<> *typeMapNode = typedoc.first_node("map");

    int tileWidth = std::stoi(mapNode->first_attribute("tilewidth")->value());
    int tileHeight = std::stoi(mapNode->first_attribute("tileheight")->value());

    int numTilesX = std::stoi(mapNode->first_attribute("width")->value());
    int numTilesY = std::stoi(mapNode->first_attribute("height")->value());

    std::string tilesData = mapNode->first_node("layer")->first_node("data")->value();
    std ::string tilesTypeData = typeMapNode->first_node("layer")->first_node("data")->value();
    int j = 0, k = 0;
    // Initialize the tiles
    for (int i = 0; i < numTilesX * numTilesY; ++i)
    {
        // Read tile from map file
        std::string token, typeToken;
        while (j < tilesData.size() && tilesData[j] != ',')
        {
            token += tilesData[j];
            j++;
        }
        while (k < tilesTypeData.size() && tilesTypeData[k] != ',')
        {
            typeToken += tilesTypeData[k];
            k++;
        }
        j++;
        k++;

        int tileID = std::stoi(token);
        int tileType = std::stoi(typeToken) - 1;

        Tile myTile(tilesTexture, *this, (i % numTilesX) * tileWidth, (i / numTilesX) * tileHeight, tileWidth, tileHeight, tileID);
        myTile.setType(tileType);
        tiles.push_back(myTile);
    }

    // Close the files
    mapStream.close();
    mapTypeStream.close();

    mTilesX = numTilesX;
    mTilesY = numTilesY;
    mTileWidth = tileWidth;
    mTileHeight = tileHeight;
    mMaxPlayerHealth = gMaxPlayerHealth;

    return true;
}

std::function<void(Player &player, std::string &displayText)> getTextPromptFunc(std ::string a)
{
    return [=](Player &player, std::string &displayText)
    {
        displayText = a;
    };
}
std::function<void(Player &player, std::string &displayText)> getHostelCollideFunc(std ::string hostelName)
{
    return [=](Player &player, std::string &displayText)
    {
        if (player.getHostelName() == hostelName)
        {
            displayText = "YOUR HOSTEL,  ";
            displayText += " B-BreakFast  L-Lunch  D-Dinner  R-REST";
        }
        else
        {
            displayText = "NEIGHBOURING HOSTEL";
        }
    };
}

std ::function<void(SDL_Event &e, Player &player)> getHostelEventListener(std ::string HostelName)
{

    return [=](SDL_Event &e, Player &player)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && player.getHostelName() == HostelName)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_r:
                // create rest task if player is not busy
                if (!player.isBusy())
                {
                    player.setCurrentTaskTime(10000);
                    player.getCurrentTaskTimer().start();
                    player.setTaskText("resting ... ");
                    player.setTaskAnimation(player.getGame().sleepingAnimation);
                    player.setUpdateStateParameters({gMaxPlayerHealth,
                                                     0,
                                                     0});
                }
                break;
            case SDLK_b:
                if (!player.isBusy() and !player.hadBreakFast() and (player.getGame().getTimer().getTicks() < 100 * 1000))
                {
                    player.setBreakfast(true);
                    player.setTaskText("having breakfast... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({30,
                                                     0,
                                                     0});
                }
                break;
            case SDLK_l:
                if (!player.isBusy() and !player.hadLunch() and (player.getGame().getTimer().getTicks() < 100 * 1000))
                {
                    player.setLunch(true);
                    player.setTaskText("having lunch ... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({30,
                                                     0,
                                                     0});
                }
                break;
            case SDLK_d:
                if (!player.isBusy() and !player.hadDinner() and (player.getGame().getTimer().getTicks() < 100 * 1000))
                {
                    player.setDinner(true);
                    player.setTaskText("having dinner... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({30,
                                                     0,
                                                     0});
                }
                break;
            }
        }
    };
}
void LGame::initEntities()
{
    Entity road("Road", [&](Player &player, std::string &displayText)
                {
                    player.setVelocity(2);
                    displayText = ""; });
    Entity grass("Grass", [&](Player &player, std::string &displayText)
                 { 
                     player.setVelocity(1); 
                     displayText = "" ; });
    Entity pavement("Pavement", [&](Player &player, std::string &displayText)
                    { 
                    player.setVelocity(2); 
                    displayText = "" ; });
    Entity no_movement("no_movement", [&](Player &player, std::string &displayText)
                       { 
                           player.moveBy(-player.getXVel() * player.getMoveFactor(), -player.getYVel() * player.getMoveFactor());
                           displayText = "" ; });

    Entity yulu(
        "yulu", [&](Player &player, std::string &displayText)
        {
        // if (player.getLastTileType() != 4)
        // {
        //     //player.toggleYulu();
        //     // std::cout << "Toggled" << std::endl;
        // }
        if(player.hasYulu()){
            displayText = "Press y to release yulu";
        }else
        {
            displayText = "Press y to take yulu";
        } },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_y:
                    player.toggleYulu();
                    break;
                }
            }
        });

    Entity nilgiri("nilgiri", getHostelCollideFunc("nilgiri"), getHostelEventListener("nilgiri"));
    Entity kara("kara", getHostelCollideFunc("kara"), getHostelEventListener("kara"));
    Entity aravali("aravali", getHostelCollideFunc("aravali"), getHostelEventListener("aravali"));
    Entity jwala("jwala", getHostelCollideFunc("jwala"), getHostelEventListener("jwala"));
    Entity kumaon("kumaon", getHostelCollideFunc("kumaon"), getHostelEventListener("kumaon"));
    Entity vindy("vindy", getHostelCollideFunc("vindy"), getHostelEventListener("vindy"));
    Entity satpura("satpura", getHostelCollideFunc("satpura"), getHostelEventListener("satpura"));
    Entity udai_girnar("udai_girnar", getHostelCollideFunc("udai_girnar"), getHostelEventListener("udai_girnar"));
    Entity himadri("himadri", getHostelCollideFunc("himadri"), getHostelEventListener("himadri"));
    Entity kailash("kailash", getHostelCollideFunc("kailash"), getHostelEventListener("kailash"));
    Entity shivalik("shivalik", getHostelCollideFunc("shivalik"), getHostelEventListener("shivalik"));
    Entity zanskar("zanskar", getHostelCollideFunc("zanskar"), getHostelEventListener("zanskar"));
    Entity volleyball("volleyball", [&](Player &player, std::string &displayText)
                      { displayText = "volleyball"; });
    Entity tennis("tennis", getTextPromptFunc("PRESS T TO PLAY TENNIS"),
                  [&](SDL_Event &e, Player &player)
                  {
                      if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                      {
                          switch (e.key.keysym.sym)
                          {
                          case SDLK_t:
                              player.setTaskText("playing tennis... ");
                              player.setTaskAnimation(player.getGame().tennisAnimation);
                              player.setCurrentTaskTime(5000);
                              player.getCurrentTaskTimer().start();
                              player.setUpdateStateParameters({0,
                                                               0,
                                                               0});
                              break;
                          }
                      }
                  });
    Entity swimming_pool("swimming_pool", [&](Player &player, std::string &displayText)
                         { displayText = "swimming_pool"; });
    Entity oat("oat", [&](Player &player, std::string &displayText)
               { displayText = "oat"; });
    Entity hot_dog("hot_dog", getTextPromptFunc("PRESS H for HOTDOG"),
                   [&](SDL_Event &e, Player &player)
                   {
                       if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                       {
                           switch (e.key.keysym.sym)
                           {
                           case SDLK_h:
                               if (player.getMoney() > 20)
                               {
                                   player.setTaskText("having hotdog... ");
                                   player.setTaskAnimation(player.getGame().hotDogAnimation);
                                   player.setCurrentTaskTime(3000);
                                   player.getCurrentTaskTimer().start();
                                   player.setUpdateStateParameters({20,
                                                                    -20,
                                                                    0});
                               }
                               break;
                           }
                       }
                   });
    Entity gas("gas", [&](Player &player, std::string &displayText)
               { displayText = "gas"; });
    Entity icecream("icecream", getTextPromptFunc("PRESS I for ICECREAM"),
                    [&](SDL_Event &e, Player &player)
                    {
                        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                        {
                            switch (e.key.keysym.sym)
                            {
                            case SDLK_i:
                                if (player.getMoney() > 10)
                                {
                                    player.setTaskText("having icecream... ");
                                    player.setCurrentTaskTime(5000);
                                    player.getCurrentTaskTimer().start();
                                    player.setTaskAnimation(player.getGame().icecreamAnimation);
                                    player.setUpdateStateParameters({10,
                                                                     -10,
                                                                     0});
                                }
                                break;
                            }
                        }
                    });
    Entity shop("shop", [&](Player &player, std::string &displayText)
                { displayText = "shop"; });
    Entity sac("sac", [&](Player &player, std::string &displayText)
               { displayText = "sac"; });
    Entity foot("foot", [&](Player &player, std::string &displayText)
                { displayText = "foot"; });
    Entity basketball("basketball", getTextPromptFunc("PRESS B TO PLAY BASKTEBALL"),
                      [&](SDL_Event &e, Player &player)
                      {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_b:
                                  player.setTaskText("having hotdog... ");
                                  player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(3000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({0,
                                                                   0,
                                                                   0});
                                  break;
                              }
                          }
                      });
    Entity athletic("athletic", [&](Player &player, std::string &displayText)
                    { displayText = "athletic"; });
    Entity cricket("cricket", [&](Player &player, std::string &displayText)
                   { displayText = "cricket"; });
    Entity lhc("lhc", [&](Player &player, std::string &displayText)
               { displayText = "lhc"; });
    Entity police("police", [&](Player &player, std::string &displayText)
                  { displayText = "police"; });
    Entity main_building("main_building", [&](Player &player, std::string &displayText)
                         { displayText = "main_building"; });
    Entity biotech_dept("biotech_dept", [&](Player &player, std::string &displayText)
                        { displayText = "biotech_dept"; });
    Entity library("library", [&](Player &player, std::string &displayText)
                   { displayText = "library"; });
    Entity coffee("coffee", [&](Player &player, std::string &displayText)
                  { displayText = "coffee"; });
    Entity hospital("hospital", [&](Player &player, std::string &displayText)
                    { displayText = "hospital"; });
    Entity burger("burger", getTextPromptFunc("PRESS B for BURGER"), [&](SDL_Event &e, Player &player)
                  {
                        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                        {
                            switch (e.key.keysym.sym)
                            {
                            case SDLK_b:
                                if(player.getMoney() > 20 ) {
                                    player.setTaskText("having burger... ") ; 
                                    player.setCurrentTaskTime(6000) ; 
                                    player.getCurrentTaskTimer().start() ;
                                    player.setTaskAnimation( player.getGame().burgerAnimation ) ; 
                                    player.setUpdateStateParameters({
                                        20 , 
                                        -20 , 
                                        0 
                                    }) ;  
                                }
                                break;
                            }
                        } });
    Entity vegetable_shop("vegetable_shop", [&](Player &player, std::string &displayText)
                          { displayText = "vegetable_shop"; });
    Entity bread_shop("bread_shop", [&](Player &player, std::string &displayText)
                      { displayText = "bread_shop"; });
    Entity hardware_shop("hardware_shop", [&](Player &player, std::string &displayText)
                         { displayText = "hardware_shop"; });
    Entity barber("barber", [&](Player &player, std::string &displayText)
                  { displayText = "barber"; });
    Entity pharmacy("pharmacy", [&](Player &player, std::string &displayText)
                    { displayText = "pharmacy"; });
    Entity beverage_shop("beverage_shop", [&](Player &player, std::string &displayText)
                         { displayText = "beverage_shop"; });

    entities.push_back(road);
    entities.push_back(grass);
    entities.push_back(pavement);
    entities.push_back(no_movement);
    entities.push_back(yulu);
    entities.push_back(nilgiri);
    entities.push_back(kara);
    entities.push_back(aravali);
    entities.push_back(jwala);
    entities.push_back(kumaon);
    entities.push_back(vindy);
    entities.push_back(satpura);
    entities.push_back(udai_girnar);
    entities.push_back(volleyball);
    entities.push_back(tennis);
    entities.push_back(swimming_pool);
    entities.push_back(oat);
    entities.push_back(hot_dog);
    entities.push_back(himadri);
    entities.push_back(kailash);
    entities.push_back(gas);
    entities.push_back(icecream);
    entities.push_back(shop);
    entities.push_back(shivalik);
    entities.push_back(zanskar);
    entities.push_back(sac);
    entities.push_back(foot);
    entities.push_back(basketball);
    entities.push_back(athletic);
    entities.push_back(cricket);
    entities.push_back(lhc);
    entities.push_back(police);
    entities.push_back(main_building);
    entities.push_back(biotech_dept);
    entities.push_back(library);
    entities.push_back(coffee);
    entities.push_back(hospital);
    entities.push_back(burger);
    entities.push_back(vegetable_shop);
    entities.push_back(bread_shop);
    entities.push_back(hardware_shop);
    entities.push_back(barber);
    entities.push_back(pharmacy);
    entities.push_back(beverage_shop);
}

SDL_Rect LGame::getTileClip(int tileID)
{
    return tileAtlas.tileClips[tileID - 1];
}

int LGame::getLevelWidth()
{
    return mTilesX * mTileWidth;
}

int LGame::getLevelHeight()
{
    return mTilesY * mTileHeight;
}

int LGame::getWindowWidth()
{
    return window.getWidth();
}

int LGame::getWindowHeight()
{
    return window.getHeight();
}

LTimer LGame ::getTimer()
{
    return globalTime;
}