#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <functional>

#include "Player.h"

class Entity
{

public:
    Entity(std::string nameOfEntity, std::function<void(Player &player, std::string &displayText)> onCollideFunction);
    Entity(std::string nameOfEntity, std::function<void(Player &player, std::string &displayText)> onCollideFunction,
           std::function<void(SDL_Event &e, Player &player)> eventHandlerFunction);
    std::string getName();
    void collided(Player &player, std::string &displayText);
    void handleEvent(SDL_Event &e, Player &player);

private:
    std::string name;
    std::function<void(Player &player, std::string &displayText)> onCollide;
    std::function<void(SDL_Event &e, Player &player)> eventHandler;
};

#endif