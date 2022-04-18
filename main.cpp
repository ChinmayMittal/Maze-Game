#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <time.h>

#include "MyWindow.h"
#include "Game.h"
#include "MainMenu.h"
#include "constants.h"

// Screen dimension constants
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
	srand(time(0));
	std::string serverIp = "192.168.227.102";
	if (argc > 1)
	{
		serverIp = args[1];
	}

	int sockfd;
	// struct sockaddr_in myAddr;
	struct sockaddr_in theirAddr;
	char buf[512];
	char recBuf[512];

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

	// Start up SDL and create window
	LWindow window(SCREEN_WIDTH, SCREEN_HEIGHT);
	// LGame myGame(window);
	// window.setCurrScreen(&myGame);
	MainMenu *mainMenu = new MainMenu(window, sockfd, theirAddr);
	window.setCurrScreen(mainMenu);
	window.begin();
	close(sockfd);
	return 0;
}