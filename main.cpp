
#include <stdlib.h>
#include <unistd.h>
#include <SDL.h>                // HEADER
#include "loop.h"

#undef NDEBUG
#include <assert.h>

#define WID 800                 // HEADER
#define HGT 600                 // HEADER

SDL_Renderer *renderer;         // HEADER

int main(int argc, char *argv[])
{

    SDL_Window *win;
    assert(win = SDL_CreateWindow(basename(argv[0]),
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, WID, HGT, 0));

    assert(renderer = SDL_CreateRenderer(win, -1, 0));

    loop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    return 0;
}
