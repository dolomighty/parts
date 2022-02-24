

#include <SDL.h>    // HEADER

#include <stdint.h>


typedef struct XYf {
  float x,y;
} XYf;


typedef struct RGB8 {
  uint8_t r,g,b;
} RGB8;


typedef struct PART {
  XYf pos;
  XYf spd;
} PART;


// penso anche che il drag possa variare con la famiglia
// per che so simulare corpi di dimensioni differenti dispersi nel fluido
// alla stessa maniera la massa, cosi da includere l'inerzia.
// ed anche la dimensione, a questo punto, che potrebbe influenzare
// quando la repulsione coulombiana interviene. quindi:

#define CLASSI 8


typedef struct PARTS {
  float mass;   // f=ma & C.
  float drag;   // che si applica a spd²
  float radius;
  RGB8 rgb;
  int pcount;
  PART *plist;
  float interact_matrix[CLASSI];
} PARTS;


// IFNIMPL extern
PARTS parts[CLASSI];



// volendo un'approccio statico.
// se volessimo un approccio dinamico, userei una hash
// con chiavi ad es. "0vs1", "2vs5"
// per ottenere la forza che 0 sente interagendo con 1, 
// che 2 sente interagendo con 5, ecc

// c'è cmq una forza aggiuntiva per evitare sovrapposizioni.
// una cosa simile alla repulsione dovuta al principio di pauli
// https://it.wikipedia.org/wiki/Principio_di_esclusione_di_Pauli
// ci sono varie formule empiriche per le repulsioni interatomiche
// ci si può giocare




#include "macros.h"
#include <assert.h>
#include <stdlib.h>
#include "main.h"


void parts_init() // HEADER
{
  const RGB8 rgb[]={
    {  63 ,  63 ,  63 },
    {  63 ,  63 , 255 },
    {  63 , 255 ,  63 },
    {  63 , 255 , 255 },
    { 255 ,  63 ,  63 },
    { 255 ,  63 , 255 },
    { 255 , 255 ,  63 },
    { 255 , 255 , 255 },
  };
  int j;
  for( j=0 ; j<CLASSI ; j++ )
  {
    PARTS *P = &parts[j];
    // genera le classi

//    // colore
//    P->rgb.r = 64+rand()*(64-255L)/(RAND_MAX-1);
//    P->rgb.g = 64+rand()*(64-255L)/(RAND_MAX-1);
//    P->rgb.b = 64+rand()*(64-255L)/(RAND_MAX-1);

    P->rgb = rgb[j];

    P->mass = 1;
    P->drag = 0.1;
    P->radius = 1;

    // genera una matrice di interazione
    memset(P->interact_matrix,0,sizeof(P->interact_matrix));
    int i;
    for( i=0 ; i<CLASSI ; i++ )
    {
      P->interact_matrix[i] = BIPORAND * 1;
    }

    // crea le particelle e le posiziona a casaccio
    P->pcount=200;
    int bytes = parts[j].pcount * sizeof(P->plist[0]);
    P->plist = (PART*)malloc(bytes);
    assert(P->plist);
    memset(P->plist,0,bytes);

    for( i=0 ; i<P->pcount ; i++ )
    {
//      fprintf(stderr,"%d %f %f\n",i,p.x,p.y);
      P->plist[i].pos.x = (float)rand()*WID/RAND_MAX;
      P->plist[i].pos.y = (float)rand()*HGT/RAND_MAX;
      P->plist[i].spd.x = BIPORAND*10;
      P->plist[i].spd.y = BIPORAND*10;
    }
  }
}


static void
draw_dots( SDL_Renderer *renderer , PARTS *P )
{
  assert(P);
  assert(P->plist);
  SDL_Point xy[P->pcount];
  int A;
  for( A=0 ; A<P->pcount ; A++ )
  {
    xy[A].x = P->plist[A].pos.x;
    xy[A].y = P->plist[A].pos.y;
//    fprintf(stderr,"%d %d\n",xy[A].x,xy[A].y);
//    fprintf(stderr,"%d %f %f\n",A,P->plist[A].pos.x,P->plist[A].pos.y);
  }
  SDL_SetRenderDrawColor( renderer , P->rgb.r,P->rgb.g,P->rgb.b , 255 );
  SDL_RenderDrawPoints( renderer , xy , COUNT(xy));
}







static void
draw_rects( SDL_Renderer *renderer , PARTS *P )
{
  assert(P);
  assert(P->plist);
  SDL_Rect xywh[P->pcount];
  int A;
  for( A=0 ; A<P->pcount ; A++ )
  {
    xywh[A].x = P->plist[A].pos.x-1;
    xywh[A].y = P->plist[A].pos.y-1;
    xywh[A].w = 3;
    xywh[A].h = 3;
  }
  SDL_SetRenderDrawColor( renderer , P->rgb.r,P->rgb.g,P->rgb.b , 255 );
  SDL_RenderFillRects( renderer , xywh , COUNT(xywh));
}






void parts_draw( SDL_Renderer *renderer ) // HEADER
{
  int C;
  for( C=0 ; C<COUNT(parts) ; C++ )
  {
//    draw_dots(renderer,&parts[C]);
    draw_rects(renderer,&parts[C]);
  } 
}



static float
sign( float x )
{
  return x>0 ? +1 : -1;
}


static void
integraz_forza( PARTS *C , PART *A )
{
  // integra le forze su A (della classe C)
  // dovute a tutte le altre parts
  // la dinamica avviene in un secondo momento

  const float EPS  = 1e-5;
  const float MAXF = 1e3;

  // per ora usiamo l'approccio naif O(n²)
  // poi impementerò una griglia di accelerazione
  assert(A);
  assert(C);
  XYf fsum={0,0};
  int ci;
  for( ci=0 ; ci<COUNT(parts) ; ci++ )
  {
    int bi;
    for( bi=0 ; bi<parts[ci].pcount ; bi++ )
    {
      PART *B = &parts[ci].plist[bi];
      if(B==A)continue; // auto-interazione? no grazie

      XYf d = { A->pos.x-B->pos.x , A->pos.y-B->pos.y };
      float rr = d.x*d.x+d.y*d.y; // d²

      // oltre questa dist, non c'è interaz
      if(rr>10.0*10.0)continue;

      // quasi https://it.wikipedia.org/wiki/Potenziale_di_Lennard-Jones
      float r6  = rr*rr*rr; // d²*³=d⁶
      float r12 = rr*rr*rr*rr*rr*rr; // d²*⁶=d¹²

// evitiamo div0 e comportamenti brutti
      float f = 1/(r12+EPS)-1/(rr+EPS);
      if(f>+MAXF) f=+MAXF; else
      if(f<-MAXF) f=-MAXF;

      // la particella B è di classe ci
      fsum.x += d.x*f*C->interact_matrix[ci];
      fsum.y += d.y*f*C->interact_matrix[ci];
    }
  }

//  // drag
//  // attrito modellato come forza opposta alla velocità
//  // e proporzionale a vel²:
//  //  fdrag = -spd * |spd|² / |spd| * Kdrag
//  // e ok. vediamo come calcolarla al meglio:
//  //  fdrag.x = x * (x²+y²) / √(x²+y²) * Kdrag
//  // però se aggiungiamo che:
//  //  d = √(x²+y²) →
//  //  d² = d*d = (x²+y²)
//  // perciò:
//  //  fdrag = x*d*d/d*k = x*d*k
//  float d = sqrt( A->spd.x*A->spd.x + A->spd.y*A->spd.y );
//  fsum.x -= A->spd.x*d*0.001;
//  fsum.y -= A->spd.y*d*0.001;

  // drag
  // modellato come ... boh
  A->spd.x *= 0.99;
  A->spd.y *= 0.99;

  A->spd.x += fsum.x / C->mass;
  A->spd.y += fsum.y / C->mass;
}







static void
integraz_forze()
{
  // per ora usiamo l'approccio naif O(n²)
  // poi impementerò una griglia di accelerazione
  int ci;
  for( ci=0 ; ci<COUNT(parts) ; ci++ )
  {
    int ai;
    for( ai=0 ; ai<parts[ci].pcount ; ai++ )
    {
      integraz_forza( &parts[ci] , &parts[ci].plist[ai] );
    }
  }
}






void parts_update( float dt )    // HEADER
{
  integraz_forze();

  // integraz posizione
  int C;
  for( C=0 ; C<COUNT(parts) ; C++ )
  {
    PARTS *P = &parts[C];
    int A;
    for( A=0 ; A<P->pcount ; A++ )
    {
      P->plist[A].pos.x += P->plist[A].spd.x * dt;
      P->plist[A].pos.y += P->plist[A].spd.y * dt;

      // wrap. uso un playfield allargato, tanto il clipping è gratis
      if( P->plist[A].pos.x <     -10 ) P->plist[A].pos.x += WID+20;  else
      if( P->plist[A].pos.x >= WID+10 ) P->plist[A].pos.x -= WID+20;
      if( P->plist[A].pos.y <     -10 ) P->plist[A].pos.y += HGT+20;  else
      if( P->plist[A].pos.y >= HGT+10 ) P->plist[A].pos.y -= HGT+20;
    }
  }

}


