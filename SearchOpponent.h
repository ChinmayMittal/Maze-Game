#ifndef SEARCH_OPPONENT
#define SEARCH_OPPONENT

#include "Screen.h"
#include <string>
#include <SDL2/SDL_net.h>

class SearchOpponent : public LScreen
{
private:
    std::string playerName;
    UDPsocket sd;
    UDPpacket *p;
    IPaddress broadcastAdd;

public:
    SearchOpponent(LWindow &window, std::string playerName);
    void handleEvent(SDL_Event &e);
    void update();
    void render(SDL_Renderer *renderer);
    void cleanUp();
};

#endif