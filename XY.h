
#ifndef _XY_h_
#define _XY_h_







struct XY {
    
    float x,y;

    XY( float _x=0, float _y=0 ){
        x = _x;
        y = _y;
    }

    XY    operator+( XY    rhs ){ return XY( x+rhs.x, y+rhs.y ); }
    XY    operator-( XY    rhs ){ return XY( x-rhs.x, y-rhs.y ); }
    XY    operator*( float rhs ){ return XY( x*rhs,   y*rhs   ); }
    XY    operator/( float rhs ){ return XY( x/rhs,   y/rhs   ); }
    float dot( XY rhs ){ return x*rhs.x + y*rhs.y; }
    float operator%( XY rhs ){ return dot(rhs); }
};


#define dot %


#endif  //  _XY_h_

