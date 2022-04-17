#ifndef NPC_H
#define NPC_H
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<vector>
#include<string>
#include"Renderable.h"
#include"MyTexture.h"
#include "Timer.h"

class LGame;

class NPC : public Renderable

{
    public:
        NPC(LTexture &myTexture, std::string nameOfNPC , LGame &game, int NPCHeight, int NPCWidth, int right, int left, int top, int bottom);        
        void move() ; 
        int render(SDL_Renderer *renderer, SDL_Rect &camera);        
        void cleanUp();
        SDL_Rect getBox() ; 
        void switchDirection() ;
        std::string getName() ;  
        bool isBusy() ; 
        void update() ; 
        void setCoolDown( int t ) ; 
    private:
        LGame &mGame ; 
        std::string name ; 
        SDL_Rect mBox ;
        LTexture &mTexture ; 
        int NPCHeight , NPCWidth ; 
        int mframes ; 
        int right, top, left, bottom ; 
        int numOfAnimationImages ; 
        int animationSpeed = 8 ; 
        char direction ; 
        std::vector<SDL_Rect> NPCImages ; 
        int velocity ; 
        int mVelX , mVelY ; 
        int changeSpeed ; 
        int current ; 
        LTimer coolDownTimer ; 
        int coolDownTime ; 

} ;

#endif