#include "SoundEffect.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>
#include <memory>

SoundEffect::SoundEffect()
{
    // initialize variables
    mMusic = NULL;
}

SoundEffect ::SoundEffect(std ::string filename)
{
    // initialize variables
    mMusic = NULL;
    loadMusic(filename);
}

SoundEffect::~SoundEffect()
{
    // Deallocate
    free();
}

// SoundEffect::SoundEffect(const std::string &path, int volume)
//     : chunk(Mix_LoadWAV(path.c_str()), Mix_FreeChunk) {
//     if (!chunk.get()) {
//         // LOG("Couldn't load audio sample: ", path);
//     }

//     Mix_VolumeChunk(chunk.get(), volume);
// }

bool SoundEffect::loadMusic(std ::string filename)
{
    // get rid of pre-existing music
    free();
    // std::cout << "loading music...\n" ;
    mMusic = Mix_LoadWAV(filename.c_str());
    if (mMusic == NULL)
    {
        printf("Failed to load music!\n");
        std::cout << Mix_GetError() << "\n";
        return false;
    }
    // std::cout << "loaded music...\n" ;
    return true;
}

void SoundEffect::free()
{
    // Free music if it exists
    if (mMusic != NULL)
    {
        // Free the sound effects
        Mix_FreeChunk(mMusic);
        mMusic = NULL;
    }
}

void SoundEffect::play()
{
    int channel = Mix_PlayChannel(-1, mMusic, 0);
    // std::cout << ( mMusic == NULL ) << "\n" ;
    if (channel == -1)
    {
        printf("Mix_PlayChannel: %s\n", Mix_GetError());
    }
    else
    {
        // std::cout << "playing on channel " << channel << "\n";
    }
    // SDL_Delay(1000) ;
    // Mix_PlayChannel(-1, chunk.get(), 0);
}