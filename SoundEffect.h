#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#include<string>
#include<memory>

class SoundEffect 
{
    public: 
        SoundEffect() ; 
        SoundEffect( std :: string filename ) ; 
        // SoundEffect( const std::string &path, int volume ) ; 
        ~SoundEffect() ; 
        bool loadMusic( std :: string filename ) ; 
        void free();
        void play() ; 
    private: 
        Mix_Chunk *mMusic ; 
        // std::unique_ptr<Mix_Chunk, void (*)(Mix_Chunk *)> chunk;

} ; 