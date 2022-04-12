#include<string>
#include<functional>
#include"Entity.h"

Entity :: Entity( std::string nameOfEntity , std:: function<void(void)> onCollidefunciton ) 
{
    name = nameOfEntity ; 
    onCollide = onCollidefunciton ; 
}

std::string Entity::getName()
{
    return name ; 
} 