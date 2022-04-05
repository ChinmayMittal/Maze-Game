#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "MyWindow.h"
#include "TileAtlas.h"
#include "rapidxml.hpp"
#include "Tile.h"
#include "Player.h"

// Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

std::vector<Tile> tiles;
std::vector<Dot> dots;
SDL_Rect camera;

LTexture dotTexture;
LTexture tilesTexture;

bool initLibs();
LWindow initWin();
bool initObjs(LWindow &window);
bool setTiles(LTexture &tilesTexture, LWindow &window);
void cleanUpObjs();

bool initLibs()
{
	// Initialization flag

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
		return false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

LWindow initWin()
{
	// Create window
	LWindow window;
	if (!window.init(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}
	return window;
}

bool initObjs(LWindow &window)
{
	if (!window.loadTexture(dotTexture, "dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		return false;
	}

	Dot dot(dotTexture, window);
	dots.push_back(dot);

	camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	if (!window.loadTexture(tilesTexture, "tiles2.png"))
	{
		printf("Failed to load tile set texture!\n");
		return false;
	}

	// Load tile map
	if (!setTiles(tilesTexture, window))
	{
		printf("Failed to load tile set!\n");
		return false;
	}

	int uniqueTilesX = tilesTexture.getWidth() / tiles[0].getBox().w;
	int uniqueTilesY = tilesTexture.getHeight() / tiles[0].getBox().h;

	TileAtlas::init(uniqueTilesX, uniqueTilesY, tiles[0].getBox().w, tiles[0].getBox().h);

	return true;
}

bool setTiles(LTexture &tilesTexture, LWindow &window)
{
	// Open the map
	std::ifstream mapStream("map2.tmx");

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

		Tile myTile(tilesTexture, window, (i % numTilesX) * tileWidth, (i / numTilesX) * tileHeight, tileWidth, tileHeight, tileType);
		tiles.push_back(myTile);
	}

	// Close the file
	mapStream.close();

	window.setParams(numTilesX, numTilesY, tileWidth, tileHeight);

	// If the map was loaded fine
	return true;
}

void cleanUpObjs()
{
	// Free loaded images
	dots[0].cleanUp();
	tiles[0].cleanUp();
}

int main(int argc, char *args[])
{
	// Start up SDL and create window
	initLibs();
	LWindow window = initWin();
	initObjs(window);

	std::vector<Renderable *> renderables;
	for (int i = 0; i < tiles.size(); i++)
	{
		renderables.push_back(&tiles[i]);
	}
	for (int i = 0; i < dots.size(); i++)
	{
		renderables.push_back(&dots[i]);
	}
	// Main loop flag
	bool quit = false;

	// Event handler
	SDL_Event e;

	// The dot that will be moving around on the screen

	// While application is running
	while (!quit)
	{
		// Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			window.handleEvent(e);
			// Handle input for the dot
			dots[0].handleEvent(e);
		}

		// Move the dot
		dots[0].move();
		dots[0].setCamera(camera);

		if (!window.isMinimized())
		{
			// Dot *d = dynamic_cast<Dot *>(renderables[0]);
			// std::cout << d->mTexture.getWidth() << std::endl;

			window.render(renderables, camera);
		}
	}

	// Free resources and close SDL
	cleanUpObjs();
	window.cleanUp();

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	return 0;
}