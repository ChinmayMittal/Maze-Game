#include <string>
#include <functional>
#include "Entity.h"
#include "Player.h"

Entity::Entity(std::string nameOfEntity, std::function<void(Player &player , std::string &displayText)> onCollidefunciton)
{
    name = nameOfEntity;
    onCollide = onCollidefunciton;
}

std::string Entity::getName()
{
    return name;
}

void Entity::collided(Player &player , std::string &displayText )
{
    onCollide(player , displayText );
}