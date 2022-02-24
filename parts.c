


#include <stdint.h>
#include "main.h"

typedef struct XYf {
  float x,y;
} XYf;

typedef struct RGB8 {
  uint8_t r,g,b;
} RGB8;


typedef struct PART {
  XYf pos;
  XYf spd;
  XYf acc;
} PART;


// penso anche che il drag possa variare con la famiglia
// per che so simulare corpi di dimensioni differenti dispersi nel fluido
// alla stessa maniera la massa, cosi da includere l'inerzia.
// ed anche la dimensione, a questo punto, che potrebbe influenzare
// quando la repulsione coulombiana interviene. quindi:

typedef struct PARTS {
  float mass;   // f=ma & C.
  float drag;   // che si applica a spd²
  float radius;
  RGB8 rgb;
  int pcount;
  PART *plist;
} PARTS;

#define CLASSI 5


// IFNIMPL extern
PARTS parts[CLASSI];


// con N classi, bisogna capire come farle interagire elegantemente
// alla fine, bisogna arrivare alla forza di interazione
// finter(a,b) non è necessariamente = finter(b,a)
// quindi penso ad una matrice NxN. vediamo:



/*


// es famo che:
// classe 0 è attirata dalla 1 ma è respinta dalla 2
// classe 1 è attratta da tutti
// classe 2 è respinta da tutti

// valori negativi attraggono, positivi respingono

{
// classe    0     1      2    3   4
      {    0   , -0.1 , +0.1 , 0 , 0   },  // classe 0
      {   -0.1 ,  0   , +0.1 , 0 , 0   },  // classe 1
      {   +0.2 , -0.1 ,  0   , 0 , 0   },  // classe 2
      {    0   , -0.1 , +0.1 , 0 , 0   },  // classe 3
      {    0   , -0.1 , +0.1 , 0 , 0   },  // classe 4
};


*/

// IFNIMPL extern
float interact_matrix[CLASSI][CLASSI];

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
#include <SDL.h>    // HEADER



void parts_init() // HEADER
{
  int j;
  for( j=0 ; j<CLASSI ; j++ )
  {
    PARTS *P = &parts[j];

    // genera le classi
    P->rgb.r = 128+rand()*127/(RAND_MAX-1);
    P->rgb.g = 128+rand()*127/(RAND_MAX-1);
    P->rgb.b = 128+rand()*127/(RAND_MAX-1);

    P->mass = 1;
    P->drag = 1;
    P->radius = 1;

    // crea le particelle e le posiziona a casaccio
    P->pcount=200;
    int bytes = parts[j].pcount * sizeof(P->plist[0]);
    P->plist = (PART*)malloc(bytes);
    assert(P->plist);
    memset(P->plist,0,bytes);

    int i;
    for( i=0 ; i<P->pcount ; i++ )
    {
//      fprintf(stderr,"%d %f %f\n",i,p.x,p.y);
      P->plist[i].pos.x = (float)rand()*WID/RAND_MAX;
      P->plist[i].pos.y = (float)rand()*HGT/RAND_MAX;
      P->plist[i].spd.x = BIPORAND;
      P->plist[i].spd.y = BIPORAND;
    }

    // genera una matrice di interazione casuale
    memset(interact_matrix,0,sizeof(interact_matrix));
    for( i=0 ; i<CLASSI ; i++ )
    {
      if(i==j)continue;   // interazione con se stessa nulla (non avrebbe molto senso in questo contesto)
      interact_matrix[j][i] = BIPORAND * 0.1;
    }
  }
}




static void draw( SDL_Renderer *renderer , PARTS *P )
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



void parts_draw( SDL_Renderer *renderer ) // HEADER
{
  int C;
  for( C=0 ; C<COUNT(parts) ; C++ )
  {
    draw(renderer,&parts[C]);
  } 
}


static void
integraz_forze()
{
  // integraz forze
  int C;
  for( C=0 ; C<COUNT(parts) ; C++ )
  {
    PARTS *P = &parts[C];
    int A;
    for( A=0 ; A<P->pcount ; A++ )
    {
      // per ora usiamo l'approccio naif O(n²)
      // poi impementerò una griglia di accelerazione
      int B;
      XYf f={0,0};
      for( B=0 ; B<P->pcount ; B++ )
      {
        // calcoliamo f tot sulla particella A dovuta alle altre particelle
        // va applicata una funzione che valuta la forza in base
        // alla matrice di interazione


      }
      // f = ma  → a = f/m
      parts[C].plist[A].acc.x = f.x / parts[C].mass;
      parts[C].plist[A].acc.y = f.y / parts[C].mass;
    }
  }
}





void parts_update( float dt ) // HEADER
{
  // integraz_forze();

  // integraz posizione
  int C;
  for( C=0 ; C<COUNT(parts) ; C++ )
  {
    PARTS *P = &parts[C];
    int A;
    for( A=0 ; A<P->pcount ; A++ )
    {
      P->plist[A].spd.x += P->plist[A].acc.x * dt;
      P->plist[A].spd.y += P->plist[A].acc.y * dt;
      P->plist[A].pos.x += P->plist[A].spd.x * dt;
      P->plist[A].pos.y += P->plist[A].spd.y * dt;
    }
  }

}
