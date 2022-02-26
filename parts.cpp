
#include "macros.h"
#include "main.h"
#include "V2f.h"
#include <SDL.h>                // HEADER
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
//#include <unordered_map>
//#include <unordered_set>
#include <algorithm>

#undef NDEBUG
#include <assert.h>



typedef struct RGB8 {
    uint8_t r, g, b;
} RGB8;

typedef struct PART {
    V2f pos;
    V2f spd;
    float mass;
    float radius;
} PART;

typedef std::vector < PART > PARTS;

PARTS parts;

// minmax per autoscaling nel rendering
V2f min;
V2f max;

const float EPS = 1e-5;






void update_radius( PART &p ){
    p.radius = 10 * cbrtf(p.mass) / 2;
}


void parts_init()               // HEADER
{
    const RGB8 rgb[] = {
        {255, 128, 63},         // orange
        {255, 255, 255},        // white
        {255, 255, 63},         // yel
        {255, 63, 255},         // magen
        {255, 63, 63},          // red
        {63, 255, 255},         // cyan
        {63, 255, 63},          // green
        {63, 63, 255},          // blu
        {63, 63, 63},           // gray
    };

    srand(time(0));

    parts.clear();
    for ( int i = 0; i < 1000; i++) {
        PART p;
        p.pos.X( BIPORAND * 100 + WID / 2 );
        p.pos.Y( BIPORAND * 100 + HGT / 2 );
        p.spd.X( BIPORAND * 10 );
        p.spd.Y( BIPORAND * 10 );
        p.mass = 100 + 10 * UNIRAND;
        update_radius(p);
        if(rand()&1) p.mass = -p.mass;
        parts.push_back(p);
    }
}




static float invlerp(float lo, float hi, float x)
{
    return (x - lo) / (hi - lo);
}




static void draw_dots( SDL_Renderer * renderer )
{
    std::vector<SDL_Point> pos;
    std::vector<SDL_Point> neg;
    for (auto &p:parts) {
        SDL_Point xy;
        xy.x = invlerp(min.X(), max.X(), p.pos.X()) * WID;
        xy.y = invlerp(min.Y(), max.Y(), p.pos.Y()) * HGT;
        if(p.mass>0) pos.push_back(xy);
        else         neg.push_back(xy);
    }

    SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
    SDL_RenderDrawPoints(renderer, neg.data(), neg.size());
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoints(renderer, pos.data(), pos.size());
}







static void draw_rects(SDL_Renderer * renderer )
{
    SDL_Rect xywh [ parts.size()];
    int A = 0;
    for (auto &p:parts) {
        xywh[A].x = invlerp(min.X(), max.X(), p.pos.X()) * WID - 1;
        xywh[A].y = invlerp(min.Y(), max.Y(), p.pos.Y()) * HGT - 1;
        xywh[A].w = 3;
        xywh[A].h = 3;
        A++;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRects(renderer, xywh, COUNT(xywh));
}





void parts_draw(SDL_Renderer * renderer)    // HEADER
{
    // minmax
    min = V2f::ZERO;
    max.X( WID );
    max.Y( HGT );

    static float aspect = HGT*1.0/WID;

//    for (auto &p:parts) {
//        min.X( std::min(min.X(), p.pos.X()));
//        min.Y( std::min(min.Y(), p.pos.Y()));
//        max.X( std::max(max.X(), p.pos.X()));
//        max.Y( std::max(max.Y(), p.pos.Y()));
//    }

    // viewport iniziale per riferimento di scala
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_Rect xywh;
    xywh.x = invlerp(min.X(), max.X(), 0) * WID;
    xywh.y = invlerp(min.Y(), max.Y(), 0) * HGT;
    xywh.w = invlerp(min.X(), max.X(), WID) * WID - xywh.x;
    xywh.h = invlerp(min.Y(), max.Y(), HGT) * HGT - xywh.y;
    SDL_RenderDrawRect(renderer, &xywh);

    draw_dots(renderer);
//    draw_rects(renderer);
}




static float sign(float x)
{
    return x > 0 ? +1 : -1;
}




static void
update_pair( PART &A, PART &B ){

    V2f ab = B.pos - A.pos;
    float rr = ab.dot(ab);
    V2f dir = ab*(1/sqrt(rr));

//    // gnuplot -p -e 'plot [0:3] [0:7] (1/x**2)*(1-.01/(.01+x**4)),1/x**2'
//    float f = (1 / (rr + EPS)) * (1 - range / (range + rr * rr));
    float f = A.mass*B.mass/(rr+EPS);

    // f=ma → a=f/m
    A.spd = A.spd + dir*(f/A.mass);
    B.spd = B.spd - dir*(f/B.mass);

    // NOTA:
    // ci serve la spd ora perche le fusioni
    // devono poter lavorare sul momento lineare
    // quindi occorrono massa e velocità
}



static void integraz()
{
    // per ora usiamo l'approccio naif O(n²)
    // poi implementerò un qualche metodo di accelerazione

    // (n²-n)/2 paia rilevanti:
    //
    //     ··· i ···
    //     0 1 2 3 4
    // : 0 · x x x x
    // · 1 · · x x x
    // j 2 · · · x x
    // · 3 · · · · x 
    // : 4 · · · · ·

    for( int j=0; j<parts.size(); j++ ){
        for( int i=0; i<parts.size(); i++ ){
            if( i <= j )continue;
            update_pair( parts[i], parts[j]);
        }
    }
}




void parts_update(float dt)     // HEADER
{
    integraz();

//    merge();

    dt *= 1e-5;

    for( auto &p: parts ){
        p.pos = p.pos + p.spd * dt;
    }
}


