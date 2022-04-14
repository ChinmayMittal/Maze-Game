#include <string>
#include <functional>
#include "Entity.h"
#include "Player.h"

Entity::Entity(std::string nameOfEntity, std::function<void(Player &player, std::string &displayText)> onCollideFunction)
{
    name = nameOfEntity;
    onCollide = onCollideFunction;
    eventHandler = [&](SDL_Event &e, Player &player) {};
}

Entity::Entity(std::string nameOfEntity, std::function<void(Player &player, std::string &displayText)> onCollideFunction, std::function<void(SDL_Event &e, Player &player)> eventHandlerFunction)
{
    name = nameOfEntity;
    onCollide = onCollideFunction;
    eventHandler = eventHandlerFunction;
}

std::string Entity::getName()
{
    return name;
}

void Entity::collided(Player &player, std::string &displayText)
{
    onCollide(player, displayText);
}

void Entity::handleEvent(SDL_Event &e, Player &player)
{
    eventHandler(e, player);
}