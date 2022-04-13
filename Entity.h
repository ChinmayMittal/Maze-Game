#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <functional>

#include "Player.h"

class Entity
{

public:
    Entity(std::string nameOfEntity, std::function<void(Player &player)> onCollidefunciton);
    std::string getName();
    void collided(Player &player);

private:
    std::string name;
    std::function<void(Player &player)> onCollide;
};

#endif