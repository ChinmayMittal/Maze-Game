#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "MyWindow.h"
#include "Game.h"
#include "MainMenu.h"
#include "constants.h"

// Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
	std::string serverIp = "192.168.227.10";
	if (argc > 1)
	{
		serverIp = args[1];
	}

	// Start up SDL and create window
	LWindow window(SCREEN_WIDTH, SCREEN_HEIGHT);
	// LGame myGame(window);
	// window.setCurrScreen(&myGame);
	MainMenu *mainMenu = new MainMenu(window, serverIp);
	window.setCurrScreen(mainMenu);
	window.begin();
	return 0;
}