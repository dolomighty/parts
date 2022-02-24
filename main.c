
// IMPL

#include <stdlib.h>
#include <unistd.h>

// ENDIMPL

#define     WID     800
#define     HGT     600

#include <SDL2/SDL.h>

// IFNIMPL extern
SDL_Renderer *renderer;


// IMPL

//#include "pix.c"
#include "loop.c"

// ENDIMPL


int main( int argc , char *argv[] )
// IMPL
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
// ENDIMPL


