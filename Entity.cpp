#include <string>
#include <functional>
#include "Entity.h"
#include "Player.h"

Entity::Entity(std::string nameOfEntity, std::function<void(Player &player)> onCollidefunciton)
{
    name = nameOfEntity;
    onCollide = onCollidefunciton;
}

std::string Entity::getName()
{
    return name;
}

void Entity::collided(Player &player)
{
    onCollide(player);
}