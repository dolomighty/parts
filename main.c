
#include <stdlib.h>
#include <unistd.h>

#define     WID     800 // HEADER
#define     HGT     600 // HEADER

#include <SDL.h>   // HEADER

SDL_Renderer *renderer; // HEADER


//#include "pix.c"
#include "loop.c"



int main( int argc , char *argv[] )
{ 
    
  SDL_Window *win = 
    SDL_CreateWindow( "sdl2" , 
      SDL_WINDOWPOS_UNDEFINED , SDL_WINDOWPOS_UNDEFINED , 
        WID , HGT , 0 );

  renderer = SDL_CreateRenderer( win , -1 , 0 );

  loop();
  
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( win );
  
  return 0 ;
}

