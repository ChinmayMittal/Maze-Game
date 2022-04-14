#include "SearchOpponent.h"
#include "MyWindow.h"
#include "stdio.h"
#include "MessageStructs.h"
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include "Game.h"

SearchOpponent::SearchOpponent(LWindow &window, std::string playerName) : LScreen(window), playerName(playerName)
{
    if (!(sd = SDLNet_UDP_Open(2000)))
    {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
    }

    if (!(p = SDLNet_AllocPacket(512)))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
    }

    if (SDLNet_ResolveHost(&broadcastAdd, "255.255.255.255", 2000))
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    }

    NewClientMessage newClientMsg;
    newClientMsg.name = playerName;
    int bytesUsed = serialize(&newClientMsg, (char *)p->data);
    p->address.host = broadcastAdd.host;
    p->address.port = broadcastAdd.port;
    p->len = bytesUsed;
    // int j = 4;
    // while (p->data[j] != 0)
    // {
    //     std::cout << p->data[j];
    //     j++;
    // }
    SDLNet_UDP_Send(sd, -1, p);
}

void SearchOpponent::handleEvent(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_KEYUP:
        if (e.key.keysym.sym == SDLK_RETURN)
        {
            LGame *myGame = new LGame(window);
            window.setCurrScreen(myGame);
        }
        break;
    default:
        break;
    }
}

void SearchOpponent::update()
{
    if (SDLNet_UDP_Recv(sd, p))
    {
        // int j = 4;
        // while (p->data[j] != 0)
        // {
        //     std::cout << p->data[j];
        //     j++;
        // }
        NewClientMessage newClientMsg;
        deserialize((char *)p->data, &newClientMsg);
        std::cout << newClientMsg.name << std::endl;
    }
}

void SearchOpponent::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 3, 211, 252, 255);
    SDL_RenderClear(renderer);
}

void SearchOpponent::cleanUp()
{
    SDLNet_FreePacket(p);
}