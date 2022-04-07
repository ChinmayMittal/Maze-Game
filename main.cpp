#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "MyWindow.h"
#include "Game.h"

// Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
	// Start up SDL and create window
	LWindow window(SCREEN_WIDTH, SCREEN_HEIGHT);
	LGame myGame(window);
	window.setCurrScreen(&myGame);
	window.begin();
	return 0;
}