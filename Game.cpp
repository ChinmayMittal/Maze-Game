#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "rapidxml.hpp"

#include "Game.h"
#include "MyWindow.h"
#include "Screen.h"

LGame::LGame(LWindow &window) : LScreen(window)
{
    initObjs();
}

void LGame::handleEvent(SDL_Event &e)
{
    players[0].handleEvent(e);
}

void LGame::update()
{
    camera = {camera.x, camera.y, window.getWidth(), window.getHeight()};
    // Move the dot
    players[0].move();
    players[0].setCamera(camera);
}

void LGame::render(SDL_Renderer *renderer)
{
    for (size_t i = 0; i < renderables.size(); i++)
    {
        int res = renderables[i]->render(renderer, camera);

        if (res != 0)
        {
            // Handle Error
        }
    }
}

void LGame::cleanUp()
{
    players[0].cleanUp();
    tiles[0].cleanUp();
}

bool LGame::initObjs()
{

    if (!window.loadTexture(ashTexture, "resources/ash.bmp"))
    {
        printf("Failed to load ash texture!\n");
        return false;
    }

    Dot ash(ashTexture, *this, 32, 32, 3, 1, 2, 0, 1);
    players.push_back(ash);

    camera = {0, 0, window.getWidth(), window.getHeight()};

    if (!window.loadTexture(tilesTexture, "resources/tiles2.png"))
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

    for (int i = 0; i < tiles.size(); i++)
    {
        renderables.push_back(&tiles[i]);
    }
    for (int i = 0; i < players.size(); i++)
    {
        renderables.push_back(&players[i]);
    }

    return true;
}

bool LGame::setTiles()
{
    // Open the map
    std::ifstream mapStream("resources/map2.tmx");

    // If the map couldn't be loaded
    if (mapStream.fail())
    {
        printf("Unable to load map file!\n");
        return false;
    }

    std::stringstream buffer;
    buffer << mapStream.rdbuf();

    std::string xmlText = buffer.str();

    rapidxml::xml_document<> doc; // character type defaults to char
    doc.parse<0>(const_cast<char *>(xmlText.c_str()));

    rapidxml::xml_node<> *mapNode = doc.first_node("map");
    int tileWidth = std::stoi(mapNode->first_attribute("tilewidth")->value());
    int tileHeight = std::stoi(mapNode->first_attribute("tileheight")->value());

    int numTilesX = std::stoi(mapNode->first_attribute("width")->value());
    int numTilesY = std::stoi(mapNode->first_attribute("height")->value());

    std::string tilesData = mapNode->first_node("layer")->first_node("data")->value();
    int j = 0;

    // Initialize the tiles
    for (int i = 0; i < numTilesX * numTilesY; ++i)
    {
        // Read tile from map file
        std::string token;
        while (j < tilesData.size() && tilesData[j] != ',')
        {
            token += tilesData[j];
            j++;
        }
        j++;

        int tileType = std::stoi(token);

        Tile myTile(tilesTexture, *this, (i % numTilesX) * tileWidth, (i / numTilesX) * tileHeight, tileWidth, tileHeight, tileType);
        tiles.push_back(myTile);
    }

    // Close the file
    mapStream.close();

    mTilesX = numTilesX;
    mTilesY = numTilesY;
    mTileWidth = tileWidth;
    mTileHeight = tileHeight;

    return true;
}

SDL_Rect LGame::getTileClip(int tileType)
{
    return tileAtlas.tileClips[tileType - 1];
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