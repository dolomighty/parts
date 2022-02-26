
#include "parts.h"

void frame(SDL_Event event)
{

    float dt = 0.1;
    Uint32 msec = SDL_GetTicks();
    static Uint32 prev_msec = 0;
    if (prev_msec)
        dt = (msec - prev_msec) / 1000.0;
    prev_msec = msec;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    parts_update(dt);
    parts_draw(renderer);

//  // mouse pointer
//  SDL_Rect xywh;
//  xywh.x = event.motion.x-2,
//  xywh.y = event.motion.y-2,
//  xywh.w = 5,
//  xywh.h = 5,
//  SDL_SetRenderDrawColor( renderer , 255,255,255 , 255 );
//  SDL_RenderFillRect( renderer , &xywh );
}
