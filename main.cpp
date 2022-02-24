
#include <SDL.h>   // HEADER

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define     SCREEN_W     800 // HEADER
#define     SCREEN_H     600 // HEADER


SDL_Renderer *renderer; // HEADER


#include "loop.cpp"



int main( int argc , char *argv[] )
{ 
    
    SDL_Window   *win;

    assert( win = SDL_CreateWindow( basename(argv[0]), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, 0 ));
    assert( renderer = SDL_CreateRenderer( win , -1 , 0 ));

    loop();

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( win );

    return 0 ;
}

