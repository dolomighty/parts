
#include <SDL.h>
#include "frame.h"
#include "parts.h"
#include "main.h"



void loop()     // HEADER
{

    parts_init();

    while(1){

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            switch (event.type){
            case SDL_KEYDOWN:
//                    printf( "Il tasto %s e' stato premuto!\n",
//                           SDL_GetKeyName(event.key.keysym.sym));
//                    switch( event.key.keysym.sym ){
//                      case SDLK_1 :
//                        printf( "1\n" );
//                      break;
//                    }
                break;
            case SDL_KEYUP:
                break;
            case SDL_MOUSEMOTION:
//                    printf("Il Mouse e' stato mosso da %d,%d "
//                               "a(%d,%d)\n", 
//                           event.motion.xrel, event.motion.yrel,
//                           event.motion.x , event.motion.y );
                break;
            case SDL_MOUSEBUTTONDOWN:
//                    printf(
//                        "Il pulsante %d del Mouse e' stato "
//                        "premuto a(%d,%d)\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y );
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            case SDL_QUIT:
//                    printf( "SDL_QUIT\n" );
                return;
            }
        }

        frame(event);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
}
