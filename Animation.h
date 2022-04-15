#include<SDL2/SDL.h>
#include<vector>
#include"MyTexture.h"
#include "Renderable.h"

class Animation : public Renderable 
{
    private : 
        LTexture &animationTexture ; 
        int mFrames ; 
        int animationSpeed ; 
        std::vector<SDL_Rect> animationImages;
        int mWidth , mHeight ; 
        SDL_Rect mBox ; 
        int numberOfAnimationImages ; 
    public : 
        Animation( LTexture &texture  , int width , int height ) ;
        ~Animation() ;
        int render(SDL_Renderer *renderer, SDL_Rect &camera);
        void cleanup() ; 
        void setBox( int x , int y ) ;
} ; 