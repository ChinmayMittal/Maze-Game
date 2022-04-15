#ifndef SEARCH_OPPONENT
#define SEARCH_OPPONENT

#include "Screen.h"
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

class SearchOpponent : public LScreen
{
private:
    std::string playerName;
    // UDPsocket sd;
    // UDPpacket *p;
    // IPaddress broadcastAdd;
    int sockfd;
    struct sockaddr_in myAddr, theirAddr;
    char buf[512];

public:
    SearchOpponent(LWindow &window, std::string playerName);
    void handleEvent(SDL_Event &e);
    void update();
    void render(SDL_Renderer *renderer);
    void cleanUp();
};

#endif