

#include <assert.h>
#include <stdlib.h>
#include <SDL.h>
#include <stdint.h>
#include <time.h>
#include "macros.h"
#include "XY.h"
#include "main.h"



typedef struct RGB8 {
    uint8_t r,g,b;
} RGB8;


typedef struct PART {
    XY pos;
    XY spd;
} PART;


#define CLASSI 2


typedef struct PARTS {
//  float mass;   // f=ma & C.
//  float drag;   // che si applica a spd²
//  float radius;
  RGB8 rgb;
  int pcount;
  PART *plist;
  float interact_matrix[CLASSI];
} PARTS;


PARTS parts[CLASSI];


const float EPS  = 1e-5;





void parts_init() // HEADER
{
  const RGB8 rgb[]={
    {  63 ,  63 , 255 },    // blu
    { 255 ,  63 ,  63 },    // red
    {  63 , 255 ,  63 },    // green
    { 255 , 128 ,  63 },    // orange
    {  63 ,  63 ,  63 },    // gray
    {  63 , 255 , 255 },    // cyan
    { 255 ,  63 , 255 },    // magen
    { 255 , 255 ,  63 },    // yel
    { 255 , 255 , 255 },    // white
  };

  srand(time(0));

  int j;
  for( j=0 ; j<CLASSI ; j++ )
  {
    PARTS *P = &parts[j];
    // genera le classi

    P->rgb = rgb[j];

//    P->mass = 1;
//    P->drag = 0.1;
//    P->radius = 1;

//    // genera una matrice di interazione
//    memset(P->interact_matrix,0,sizeof(P->interact_matrix));
//    int i;
//    for( i=0 ; i<CLASSI ; i++ )
//    {
//      P->interact_matrix[i] = BIPORAND * 1;
//    }

    // crea le particelle e le posiziona a casaccio
//    P->pcount=10;
    P->pcount = 500;
    int bytes = parts[j].pcount * sizeof(P->plist[0]);
    P->plist = (PART*)malloc(bytes);
    assert(P->plist);
    memset(P->plist,0,bytes);

    for( int i=0 ; i<P->pcount ; i++ )
    {
//      fprintf(stderr,"%d %f %f\n",i,p.x,p.y);
      P->plist[i].pos.x = BIPORAND*100 + SCREEN_W/2;
      P->plist[i].pos.y = BIPORAND*100 + SCREEN_H/2;
//      P->plist[i].spd.x = BIPORAND*10;
//      P->plist[i].spd.y = BIPORAND*10;

//        // mettiamo le particelle in una traiettoria circolare orbitante il centro di massa iniziale
//        P->plist[i].pos.x = BIPORAND*100;
//        P->plist[i].pos.y = BIPORAND*100;
//        float rr = sqrt( P->plist[i].pos.x*P->plist[i].pos.x + P->plist[i].pos.y*P->plist[i].pos.y );
//        float v = sqrt(20/(rr+EPS));
//        P->plist[i].spd.x = -P->plist[i].pos.y*v/(sqrt(rr)+EPS);
//        P->plist[i].spd.y = +P->plist[i].pos.x*v/(sqrt(rr)+EPS);
    }
  }

    // similars attract, differents repel
#define I(A,B) parts[A].interact_matrix[B]

    I(0,0) = +1;    I(0,1) = -1;
    I(1,0) = -1;    I(1,1) = +1;

//    I(0,0) = +1;    I(0,1) = -1;    I(0,2) = -1;
//    I(1,0) = -1;    I(1,1) = +1;    I(1,2) = -1;
//    I(2,0) = -1;    I(2,1) = -1;    I(2,2) = +1;
}





// minmax per autoscaling nel rendering
XY min;
XY max;

static float invlerp( float lo, float hi, float x )
{
    return (x-lo)/(hi-lo);
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
        xy[A].x = invlerp( min.x, max.x, P->plist[A].pos.x )*SCREEN_W ;
        xy[A].y = invlerp( min.y, max.y, P->plist[A].pos.y )*SCREEN_H;
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
        xywh[A].x = invlerp( min.x, max.x, P->plist[A].pos.x )*SCREEN_W -1;
        xywh[A].y = invlerp( min.y, max.y, P->plist[A].pos.y )*SCREEN_H-1;
        xywh[A].w = 3;
        xywh[A].h = 3;
    }
    SDL_SetRenderDrawColor( renderer , P->rgb.r,P->rgb.g,P->rgb.b , 255 );
    SDL_RenderFillRects( renderer , xywh , COUNT(xywh));
}






void parts_draw( SDL_Renderer *renderer ) // HEADER
{
    // minmax
    min.x = min.y = 0;
    max.x = SCREEN_W ;
    max.y = SCREEN_H;
    int C;
    for( C=0 ; C<COUNT(parts) ; C++ )
    {
        PARTS *P = &parts[C];
        int A;
        for( A=0 ; A<P->pcount ; A++ )
        {
            if( min.x > P->plist[A].pos.x ) min.x = P->plist[A].pos.x;
            if( min.y > P->plist[A].pos.y ) min.y = P->plist[A].pos.y;
            if( max.x < P->plist[A].pos.x ) max.x = P->plist[A].pos.x;
            if( max.y < P->plist[A].pos.y ) max.y = P->plist[A].pos.y;
        }
    }

    // viewport iniziale per riferimento di scala
    SDL_SetRenderDrawColor( renderer , 128,128,128, 255 );
    SDL_Rect xywh;
    xywh.x = invlerp( min.x, max.x,   0 )*SCREEN_W ;
    xywh.y = invlerp( min.y, max.y,   0 )*SCREEN_H;
    xywh.w = invlerp( min.x, max.x, SCREEN_W  )*SCREEN_W  - xywh.x;
    xywh.h = invlerp( min.y, max.y, SCREEN_H )*SCREEN_H - xywh.y;
    SDL_RenderDrawRect( renderer , &xywh );


    for( C=0 ; C<COUNT(parts) ; C++ )
    {
        draw_dots(renderer,&parts[C]);
//        draw_rects(renderer,&parts[C]);
    }

}



static float
sign( float x )
{
  return x>0 ? +1 : -1;
}


static float 
delta
( XY *D, XY *A , XY *B )
{
  // calcola il vettore delta AB, in uno spazio toroidale
  // vengono generati 9 vettori delta (AB e gli alias ABn)
  // vengono poi confrontati e ritornato il più corto, es.
  // ___________ __________ __________ 
  // |       B₁.|       B₂.|       B₃.|
  // |          |          |          |
  // |          |          |          |
  // |          |          |          |
  // |__________|__________|__________|
  // |       B₄.|        B.|       B₅.|
  // |          | .A       |          |
  // |          |          |          |
  // |          |          |          |
  // |__________|__________|__________|
  // |       B₆.|       B₇.|       B₈.|
  // |          |          |          |
  // |          |          |          |
  // |          |          |          |
  // |__________|__________|__________|
  //
  // il vettore più corto in questo caso è AB₄

  // spazio standard (no toroidale)
  *D = *B - *A;
  return *D dot *D;

//  XY Bn[3*3] = {
//    { B->x-(SCREEN_W +20) - A->x, B->y-(SCREEN_H+20) - A->y },
//    { B->x          - A->x, B->y-(SCREEN_H+20) - A->y },
//    { B->x+(SCREEN_W +20) - A->x, B->y-(SCREEN_H+20) - A->y },
//    { B->x-(SCREEN_W +20) - A->x, B->y          - A->y },
//    { B->x          - A->x, B->y          - A->y },
//    { B->x+(SCREEN_W +20) - A->x, B->y          - A->y },
//    { B->x-(SCREEN_W +20) - A->x, B->y+(SCREEN_H+20) - A->y },
//    { B->x          - A->x, B->y+(SCREEN_H+20) - A->y },
//    { B->x+(SCREEN_W +20) - A->x, B->y+(SCREEN_H+20) - A->y },
//  };
//
//  D->x = Bn[0].x;
//  D->y = Bn[0].y;
//  float rr = D->x*D->x + D->y*D->y;
//
//  int i;
//  for( i=1; i<COUNT(Bn); i++ ){
//    XY Dn = { Bn[i].x-A->x, Bn[i].y-A->y };
//    float dd = Dn.x*Dn.x + Dn.y*Dn.y;
//    if( dd >= rr ) continue;
//    *D = Dn;
//    rr = dd;
//  }
//
//  return rr;
}





static void
integraz_forza( PARTS *C , PART *A )
{
    // per ora usiamo l'approccio naif O(n²)
    // poi impementerò una griglia di accelerazione
    assert(A);
    assert(C);
    XY fsum;
    for( int ci=0 ; ci<COUNT(parts) ; ci++ )
    {
        for( int bi=0 ; bi<parts[ci].pcount ; bi++ )
        {
            PART *B = &parts[ci].plist[bi];
            if( B == A )continue; // auto-interazione? no grazie

            XY d = B->pos - A->pos;   // ab
            float rr = d dot d; // d²

//            // le parts si respingono con 1/r²
//            // parts simili si attirano con hooke²
//            float f =  - (1/(rr+EPS));
//            if( C == &parts[ci] ) f += (rr*0.0000001);

            // le parts si attirano con 1/r²
            // parts diverse si respingono con hooke²
            
            // plot [0:3] 1/(x**2)
            // plot [0:3] x/(0.1+x)
            // range=0.01 ; plot [0:3] (x**4/(range+x**4))/(x**2)

            float r4 = rr*rr;
            float f =  (r4/(0.01+r4))/(rr+EPS);
            if( C != &parts[ci] ) f -= (rr*0.0000001);

            // la particella B è di classe ci
            A->pos.x += d.x/sqrt(rr)*f;
            A->pos.y += d.y/sqrt(rr)*f;
        }
    }
}







static void 
integraz_forze()
{
    // per ora usiamo l'approccio naif O(n²)
    // poi impementerò una griglia di accelerazione
    // o qualche altro schema
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






void parts_update( float dt ) // HEADER
{
    integraz_forze();

//    // integraz posizione
//    int C;
//    for( C=0 ; C<COUNT(parts) ; C++ )
//    {
//        PARTS *P = &parts[C];
//        int A;
//        for( A=0 ; A<P->pcount ; A++ )
//        {
//            P->plist[A].pos.x += P->plist[A].spd.x * dt;
//            P->plist[A].pos.y += P->plist[A].spd.y * dt;
//        }
//    }

}


