#include<string>
#include<functional>

class Entity{

    public : 

        Entity( std::string nameOfEntity , std:: function<void(void)> onCollidefunciton ) ; 
        std::string getName() ; 
        

    private : 

        std::string name ; 
        std::function<void(void)> onCollide ; 

} ; 