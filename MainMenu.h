#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Screen.h"
#include "MyWindow.h"
#include "Button.h"
#include "TextInput.h"
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

class MainMenu : public LScreen
{
private:
    std::string serverIp;
    std::vector<Button *> buttons;
    std::vector<TextInput *> textInputs;
    Text *waitingText;
    LTexture buttonBg;
    LTexture buttonBgHighlighted;
    LTexture buttonBgClicked;
    int &sockfd;
    // struct sockaddr_in myAddr;
    struct sockaddr_in &theirAddr;
    char buf[512];
    char recBuf[512];
    bool waiting = false;

public:
    MainMenu(LWindow &window, int &sockfd, sockaddr_in &theiraddr);
    void handleEvent(SDL_Event &e);
    void update();
    void render(SDL_Renderer *renderer);
    void cleanUp();
    void sendGameReq();
};

#endif