#include "SearchOpponent.h"
#include "MyWindow.h"
#include "MessageStructs.h"
#include <string>
#include <SDL2/SDL.h>
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
#include <iostream>
#include "Game.h"

SearchOpponent::SearchOpponent(LWindow &window, std::string playerName) : LScreen(window), playerName(playerName)
{
    // if (!(sd = SDLNet_UDP_Open(2000)))
    // {
    //     fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
    // }

    // if (!(p = SDLNet_AllocPacket(512)))
    // {
    //     fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
    // }

    // if (SDLNet_ResolveHost(&broadcastAdd, "192.168.227.102", 2001))
    // {
    //     fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    // }

    // memset(&hints, 0, sizeof hints);
    // hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    // hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_flags = AI_PASSIVE; // use my IP

    // if ((rv = getaddrinfo(NULL, "2000", &hints, &servinfo)) != 0)
    // {
    //     fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    // }

    // for (p = servinfo; p != NULL; p = p->ai_next)
    // {
    //     if ((sockfd = socket(p->ai_family, p->ai_socktype,
    //                          p->ai_protocol)) == -1)
    //     {
    //         perror("listener: socket");
    //         continue;
    //     }

    //     if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    //     {
    //         close(sockfd);
    //         perror("listener: bind");
    //         continue;
    //     }

    //     break;
    // }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
    }

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // int broadcast = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
    //                sizeof(broadcast)) == -1)
    // {
    //     perror("setsockopt (SO_BROADCAST)");
    // }

    memset(&myAddr, 0, sizeof(myAddr));
    memset(&theirAddr, 0, sizeof(theirAddr));

    myAddr.sin_family = AF_INET; // IPv4
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(2000);

    theirAddr.sin_family = AF_INET;
    theirAddr.sin_port = htons(8080);
    // theirAddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "192.168.227.10", &(theirAddr.sin_addr.s_addr));

    // if (bind(sockfd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) < 0)
    // {
    //     perror("bind failed");
    // }

    // if (p == NULL)
    // {
    //     fprintf(stderr, "listener: failed to bind socket\n");
    // }

    // addr_len = sizeof their_addr;

    NewClientMessage newClientMsg;
    newClientMsg.name = playerName;
    int bytesUsed = serialize(&newClientMsg, buf);
    sendto(sockfd, buf, bytesUsed, 0, (const struct sockaddr *)&theirAddr,
           sizeof(theirAddr));

    // sockaddr_in sendAddr;
    // memset(&sendAddr, '\0', sizeof(sendAddr));
    // sendAddr.sin_family = AF_INET;
    // sendAddr.sin_port = 2000;
    // inet_pton(AF_INET, "255.255.255.255", &sendAddr.sin_addr);

    // if ((numbytes = sendto(sockfd, data, bytesUsed, 0,
    //                        (const struct sockaddr *)sendAddr, p->ai_addrlen)) == -1)
    // {
    //     perror("talker: sendto");
    // }

    // p->address.host = broadcastAdd.host;
    // p->address.port = broadcastAdd.port;
    // p->len = bytesUsed;
    // int j = 4;
    // while (p->data[j] != 0)
    // {
    //     std::cout << p->data[j];
    //     j++;
    // }
    // SDLNet_UDP_Send(sd, -1, p);
}

void SearchOpponent::handleEvent(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_KEYUP:
        if (e.key.keysym.sym == SDLK_s)
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
    // int numbytes = 0;
    // unsigned int len;
    // if ((numbytes = recvfrom(sockfd, buf, sizeof(buf) - 1, 0,
    //                          (struct sockaddr *)&theirAddr, &len)) != -1)
    // {
    //     // NewClientMessage newClientMsg;
    //     // deserialize((char *)buf, &newClientMsg);
    //     // std::cout << newClientMsg.name << std::endl;
    //     // perror("recvfrom");
    // }

    // if (SDLNet_UDP_Recv(sd, p))
    // {
    //     // int j = 4;
    //     // while (p->data[j] != 0)
    //     // {
    //     //     std::cout << p->data[j];
    //     //     j++;
    //     // }
    // }
}

void SearchOpponent::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 3, 211, 252, 255);
    SDL_RenderClear(renderer);
}

void SearchOpponent::cleanUp()
{
    // SDLNet_FreePacket(p);

    // freeaddrinfo(servinfo);
    close(sockfd);
}