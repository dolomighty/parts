

#include "parts.h"


void frame( SDL_Event event ){

  SDL_SetRenderDrawColor( renderer , 0,0,0 , 255 );
  SDL_RenderClear( renderer );

  float dt = 0.1;
  Uint32 msec = SDL_GetTicks();
  static Uint32 prev_msec = 0;
  if( prev_msec ) dt = (msec-prev_msec)/1000.0;
  prev_msec = msec;

//  fprintf(stderr,"dt %f\n",dt);

  parts_update( dt );
  parts_draw( renderer );

//  pix( rand()%WID , rand()%HGT, rand()%256 , rand()%256 , rand()%256 );
//  pix( event.motion.x , event.motion.y , 255 , 255 , 255 );
}

