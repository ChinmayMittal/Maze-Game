#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "rapidxml.hpp"

#include "Game.h"
#include "MyWindow.h"
#include "Screen.h"
#include "Entity.h"

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

    SDL_Rect playerBox = players[0].getBox();
    // std::cout << "After move " << playerBox.y << std::endl;
    int tileX = (playerBox.x + playerBox.w / 2) / mTileWidth;
    int tileY = (playerBox.y + playerBox.h / 2) / mTileHeight;
    int tileType = tiles[tileY * mTilesX + tileX].getType();
    // std::cout << "Tiletype " << tileType << std::endl;
    entities[tileType].collided(players[0]);
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
    int mins = 3 * (secs % 20);
    int hours = (secs / 20);

    timeText->setText("TIME: " + std::to_string(hours) + ":" + std::to_string(mins));
    pointsText->setText("POINTS: " + std::to_string(players[0].getPoints()));
    moneyText->setText("MONEY: " + std::to_string(players[0].getMoney()));
    healthText->setText("HEALTH:");
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

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

    int offset = timeText->getWidth() + 32;
    timeText->render(renderer, window.getWidth() - offset, 32);
    offset += 100 + 10;
    SDL_Rect fillRect = {window.getWidth() - offset, 32, (players[0].getHealth() * (100) / mMaxPlayerHealth), 32};
    SDL_RenderFillRect(renderer, &fillRect);
    SDL_Rect outlineRect = {window.getWidth() - offset, 32, 100, 32};

    SDL_RenderDrawRect(renderer, &outlineRect);

    offset += healthText->getWidth() + 10;
    healthText->render(renderer, window.getWidth() - offset, 32);

    offset += moneyText->getWidth() + 30;
    moneyText->render(renderer, window.getWidth() - offset, 32);

    offset += pointsText->getWidth() + 30;
    pointsText->render(renderer, window.getWidth() - offset, 32);
    players[0].setHealth((players[0].getHealth() + 10) % 100);
}

void LGame::cleanUp()
{
    players[0].cleanUp();
    tiles[0].cleanUp();
    delete timeText;
}

bool LGame::initObjs()
{

    if (!window.loadTexture(ashTexture, "resources/ash.bmp"))
    {
        printf("Failed to load ash texture!\n");
        return false;
    }

    Player ash(ashTexture, *this, 32, 32, 3, 1, 2, 0);
    players.push_back(ash);

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
    mMaxPlayerHealth = 100;

    return true;
}

void LGame::initEntities()
{
    Entity road("Road", [&](Player &player)
                { player.setVelocity(2); });
    Entity grass("Grass", [&](Player &player)
                 { player.setVelocity(1); });
    Entity pavement("Pavement", [&](Player &player)
                    { player.setVelocity(2); });
    Entity no_movement("no_movement", [&](Player &player)
                       { player.moveBy(-player.getXVel(), -player.getYVel()); });

    Entity yulu("yulu", [&](Player &player)
                { 
                    if (player.getLastTileType() != 4)
                    {
                        player.toggleYulu();
                        //std::cout << "Toggled" << std::endl;
                    } });

    Entity nilgiri("nilgiri", [&](Player &player)
                   { std ::cout << "nilgiri"
                                << " \n  "; });
    Entity kara("kara", [&](Player &player)
                { std ::cout << "kara"
                             << " \n  "; });
    Entity aravali("aravali", [&](Player &player)
                   { std ::cout << "aravali"
                                << " \n  "; });
    Entity jwala("jwala", [&](Player &player)
                 { std ::cout << "jwala"
                              << " \n  "; });
    Entity kumaon("kumaon", [&](Player &player)
                  { std ::cout << "kumaon"
                               << " \n  "; });
    Entity vindy("vindy", [&](Player &player)
                 { std ::cout << "vindy"
                              << " \n  "; });
    Entity satpura("satpura", [&](Player &player)
                   { std ::cout << "satpura"
                                << " \n  "; });
    Entity udai_girnar("udai_girnar", [&](Player &player)
                       { std ::cout << "udai_girnar"
                                    << " \n  "; });
    Entity volleyball("volleyball", [&](Player &player)
                      { std ::cout << "volleyball"
                                   << " \n  "; });
    Entity tennis("tennis", [&](Player &player)
                  { std ::cout << "tennis"
                               << " \n  "; });
    Entity swimming_pool("swimming_pool", [&](Player &player)
                         { std ::cout << "swimming_pool"
                                      << " \n  "; });
    Entity oat("oat", [&](Player &player)
               { std ::cout << "oat"
                            << " \n  "; });
    Entity hot_dog("hot_dog", [&](Player &player)
                   { std ::cout << "hot_dog"
                                << " \n  "; });
    Entity himadri("himadri", [&](Player &player)
                   { std ::cout << "himadri"
                                << " \n  "; });
    Entity kailash("kailash", [&](Player &player)
                   { std ::cout << "kailash"
                                << " \n  "; });
    Entity gas("gas", [&](Player &player)
               { std ::cout << "gas"
                            << " \n  "; });
    Entity icecream("icecream", [&](Player &player)
                    { std ::cout << "icecream"
                                 << " \n  "; });
    Entity shop("shop", [&](Player &player)
                { std ::cout << "shop"
                             << " \n  "; });
    Entity shivalik("shivalik", [&](Player &player)
                    { std ::cout << "shivalik"
                                 << " \n  "; });
    Entity zanskar("zanskar", [&](Player &player)
                   { std ::cout << "zanskar"
                                << " \n  "; });
    Entity sac("sac", [&](Player &player)
               { std ::cout << "sac"
                            << " \n  "; });
    Entity foot("foot", [&](Player &player)
                { std ::cout << "foot"
                             << " \n  "; });
    Entity basketball("basketball", [&](Player &player)
                      { std ::cout << "basketball"
                                   << " \n  "; });
    Entity athletic("athletic", [&](Player &player)
                    { std ::cout << "athletic"
                                 << " \n  "; });
    Entity cricket("cricket", [&](Player &player)
                   { std ::cout << "cricket"
                                << " \n  "; });
    Entity lhc("lhc", [&](Player &player)
               { std ::cout << "lhc"
                            << " \n  "; });
    Entity police("police", [&](Player &player)
                  { std ::cout << "police"
                               << " \n  "; });
    Entity main_building("main_building", [&](Player &player)
                         { std ::cout << "main_building"
                                      << " \n  "; });
    Entity biotech_dept("biotech_dept", [&](Player &player)
                        { std ::cout << "biotech_dept"
                                     << " \n  "; });
    Entity library("library", [&](Player &player)
                   { std ::cout << "library"
                                << " \n  "; });
    Entity coffee("coffee", [&](Player &player)
                  { std ::cout << "coffee"
                               << " \n  "; });
    Entity hospital("hospital", [&](Player &player)
                    { std ::cout << "hospital"
                                 << " \n  "; });
    Entity burger("burger", [&](Player &player)
                  { std ::cout << "burger"
                               << " \n  "; });
    Entity vegetable_shop("vegetable_shop", [&](Player &player)
                          { std ::cout << "vegetable_shop"
                                       << " \n  "; });
    Entity bread_shop("bread_shop", [&](Player &player)
                      { std ::cout << "bread_shop"
                                   << " \n  "; });
    Entity hardware_shop("hardware_shop", [&](Player &player)
                         { std ::cout << "hardware_shop"
                                      << " \n  "; });
    Entity barber("barber", [&](Player &player)
                  { std ::cout << "barber"
                               << " \n  "; });
    Entity pharmacy("pharmacy", [&](Player &player)
                    { std ::cout << "pharmacy"
                                 << " \n  "; });
    Entity beverage_shop("beverage_shop", [&](Player &player)
                         { std ::cout << "beverage_shop"
                                      << " \n  "; });

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