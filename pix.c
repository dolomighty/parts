
void pix( int x , int y , int r , int g , int b ){
  SDL_SetRenderDrawColor( renderer , r,g,b , 255 );
  SDL_RenderDrawPoint( renderer , x , y );
}

