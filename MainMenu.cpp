#include "Screen.h"
#include "MainMenu.h"
#include "Game.h"
#include "TextInput.h"
#include <functional>
#include <iostream>
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
#include "MessageStructs.h"
#include "constants.h"

MainMenu::MainMenu(LWindow &window, int &sockfd, sockaddr_in &theiraddr) : LScreen(window), sockfd(sockfd), theirAddr(theiraddr)
{
    window.loadTexture(buttonBg, "resources/ButtonBG.png");
    window.loadTexture(buttonBgHighlighted, "resources/ButtonBGHighlighted.png");
    window.loadTexture(buttonBgClicked, "resources/ButtonBGClicked.png");

    int x = (window.getWidth() - buttonBg.getWidth()) / 2;
    int y = (window.getHeight() - buttonBg.getHeight()) / 2;
    int w = buttonBg.getWidth();
    int h = buttonBg.getHeight();

    SDL_Color txtColor = {2, 10, 255, 255};
    TTF_Font *font = TTF_OpenFont("resources/FrostbiteBossFight-dL0Z.ttf", 28);

    Button *playButton = new Button(x, y, &buttonBg, window, "Play", font, txtColor);
    playButton->setCenterX(true);
    playButton->setHighlightTexture(buttonBgHighlighted);
    playButton->setClickTexture(buttonBgClicked);
    playButton->setOnClickListener([&](LWindow &window)
                                   {
                                       LScreen *currScreen = window.getCurrScreen();
                                       MainMenu *menuScreen = dynamic_cast<MainMenu *>(currScreen);
                                       menuScreen->sendGameReq(); });
    buttons.push_back(playButton);

    txtColor = {0, 0, 0, 255};
    TextInput *nameInput = new TextInput(x, y - 40 - 20, 240, 40, {255, 255, 255, 255}, window, "", font, txtColor);
    nameInput->setCenterX(true);
    textInputs.push_back(nameInput);

    waitingText = new Text(window, "", font, {0, 0, 0, 255});
    // initSocket();
}

void MainMenu::sendGameReq()
{
    std::string playerName = textInputs[0]->getText();
    if (playerName == "")
    {
        waitingText->setText("Please enter a valid name.");
        return;
    }
    int i = 0;
    while (playerName[i] == ' ' || playerName[i] == '\n')
    {
        i++;
        if (playerName.size() == i)
        {
            waitingText->setText("Please enter a valid name.");
            return;
        }
    }

    NewClientMessage newClientMsg;
    newClientMsg.name = playerName;
    int bytesUsed = serialize(&newClientMsg, buf);
    int x = sendto(sockfd, buf, bytesUsed, 0, (const struct sockaddr *)&theirAddr,
                   sizeof(theirAddr));
    if (x == -1)
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
    }

    // std::cout << x << std::endl;
    waitingText->setText("Searching for opponent...");
    waiting = true;
}

void MainMenu::handleEvent(SDL_Event &e)
{
    if (!waiting)
    {
        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i]->handle(e);
        }
        for (int i = 0; i < textInputs.size(); i++)
        {
            textInputs[i]->handleEvent(e);
        }
    }
}

void MainMenu::update()
{

    int x = (window.getWidth() - buttonBg.getWidth()) / 2;
    int y = (window.getHeight() - buttonBg.getHeight()) / 2;

    buttons[0]->setX(x);
    buttons[0]->setY(y);

    textInputs[0]->setX(x);
    textInputs[0]->setY(y - 40 - 20);

    unsigned int len = sizeof(theirAddr);
    int n = recvfrom(sockfd, (char *)recBuf, 512, MSG_WAITALL, (struct sockaddr *)&theirAddr, &len);
    if (n != -1)
    {
        Message *msg = deserialize(recBuf);
        if (msg->type == 1)
        {
            GameBeginMessage *beginMsg = dynamic_cast<GameBeginMessage *>(msg);
            LGame *myGame = new LGame(window, textInputs[0]->getText(), beginMsg->opponentName, sockfd, theirAddr);
            window.setCurrScreen(myGame);
        }
        delete msg;
    }
}

void MainMenu::render(SDL_Renderer *renderer)
{
    SDL_Rect defViewport = {0, 0, window.getWidth(), window.getHeight()};
    SDL_RenderSetViewport(renderer, &defViewport);
    // std::cout << window.getHeight() << std::endl;

    SDL_SetRenderDrawColor(renderer, 3, 211, 252, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->render(renderer);
    }
    for (int i = 0; i < textInputs.size(); i++)
    {
        textInputs[i]->render(renderer);
    }
    waitingText->render(renderer, (window.getWidth() - waitingText->getWidth()) / 2, buttons[0]->getY() + buttons[0]->getHeight() + 32);
}

void MainMenu::cleanUp()
{
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->cleanUp();
        delete buttons[i];
    }
    for (int i = 0; i < textInputs.size(); i++)
    {
        textInputs[i]->cleanUp();
        delete textInputs[i];
    }
}