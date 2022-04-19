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
#include <algorithm>
#include "MessageStructs.h"
#include "MainMenu.h"

LGame::LGame(LWindow &window, std::string playerName, std::string opponentName, int &sockfd, sockaddr_in &theiraddr) : LScreen(window), playerName(playerName), opponentName(opponentName), sockfd(sockfd), theirAddr(theiraddr)
{
    window.maximize();
    // std::cout << "Name: " << playerName << " Opponent: " << opponentName << std::endl;
    backGroundMusic = Mix_LoadMUS("resources/music.mpeg");
    introMusic = Mix_LoadWAV("resources/intro.wav");
    Mix_PlayChannel(-1, introMusic, 0);
    Mix_PlayMusic(backGroundMusic, -1);
    initObjs();
    initTasks();
    // initSocket();
}

void LGame::initTasks()
{
    for (int i = 0; i < tasksNum; i++)
    {
        std::vector<std::pair<int, std::string>>::iterator it;
        std::pair<int, std::string> t;
        do
        {
            t = allTasks[rand() % allTasks.size()];
            it = std::find(currTasks.begin(), currTasks.end(), t);
        } while (it != currTasks.end());
        currTasks.push_back(t);
        taskTexts[i]->setText(t.second);
    }
}

void LGame::handleEvent(SDL_Event &e)
{
    if (waitingEnd && !gameEnded)
    {
        return;
    }

    if (gameEnded)
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
        {
            if (e.key.keysym.sym == SDLK_SPACE)
            {
                MainMenu *mainMenu = new MainMenu(window, sockfd, theirAddr);
                window.setCurrScreen(mainMenu);
                break;
            }
        }
        default:
        {
            break;
        }
        }
        return;
    }

    players[0].handleEvent(e);
    int lastTileType = players[0].getLastTileType();
    if (lastTileType != -1)
    {
        entities[lastTileType].handleEvent(e, players[0]);
    }
}

void LGame::update()
{

    if (gameEnded)
    {
        return;
    }

    if (waitingEnd)
    {

        unsigned int len = sizeof(theirAddr);
        int n = recvfrom(sockfd, (char *)recBuf, 512, MSG_WAITALL, (struct sockaddr *)&theirAddr, &len);
        if (n != -1)
        {
            Message *msg = deserialize(recBuf);
            if (msg->type == 4)
            {
                GameResultMessage *gameResultMessage = dynamic_cast<GameResultMessage *>(msg);
                result = gameResultMessage->won;
                gameEnded = true;
            }
            delete msg;
        }
        return;
    }

    camera = {camera.x, camera.y, window.getWidth() - tasksVPWidth, window.getHeight() - 3 * gyRenderOffset - 5 * gyPadding};
    // Move the dot
    for (int i = 0; i < players.size(); i++)
    {
        players[i].move();
    }
    for (int i = 0; i < NPCs.size(); i++)
    {
        NPCs[i].move();
    }
    SDL_Rect playerBox = players[0].getBox();
    // std::cout << "After move " << playerBox.y << std::endl;
    int tileX = (playerBox.x + playerBox.w / 2) / mTileWidth;
    int tileY = (playerBox.y + playerBox.h / 2) / mTileHeight;
    int tileType = tiles[tileY * mTilesX + tileX].getType();
    // std::cout << "Tiletype " << tileType << std::endl;
    // stores the prompt text to be displayed
    for (int i = 0; i < NPCs.size(); i++)
    {
        SDL_Rect NPCbox = NPCs[i].getBox();
        int NPCtileX = (NPCbox.x + NPCbox.w / 2) / mTileWidth;
        int NPCtileY = (NPCbox.y + NPCbox.h / 2) / mTileHeight;
        // std::cout << NPCtileX << " " << NPCtileY << "\n";
        if (((tileX >= NPCtileX - 1) && (tileX <= NPCtileX + 1)) && ((tileY >= NPCtileY - 1) && (tileY <= NPCtileY + 1)) && !NPCs[i].isBusy() && !players[0].isBusy())
        {
            // std ::cout << "collision with " << NPCs[i].getName() << "\n";
            NPCs[i].setCoolDown(7000);
            if (NPCs[i].getName().substr(0, 3) == "dog")
            {
                players[0].changeStateParameters({-5,
                                                  0,
                                                  0});
            }
            else if (NPCs[i].getName().substr(0, 1) == "p")
            {
                players[0].changeStateParameters(
                    {0,
                     0,
                     -5});
            }else if( NPCs[i].getName().substr(0,1) == "r"){
                players[0].changeStateParameters(
                    {0,
                     -10,
                     0});                
            }
        }
    }
    entities[tileType].collided(players[0], displayText);

    // std::cout << "After collide " << playerBox.y << std::endl;
    players[0].setCamera(camera);

    players[0].update();
    for (int i = 0; i < NPCs.size(); i++)
    {
        NPCs[i].update();
    }
    SDL_Rect playerBoxNew = players[0].getBox();
    int newTileX = (playerBoxNew.x + playerBoxNew.w / 2) / mTileWidth;
    int newTileY = (playerBoxNew.y + playerBoxNew.h / 2) / mTileHeight;
    int newTileType = tiles[newTileY * mTilesX + newTileX].getType();
    // std::cout << newTileType << std::endl;
    players[0].setLastTileType(newTileType);

    int secs = globalTime.getTicks() / 1000;
    int secsPerHour = gameLenSecs / 24;
    std::string mins = std::to_string((60 / secsPerHour) * (secs % secsPerHour));
    std::string hours = std::to_string(secs / secsPerHour);

    if (mins.size() <= 1)
        mins = "0" + mins;
    if (hours.size() <= 1)
        hours = "0" + hours;
    timeText->setText("TIME: " + hours + ":" + mins);
    pointsText->setText("POINTS: " + std::to_string(players[0].getPoints()));
    moneyText->setText("MONEY: " + std::to_string(players[0].getMoney()));
    // healthText->setText("HEALTH: ");
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
    gameUpdateMsg.health = players[0].getHealth();
    // std::cout << "Sending health " << gameUpdateMsg.health << std::endl;
    int bytesUsed = serialize(&gameUpdateMsg, buf);

    sendto(sockfd, buf, bytesUsed, 0, (const struct sockaddr *)&theirAddr,
           sizeof(theirAddr));
    // std::cout << "SENT" << std::endl;

    unsigned int len = sizeof(theirAddr);
    int n = recvfrom(sockfd, (char *)recBuf, 512, MSG_WAITALL, (struct sockaddr *)&theirAddr, &len);

    // std::cout << n << std::endl;
    if (n != -1)
    {
        // std::cout << "Received!" << std::endl;
        Message *msg = deserialize(recBuf);
        if (msg->type == 2)
        {
            GameUpdateMessage *updateMsg = dynamic_cast<GameUpdateMessage *>(msg);
            players[1].setCoords(updateMsg->x, updateMsg->y);
            players[1].setVel(updateMsg->velX, updateMsg->velY);
            players[1].setMoveFactor(updateMsg->moveFactor);
            players[1].setPoints(updateMsg->points);
            players[1].setMoney(updateMsg->money);
            players[1].setHealth(updateMsg->health);
            // std::cout << "Receiving health " << gameUpdateMsg.health << std::endl;
        }
        delete msg;
    }
    else
    {
        // fprintf(stderr, "socket() failed: %s\n", strerror(errno));
    }
    pointsTextOpp->setText("POINTS: " + std::to_string(players[1].getPoints()));
    moneyTextOpp->setText("MONEY: " + std::to_string(players[1].getMoney()));

    if (secs / secsPerHour >= 24)
    {
        gameEnd();
        return;
    }
}

void LGame::gameEnd()
{
    GameEndMessage gameEndMessage;
    gameEndMessage.points = players[0].getPoints();
    gameEndMessage.money = players[0].getMoney();
    gameEndMessage.health = players[0].getHealth();
    int bytesUsed = serialize(&gameEndMessage, buf);

    sendto(sockfd, buf, bytesUsed, 0, (const struct sockaddr *)&theirAddr,
           sizeof(theirAddr));

    players[0].resetPlayer();
    players[1].resetPlayer();
    waitingEnd = true;
}

void LGame::render(SDL_Renderer *renderer)
{
    SDL_Rect viewport;
    viewport.x = 0;
    viewport.y = 2 * gyRenderOffset + 3 * gyPadding;
    viewport.w = window.getWidth() - tasksVPWidth;
    viewport.h = window.getHeight() - 3 * gyRenderOffset - 5 * gyPadding;
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
    viewport.h = 2 * gyRenderOffset + 3 * gyPadding;
    SDL_RenderSetViewport(renderer, &viewport);

    // int offset = timeText->getWidth() + 32;
    // timeText->render(renderer, window.getWidth() - offset, gyPadding);

    int offset = gMaxPlayerHealth + gxTextSpacing;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect fillRect = {window.getWidth() - offset, 2 * gyPadding + gyRenderOffset, (players[0].getHealth() * (gMaxPlayerHealth) / mMaxPlayerHealth), gyRenderOffset};
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_Rect outlineRect = {window.getWidth() - offset, 2 * gyPadding + gyRenderOffset, gMaxPlayerHealth, gyRenderOffset};
    SDL_RenderDrawRect(renderer, &outlineRect);

    offset += healthText->getWidth() + gxTextSpacing;
    healthText->render(renderer, window.getWidth() - offset, 2 * gyPadding + gyRenderOffset);

    offset += moneyText->getWidth() + gxTextSpacing;
    moneyText->render(renderer, window.getWidth() - offset, 2 * gyPadding + gyRenderOffset);

    offset += pointsText->getWidth() + gxTextSpacing;
    pointsText->render(renderer, window.getWidth() - offset, 2 * gyPadding + gyRenderOffset);
    offset += pointsText->getWidth();

    nameText->render(renderer, window.getWidth() - offset / 2, gyPadding);

    if (gameEnded)
    {
        if (result)
        {
            winText->render(renderer, (window.getWidth() - winText->getWidth()) / 2, (3 * gyPadding) / 2 + gyRenderOffset - winText->getHeight() / 2);
            // pressAnyKey->render(renderer, (window.getWidth() - pressAnyKey->getWidth()) / 2, (window.getHeight() + winText->getHeight()) / 2 + 32);
        }
        else
        {
            loseText->render(renderer, (window.getWidth() - loseText->getWidth()) / 2, (3 * gyPadding) / 2 + gyRenderOffset - loseText->getHeight() / 2);
            // pressAnyKey->render(renderer, (window.getWidth() - pressAnyKey->getWidth()) / 2, (window.getHeight() + loseText->getHeight()) / 2 + 32);
        }
    }
    else
    {
        timeText->render(renderer, (window.getWidth() - timeText->getWidth()) / 2, (3 * gyPadding) / 2 + gyRenderOffset - timeText->getHeight() / 2);
    }

    offset = gxTextSpacing;
    healthText->render(renderer, offset, 2 * gyPadding + gyRenderOffset);

    offset += healthText->getWidth();
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

    fillRect = {offset, 2 * gyPadding + gyRenderOffset, (players[1].getHealth() * (gMaxPlayerHealth) / mMaxPlayerHealth), gyRenderOffset};
    SDL_RenderFillRect(renderer, &fillRect);
    outlineRect = {offset, 2 * gyPadding + gyRenderOffset, gMaxPlayerHealth, gyRenderOffset};
    SDL_RenderDrawRect(renderer, &outlineRect);

    offset += gMaxPlayerHealth + gxTextSpacing;
    moneyTextOpp->render(renderer, offset, 2 * gyPadding + gyRenderOffset);

    offset += moneyText->getWidth() + gxTextSpacing;
    pointsTextOpp->render(renderer, offset, 2 * gyPadding + gyRenderOffset);

    offset += pointsTextOpp->getWidth();
    oppText->render(renderer, offset / 2, gyPadding);

    viewport.x = 0;
    viewport.y = window.getHeight() - gyRenderOffset - 2 * gyPadding;
    viewport.w = window.getWidth();
    viewport.h = gyRenderOffset + 2 * gyPadding;
    SDL_RenderSetViewport(renderer, &viewport);

    if (gameEnded)
    {
        pressAnyKey->render(renderer, 0, gyPadding);
    }
    else
    {
        prompText->render(renderer, 0, gyPadding);

        if (players[0].isBusy())
        {
            SDL_SetRenderDrawColor(renderer, 0xFd, 0xb3, 0x36, 0xFF);
            offset = taskStatusBarWidth + 10;
            SDL_Rect fillRect = {window.getWidth() - offset, gyPadding, ((int)players[0].getCurrentTaskTimer().getTicks() * (taskStatusBarWidth)) / (players[0].getCurrentTaskTime()), gyRenderOffset};
            SDL_RenderFillRect(renderer, &fillRect);
            SDL_Rect outlineRect = {window.getWidth() - offset, gyPadding, taskStatusBarWidth, gyRenderOffset};
            SDL_RenderDrawRect(renderer, &outlineRect);
        }
    }

    viewport.x = window.getWidth() - tasksVPWidth;
    viewport.y = 3 * gyPadding + 2 * gyRenderOffset;
    viewport.w = tasksVPWidth;
    viewport.h = window.getHeight() - (5 * gyPadding + 3 * gyRenderOffset);
    SDL_RenderSetViewport(renderer, &viewport);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    outlineRect = {0, 0, tasksVPWidth, window.getHeight() - (5 * gyPadding + 3 * gyRenderOffset)};
    SDL_RenderDrawRect(renderer, &outlineRect);

    int maxHeight = taskTexts[0]->getHeight();
    for (int i = 0; i < taskTexts.size(); i++)
    {
        maxHeight = maxHeight > taskTexts[i]->getHeight() ? maxHeight : taskTexts[i]->getHeight();
    }

    tasksText->render(renderer, (tasksVPWidth - tasksText->getWidth()) / 2, 16);
    for (int i = 0; i < taskTexts.size(); i++)
    {
        taskTexts[i]->render(renderer, 8, (16 + maxHeight) * i + 20 + tasksText->getHeight() + 16);
    }
}

void LGame::cleanUp()
{
    window.restore();
    for (int i = 0; i < players.size(); i++)
    {
        players[i].cleanUp();
    }
    tiles[0].cleanUp();
    for (int i = 0; i < NPCs.size(); i++)
    {
        NPCs[i].cleanUp();
    }
    delete timeText;
    delete healthText;
    delete pointsText;
    delete moneyText;
    delete oppText;
    delete nameText;
    delete moneyTextOpp;
    delete pointsTextOpp;

    for (int i = 0; i < currTasks.size(); i++)
    {
        delete taskTexts[i];
    }

    delete sleepingAnimation;
    Mix_FreeMusic(backGroundMusic);
    Mix_FreeChunk(introMusic);
    backGroundMusic = NULL;
}

bool LGame::initObjs()
{

    if (!window.loadTexture(ashTexture, "resources/ash.bmp"))
    {
        printf("Failed to load ash texture!\n");
        return false;
    }
    if (!window.loadTexture(ash2Texture, "resources/ash2.bmp"))
    {
        printf("Failed to load ash2 texture!\n");
        return false;
    }
    if (!window.loadTexture(dogTexture, "resources/dog.png"))
    {
        printf("Failed to load dog texture!\n");
        return false;
    }
    if (!window.loadTexture(profTexture, "resources/oak.png"))
    {
        printf("Failed to load prof texture!\n");
        return false;
    }
    if (!window.loadTexture(robberTexture, "resources/robber.png"))
    {
        printf("Failed to load robber texture!\n");
        return false;
    }   
    if (!window.loadTexture(yuluTexture, "resources/yulu.bmp"))
    {
        printf("Failed to load yulu texture!\n");
        return false;
    }         
    LTexture *sleepingAnimationTexture = new LTexture();
    LTexture *burgerAnimationTexture = new LTexture();
    LTexture *hotdogAnimationTexture = new LTexture();
    LTexture *icecreamAnimationTexture = new LTexture();
    LTexture *tennisAnimationTexture = new LTexture();
    LTexture *basketBallAnimationTexture = new LTexture();
    LTexture *footBallAnimationTexture = new LTexture();
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
    if (!window.loadTexture(*footBallAnimationTexture, "resources/football.png"))
    {
        printf("Failed to load football texture!\n");
        return false;
    }
    sleepingAnimation = new Animation(*sleepingAnimationTexture, 32, 32);
    burgerAnimation = new Animation(*burgerAnimationTexture, 32, 32);
    hotDogAnimation = new Animation(*hotdogAnimationTexture, 32, 32);
    icecreamAnimation = new Animation(*icecreamAnimationTexture, 32, 32);
    basketballAnimation = new Animation(*basketBallAnimationTexture, 32, 32);
    tennisAnimation = new Animation(*tennisAnimationTexture, 32, 32);
    footballAnimation = new Animation(*footBallAnimationTexture, 32, 32);
    Player ash(ashTexture, yuluTexture, *this, 32, 32, 3, 1, 2, 0);
    players.push_back(ash);
    int numberOfNPCs = 5;
    for (int i = 0; i < numberOfNPCs; i++)
    {
        NPC dog(dogTexture, "dog" + std::to_string(i + 1), *this, 32, 32, 1, 3, 2, 0);
        NPC prof(profTexture, "prof" + std::to_string(i + 1), *this, 42, 32, 3, 2, 0, 1);
        NPC robber( robberTexture , "robbber" + std::to_string(i+1),*this, 40 , 30 , 2 , 1 , 3 , 0 ) ; 
        NPCs.push_back(dog);
        NPCs.push_back(prof);
        NPCs.push_back( robber) ; 
    }

    Player opponent(ash2Texture, yuluTexture , *this, 32, 32, 3, 1, 2, 0);
    players.push_back(opponent);

    camera = {0, 0, window.getWidth() - tasksVPWidth, window.getHeight() - 3 * gyRenderOffset - 5 * gyPadding};

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
    TTF_Font *fontSmall = TTF_OpenFont("resources/FrostbiteBossFight-dL0Z.ttf", 20);
    TTF_Font *fontLarge = TTF_OpenFont("resources/FrostbiteBossFight-dL0Z.ttf", 32);

    timeText = new Text(window, "00:00", font, txtColor);
    healthText = new Text(window, "HEALTH: ", font, txtColor);
    moneyText = new Text(window, "MONEY: ", font, txtColor);
    pointsText = new Text(window, "POINTS: ", font, txtColor);
    moneyTextOpp = new Text(window, "MONEY: ", font, txtColor);
    pointsTextOpp = new Text(window, "POINTS: ", font, txtColor);
    nameText = new Text(window, playerName, font, txtColor);
    oppText = new Text(window, opponentName, font, txtColor);
    tasksText = new Text(window, "TASKS", font, txtColor);
    winText = new Text(window, "YOU WON!", fontLarge, txtColor);
    loseText = new Text(window, "YOU LOST!", fontLarge, txtColor);
    pressAnyKey = new Text(window, "Press space to go back to main menu", font, txtColor);

    for (int i = 0; i < tasksNum; i++)
    {
        taskTexts.push_back(new Text(window, "DUMMY", fontSmall, txtColor));
    }

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
    for (int i = 0; i < NPCs.size(); i++)
    {
        renderables.push_back(&NPCs[i]);
    }
    // std::cout << "OBJECTS INITIALIZED" << "\n";
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
            displayText += " B-BreakFast  L-Lunch  D-Dinner  Space-REST";
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
        int secs = player.getGame().getTimer().getTicks() / 1000;
        int secsPerHour = gameLenSecs / 24;
        int mins = (60 / secsPerHour) * (secs % secsPerHour);
        int hours = secs / secsPerHour;
        int totalMins = 60 * hours + mins;

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && player.getHostelName() == HostelName)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_SPACE:
                // create rest task if player is not busy
                if (!player.isBusy())
                {
                    player.setCurrentTaskTime(10000);
                    player.getCurrentTaskTimer().start();
                    player.setTaskText("resting ... ");
                    player.setTaskAnimation(player.getGame().sleepingAnimation);
                    player.setUpdateStateParameters({
                        gMaxPlayerHealth,
                        0,
                        0,
                        0,
                    });
                }
                break;
            case SDLK_b:
                if (!player.isBusy() && !player.hadBreakFast() && (totalMins >= 7 * 60 + 30 && totalMins <= 9 * 60 + 30))
                {
                    player.setBreakfast(true);
                    player.setTaskText("having breakfast... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({35,
                                                     0,
                                                     0,
                                                     0});
                }
                break;
            case SDLK_l:
                if (!player.isBusy() && !player.hadLunch() && (totalMins >= 12 * 60 && totalMins <= 14 * 60))
                {
                    player.setLunch(true);
                    player.setTaskText("having lunch ... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({35,
                                                     0,
                                                     0,
                                                     0});
                }
                break;

            case SDLK_d:
                if (!player.isBusy() && !player.hadDinner() && (totalMins >= 19 * 60 && totalMins <= 21 * 60))
                {
                    player.setDinner(true);
                    player.setTaskText("having dinner... ");
                    player.setCurrentTaskTime(3000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({35,
                                                     0,
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
            displayText = "Press space to release yulu";
        }else
        {
            displayText = "Press space to take yulu";
        } },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
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
    Entity volleyball(
        "volleyball", [&](Player &player, std::string &displayText)
        { displayText = "Press space to play volleyball"; },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    player.setTaskText("playing volleyball... ");
                    // player.setTaskAnimation(player.getGame().tennisAnimation);
                    player.setCurrentTaskTime(5000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({-5,
                                                     0,
                                                     0,
                                                     10});
                    break;
                }
            }
        });

    Entity tennis("tennis", getTextPromptFunc("Press space to play tennis"),
                  [&](SDL_Event &e, Player &player)
                  {
                      if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                      {
                          switch (e.key.keysym.sym)
                          {
                          case SDLK_SPACE:
                              player.setTaskText("playing tennis... ");
                              player.setTaskAnimation(player.getGame().tennisAnimation);
                              player.setCurrentTaskTime(5000);
                              player.getCurrentTaskTimer().start();
                              player.setUpdateStateParameters({-5,
                                                               0,
                                                               0,
                                                               10});
                              break;
                          }
                      }
                  });

    Entity swimming_pool(
        "swimming_pool", [&](Player &player, std::string &displayText)
        { displayText = "Press space to swim"; },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    player.setTaskText("swimming... ");
                    // player.setTaskAnimation(player.getGame().tennisAnimation);
                    player.setCurrentTaskTime(5000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({-5,
                                                     0,
                                                     0,
                                                     10});
                    break;
                }
            }
        });

    Entity oat(
        "oat", [&](Player &player, std::string &displayText)
        { displayText = "Press space to perform"; },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    player.setTaskText("performing... ");
                    // player.setTaskAnimation(player.getGame().tennisAnimation);
                    player.setCurrentTaskTime(7000);
                    player.getCurrentTaskTimer().start();
                    player.setUpdateStateParameters({-5,
                                                     40,
                                                     0,
                                                     0});
                    break;
                }
            }
        });

    Entity hot_dog("hot_dog", getTextPromptFunc("Press space to buy hotdog (40 Rs.)"),
                   [&](SDL_Event &e, Player &player)
                   {
                       if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                       {
                           switch (e.key.keysym.sym)
                           {
                           case SDLK_SPACE:
                               if (player.getMoney() > 40)
                               {
                                   player.setTaskText("having hotdog... ");
                                   player.setTaskAnimation(player.getGame().hotDogAnimation);
                                   player.setCurrentTaskTime(8000);
                                   player.getCurrentTaskTimer().start();
                                   player.setUpdateStateParameters({20,
                                                                    -40,
                                                                    0,
                                                                    0});
                               }
                               break;
                           }
                       }
                   });

    Entity gas("gas", [&](Player &player, std::string &displayText)
               { displayText = "Gas Station"; });

    Entity icecream("icecream", getTextPromptFunc("Press space to buy icecream (10 Rs.)"),
                    [&](SDL_Event &e, Player &player)
                    {
                        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                        {
                            switch (e.key.keysym.sym)
                            {
                            case SDLK_SPACE:
                                if (player.getMoney() > 10)
                                {
                                    player.setTaskText("having icecream... ");
                                    player.setCurrentTaskTime(3000);
                                    player.getCurrentTaskTimer().start();
                                    player.setTaskAnimation(player.getGame().icecreamAnimation);
                                    player.setUpdateStateParameters({5,
                                                                     -10,
                                                                     0,
                                                                     0});
                                }
                                break;
                            }
                        }
                    });

    Entity shop(
        "shop", [&](Player &player, std::string &displayText)
        { 
            if(player.getMoney() > 20){
                displayText = "Press space to buy stationery";
            }
            else{
                displayText = "You need at least 20 rupees to buy stationery.";

            } },
        [&](SDL_Event &e, Player &player)
        {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    if (player.getMoney() > 20)
                    {
                        player.setTaskText("Buying stationery... ");
                        player.setCurrentTaskTime(5000);
                        player.getCurrentTaskTimer().start();
                        // player.setTaskAnimation(player.getGame().icecreamAnimation);
                        player.setUpdateStateParameters({0,
                                                         -20,
                                                         0,
                                                         10});
                    }
                    break;
                }
            }
        });

    Entity sac(
        "sac", [&](Player &player, std::string &displayText)
        { displayText = "Press space to dance"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("dancing... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity foot("foot", getTextPromptFunc("Press space to play football"),
                [&](SDL_Event &e, Player &player)
                {
                    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                    {
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_SPACE:
                            player.setTaskText("playing football... ");
                            player.setTaskAnimation(player.getGame().footballAnimation);
                            player.setCurrentTaskTime(5000);
                            player.getCurrentTaskTimer().start();
                            player.setUpdateStateParameters({-5,
                                                             0,
                                                             0,
                                                             10});
                            break;
                        }
                    }
                });

    Entity basketball("basketball", getTextPromptFunc("Press space to play basketball"),
                      [&](SDL_Event &e, Player &player)
                      {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("playing basketball... ");
                                  player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          }
                      });

    Entity athletic("athletic", getTextPromptFunc("Press space to do athletics"), [&](SDL_Event &e, Player &player)
                    {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("doing athletics... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity cricket("cricket", getTextPromptFunc("Press space to play cricket"), [&](SDL_Event &e, Player &player)
                   {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("playing cricket... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });
    Entity lhc(
        "lhc", [&](Player &player, std::string &displayText)
        { displayText = "Press space to attend class"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("attending class... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity police(
        "police", [&](Player &player, std::string &displayText)
        { displayText = "Press space to get ID card"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("Getting the ID card... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity main_building(
        "main_building", [&](Player &player, std::string &displayText)
        { displayText = "Press space to get documents verified"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("Getting documents verified... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity biotech_dept(
        "biotech_dept", [&](Player &player, std::string &displayText)
        { displayText = "Press space to meet professor"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("Meeting professor... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity library(
        "library", [&](Player &player, std::string &displayText)
        { displayText = "Press space to study"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                                  player.setTaskText("Studying... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({-5,
                                                                   0,
                                                                   0,
                                                                   10});
                                  break;
                              }
                          } });

    Entity coffee(
        "coffee", [&](Player &player, std::string &displayText)
        { displayText = "Press space to buy coffee (10 Rs.)"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 10){
                                  player.setTaskText("playing cricket... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(3000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({5,
                                                                   -10,
                                                                   0,
                                                                   0});
                                  break;
                              }
                              }
                          } });

    Entity hospital(
        "hospital", [&](Player &player, std::string &displayText)
        { displayText = "Press space to regenerate health (5 Rs.)"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 5){
                                  player.setTaskText("Getting treated... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(10000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({gMaxPlayerHealth,
                                                                   -5,
                                                                   0,
                                                                   0});
                                  break;
                              }
                              }
                          } });

    Entity burger("burger", getTextPromptFunc("Press space to buy burger (20 Rs.)"), [&](SDL_Event &e, Player &player)
                  {
                        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                        {
                            switch (e.key.keysym.sym)
                            {
                            case SDLK_SPACE:
                                if(player.getMoney() > 20 ) {
                                    player.setTaskText("Having burger... ") ; 
                                    player.setCurrentTaskTime(6000) ; 
                                    player.getCurrentTaskTimer().start() ;
                                    player.setTaskAnimation( player.getGame().burgerAnimation ) ; 
                                    player.setUpdateStateParameters({
                                        10 , 
                                        -20 , 
                                        0,
                                        0 
                                    }) ;  
                                }
                                break;
                            }
                        } });

    Entity vegetable_shop(
        "vegetable_shop", [&](Player &player, std::string &displayText)
        { if(player.getMoney() > 20){ 
            displayText = "Press space to buy vegetables"; }
            else{
                displayText = "You need to have at least 20 rupees to buy vegetables";
            } },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 20){
                                  player.setTaskText("buying vegetables... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({0,
                                                                   -20,
                                                                   0,
                                                                   10});
                                  break;
                              }
                              }
                          } });

    Entity bread_shop(
        "bread_shop", [&](Player &player, std::string &displayText)
        { if(player.getMoney() > 20){ 
            displayText = "Press space to buy bread"; }
            else{
                displayText = "You need to have at least 20 rupees to buy bread";
            } },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 20){
                                  player.setTaskText("buying bread... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({0,
                                                                   -20,
                                                                   0,
                                                                   10});
                                  break;
                              }
                              }
                          } });

    Entity hardware_shop(
        "hardware_shop", [&](Player &player, std::string &displayText)
        { if(player.getMoney() > 20){ 
            displayText = "Press space to buy tools"; }
            else{
                displayText = "You need to have at least 20 rupees to buy tools";
            } },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 20){
                                  player.setTaskText("buying tools... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({0,
                                                                   -20,
                                                                   0,
                                                                   10});
                                  break;
                              }
                              }
                          } });

    Entity barber(
        "barber", [&](Player &player, std::string &displayText)
        { if(player.getMoney() > 20){ 
            displayText = "Press space to get a haircut"; }
            else{
                displayText = "You need to have at least 20 rupees to get a haircut";
            } },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 20){
                                  player.setTaskText("Getting a haircut... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(5000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({0,
                                                                   -20,
                                                                   0,
                                                                   10});
                                  break;
                              }
                              }
                          } });

    Entity pharmacy(
        "pharmacy", [&](Player &player, std::string &displayText)
        { displayText = "Press space to buy medicines (Rs. 40)"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 40){
                                  player.setTaskText("buying medicines... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(8000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({20,
                                                                   -40,
                                                                   0,
                                                                   0});
                                  break;
                              }
                              }
                          } });

    Entity beverage_shop(
        "beverage_shop", [&](Player &player, std::string &displayText)
        { displayText = "Press space to buy a beverage (Rs. 20)"; },
        [&](SDL_Event &e, Player &player)
        {
                          if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                          {
                              switch (e.key.keysym.sym)
                              {
                              case SDLK_SPACE:
                              if(player.getMoney() > 20){
                                  player.setTaskText("drinking a beverage... ");
                                  //player.setTaskAnimation(player.getGame().basketballAnimation);
                                  player.setCurrentTaskTime(8000);
                                  player.getCurrentTaskTimer().start();
                                  player.setUpdateStateParameters({10,
                                                                   -20,
                                                                   0,
                                                                   0});
                                  break;
                              }
                              }
                          } });

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

bool LGame::hasTask(std::pair<int, std::string> task)
{
    std::vector<std::pair<int, std::string>>::iterator it;
    it = std::find(currTasks.begin(), currTasks.end(), task);
    return it != currTasks.end();
}

bool LGame::hasTask(int task)
{
    for (int i = 0; i < currTasks.size(); i++)
    {
        if (currTasks[i].first == task)
        {
            return true;
        }
    }
    return false;
}

void LGame::replaceTask(std::pair<int, std::string> task)
{
    std::vector<std::pair<int, std::string>>::iterator it;
    it = std::find(currTasks.begin(), currTasks.end(), task);
    int index = it - currTasks.begin();
    std::vector<std::pair<int, std::string>>::iterator it2;
    std::pair<int, std::string> t;
    do
    {
        t = allTasks[rand() % allTasks.size()];
        it2 = std::find(currTasks.begin(), currTasks.end(), t);
    } while (it2 != currTasks.end());
    currTasks[index] = t;
    taskTexts[index]->setText(t.second);
}

void LGame::replaceTask(int task)
{
    int index = -1;
    for (int i = 0; i < currTasks.size(); i++)
    {
        if (currTasks[i].first == task)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        return;
    }

    std::vector<std::pair<int, std::string>>::iterator it2;
    std::pair<int, std::string> t;
    do
    {
        t = allTasks[rand() % allTasks.size()];
        it2 = std::find(currTasks.begin(), currTasks.end(), t);
    } while (it2 != currTasks.end());
    currTasks[index] = t;
    taskTexts[index]->setText(t.second);
}

int LGame ::getTileType(int x, int y)
{
    int tileX = x / mTileWidth;
    int tileY = y / mTileHeight;
    // std :: cout << x << " " << tileX << "\n" ;
    // std :: cout << y << " " << tileY << "\n" ;
    return tiles[tileY * mTilesX + tileX].getType();
}