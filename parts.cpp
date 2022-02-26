
#include <SDL.h>                // HEADER

#include <stdint.h>
#include <time.h>

typedef struct XYf {
    float x, y;
} XYf;

typedef struct RGB8 {
    uint8_t r, g, b;
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

const float EPS = 1e-5;

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

void parts_init()               // HEADER
{
    const RGB8 rgb[] = {
        {255, 255, 255},        // white
        {255, 63, 63},          // red
        {63, 255, 63},          // green
        {63, 63, 255},          // blu
        {255, 128, 63},         // orange
        {63, 63, 63},           // gray
        {63, 255, 255},         // cyan
        {255, 63, 255},         // magen
        {255, 255, 63},         // yel
    };

    srand(time(0));

    int j;
    for (j = 0; j < CLASSI; j++) {
        PARTS *P = &parts[j];
        // genera le classi

//    // colore
//    P->rgb.r = 64+rand()*(64-255L)/(RAND_MAX-1);
//    P->rgb.g = 64+rand()*(64-255L)/(RAND_MAX-1);
//    P->rgb.b = 64+rand()*(64-255L)/(RAND_MAX-1);

        P->rgb = rgb[j];

//    P->mass = 1;
//    P->drag = 0.1;
//    P->radius = 1;

        // genera una matrice di interazione
        memset(P->interact_matrix, 0, sizeof(P->interact_matrix));
        int i;
        for (i = 0; i < CLASSI; i++) {
            P->interact_matrix[i] = BIPORAND * 1;
        }

        // crea le particelle e le posiziona a casaccio
//    P->pcount=10;
        P->pcount = 500;
        int bytes = parts[j].pcount * sizeof(P->plist[0]);
        P->plist = (PART *) malloc(bytes);
        assert(P->plist);
        memset(P->plist, 0, bytes);

        for (i = 0; i < P->pcount; i++) {
//      fprintf(stderr,"%d %f %f\n",i,p.x,p.y);
//      P->plist[i].pos.x = BIPORAND*100 + WID/2;
//      P->plist[i].pos.y = BIPORAND*100 + HGT/2;
//      P->plist[i].spd.x = BIPORAND*10;
//      P->plist[i].spd.y = BIPORAND*10;

            // mettiamo le particelle in una traiettoria circolare orbitante il centro di massa iniziale
            P->plist[i].pos.x = BIPORAND * 100;
            P->plist[i].pos.y = BIPORAND * 100;
            float rr =
                sqrt(P->plist[i].pos.x * P->plist[i].pos.x +
                     P->plist[i].pos.y * P->plist[i].pos.y);
            float v = sqrt(20 / (rr + EPS));
            P->plist[i].spd.x = -P->plist[i].pos.y * v / (sqrt(rr) + EPS);
            P->plist[i].spd.y = +P->plist[i].pos.x * v / (sqrt(rr) + EPS);

            P->plist[i].pos.x += WID / 2;
            P->plist[i].pos.y += HGT / 2;
        }
    }

    // similars attract, differents repel
#define I(A,B) parts[A].interact_matrix[B]

    I(0, 0) = +1;
    I(0, 1) = -1;
    I(1, 0) = -1;
    I(1, 1) = +1;

//    I(0,0) = +1;    I(0,1) = -1;    I(0,2) = -1;
//    I(1,0) = -1;    I(1,1) = +1;    I(1,2) = -1;
//    I(2,0) = -1;    I(2,1) = -1;    I(2,2) = +1;
}

// minmax per autoscaling nel rendering
XYf min;
XYf max;

static float invlerp(float lo, float hi, float x)
{
    return (x - lo) / (hi - lo);
}

static void draw_dots(SDL_Renderer * renderer, PARTS * P)
{
    assert(P);
    assert(P->plist);
    SDL_Point xy[P->pcount];
    int A;
    for (A = 0; A < P->pcount; A++) {
        xy[A].x = invlerp(min.x, max.x, P->plist[A].pos.x) * WID;
        xy[A].y = invlerp(min.y, max.y, P->plist[A].pos.y) * HGT;
    }
    SDL_SetRenderDrawColor(renderer, P->rgb.r, P->rgb.g, P->rgb.b, 255);
    SDL_RenderDrawPoints(renderer, xy, COUNT(xy));
}

static void draw_rects(SDL_Renderer * renderer, PARTS * P)
{
    assert(P);
    assert(P->plist);
    SDL_Rect xywh[P->pcount];
    int A;
    for (A = 0; A < P->pcount; A++) {
        xywh[A].x = invlerp(min.x, max.x, P->plist[A].pos.x) * WID - 1;
        xywh[A].y = invlerp(min.y, max.y, P->plist[A].pos.y) * HGT - 1;
        xywh[A].w = 3;
        xywh[A].h = 3;
    }
    SDL_SetRenderDrawColor(renderer, P->rgb.r, P->rgb.g, P->rgb.b, 255);
    SDL_RenderFillRects(renderer, xywh, COUNT(xywh));
}



void parts_draw(SDL_Renderer * renderer)    // HEADER
{
    // minmax
    min.x = min.y = 0;
    max.x = WID;
    max.y = HGT;
    int C;
    for (C = 0; C < COUNT(parts); C++) {
        PARTS *P = &parts[C];
        int A;
        for (A = 0; A < P->pcount; A++) {
            if (min.x > P->plist[A].pos.x)
                min.x = P->plist[A].pos.x;
            if (min.y > P->plist[A].pos.y)
                min.y = P->plist[A].pos.y;
            if (max.x < P->plist[A].pos.x)
                max.x = P->plist[A].pos.x;
            if (max.y < P->plist[A].pos.y)
                max.y = P->plist[A].pos.y;
        }
    }

    // viewport iniziale per riferimento di scala
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_Rect xywh;
    xywh.x = invlerp(min.x, max.x, 0) * WID;
    xywh.y = invlerp(min.y, max.y, 0) * HGT;
    xywh.w = invlerp(min.x, max.x, WID) * WID - xywh.x;
    xywh.h = invlerp(min.y, max.y, HGT) * HGT - xywh.y;
    SDL_RenderDrawRect(renderer, &xywh);

    for (C = 0; C < COUNT(parts); C++) {
        draw_dots(renderer, &parts[C]);
//        draw_rects(renderer,&parts[C]);
    }

}

static float sign(float x)
{
    return x > 0 ? +1 : -1;
}

static float delta(XYf * D, XYf * A, XYf * B)
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
    D->x = B->x - A->x;
    D->y = B->y - A->y;
    return D->x * D->x + D->y * D->y;

//  XYf Bn[3*3] = {
//    { B->x-(WID+20) - A->x, B->y-(HGT+20) - A->y },
//    { B->x          - A->x, B->y-(HGT+20) - A->y },
//    { B->x+(WID+20) - A->x, B->y-(HGT+20) - A->y },
//    { B->x-(WID+20) - A->x, B->y          - A->y },
//    { B->x          - A->x, B->y          - A->y },
//    { B->x+(WID+20) - A->x, B->y          - A->y },
//    { B->x-(WID+20) - A->x, B->y+(HGT+20) - A->y },
//    { B->x          - A->x, B->y+(HGT+20) - A->y },
//    { B->x+(WID+20) - A->x, B->y+(HGT+20) - A->y },
//  };
//
//  D->x = Bn[0].x;
//  D->y = Bn[0].y;
//  float rr = D->x*D->x + D->y*D->y;
//
//  int i;
//  for( i=1; i<COUNT(Bn); i++ ){
//    XYf Dn = { Bn[i].x-A->x, Bn[i].y-A->y };
//    float dd = Dn.x*Dn.x + Dn.y*Dn.y;
//    if( dd >= rr ) continue;
//    *D = Dn;
//    rr = dd;
//  }
//
//  return rr;
}

static void integraz_forza(PARTS * C, PART * A)
{
    // integra le forze su A (della classe C)
    // dovute a tutte le altre parts
    // la dinamica avviene in un secondo momento

    const float MAXF = 1e3;

    // per ora usiamo l'approccio naif O(n²)
    // poi impementerò una griglia di accelerazione
    assert(A);
    assert(C);
    XYf fsum = { 0, 0 };
    int ci;
    for (ci = 0; ci < COUNT(parts); ci++) {
        int bi;
        for (bi = 0; bi < parts[ci].pcount; bi++) {
            PART *B = &parts[ci].plist[bi];
            if (B == A)
                continue;       // auto-interazione? no grazie

            XYf d;
            float rr = delta(&d, &A->pos, &B->pos);     // d²

//      // oltre questa dist, non c'è interaz
//      if(rr>10.0*10.0)continue;

//      // sotto questa dist, non c'è interaz
//      // rende tutto più stabile
//      // forse mettendo una fn continua per abbassare drasticamente 
//      // le forze in caso di distanza bassa ... vediamo
//      if(rr<5.0*5.0)continue; 

//      float f = 1/(rr+EPS);

            // gnuplot -p -e 'plot [0:3] [0:7] (1/x**2)*(1-.01/(.01+x**4)),1/x**2'

            float range = 0.1;
            float f = (1 / (rr + EPS)) * (1 - range / (range + rr * rr));

//      // quasi https://it.wikipedia.org/wiki/Potenziale_di_Lennard-Jones
//      float r6  = rr*rr*rr; // d²*³=d⁶
//      float r12 = rr*rr*rr*rr*rr*rr; // d²*⁶=d¹²
//// evitiamo div0 e comportamenti brutti
//      float f = 1/(r12+EPS)-1/(rr+EPS);
//      if(f>+MAXF) f=+MAXF; else
//      if(f<-MAXF) f=-MAXF;

//      // la particella B è di classe ci
//      // forza semplificata
//      fsum.x += d.x*f*C->interact_matrix[ci];
//      fsum.y += d.y*f*C->interact_matrix[ci];

            // la particella B è di classe ci
            fsum.x += d.x / sqrt(rr) * f * C->interact_matrix[ci];
            fsum.y += d.y / sqrt(rr) * f * C->interact_matrix[ci];
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

//  // drag
//  // modellato come ... boh
//  A->spd.x *= 0.999;
//  A->spd.y *= 0.999;

//  A->spd.x += fsum.x / C->mass;
//  A->spd.y += fsum.y / C->mass;

    A->spd.x += fsum.x;
    A->spd.y += fsum.y;
}


static void integraz_forze()
{
    // per ora usiamo l'approccio naif O(n²)
    // poi impementerò una griglia di accelerazione
    int ci;
    for (ci = 0; ci < COUNT(parts); ci++) {
        int ai;
        for (ai = 0; ai < parts[ci].pcount; ai++) {
            integraz_forza(&parts[ci], &parts[ci].plist[ai]);
        }
    }
}


void parts_update(float dt)     // HEADER
{
    integraz_forze();

    // integraz posizione
    int C;
    for (C = 0; C < COUNT(parts); C++) {
        PARTS *P = &parts[C];
        int A;
        for (A = 0; A < P->pcount; A++) {
            P->plist[A].pos.x += P->plist[A].spd.x * dt;
            P->plist[A].pos.y += P->plist[A].spd.y * dt;

//            // wrap. uso un playfield allargato, tanto il clipping è gratis
//            if( P->plist[A].pos.x <     -10 ) P->plist[A].pos.x += WID+20;  else
//            if( P->plist[A].pos.x >= WID+10 ) P->plist[A].pos.x -= WID+20;
//            if( P->plist[A].pos.y <     -10 ) P->plist[A].pos.y += HGT+20;  else
//            if( P->plist[A].pos.y >= HGT+10 ) P->plist[A].pos.y -= HGT+20;
        }
    }
}

