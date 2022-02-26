
//#include <string.h>
#include <math.h>    // HEADER

// HEADERBEG





struct V2f {
    float v[2];

    static const V2f ZERO;
    static const V2f ONE;

     V2f(float x_ = 0, float y_ = 0) {
        v[0] = x_;
        v[1] = y_;
    }
    
    // accessors per leggibilità (-O3 dovrebbe metterli inline) 
    
    float X() const {
        return v[0];
    }

    float Y() const {
        return v[1];
    }

    float X( float _ ){
        return v[0]=_;
    }

    float Y( float _ ){
        return v[1]=_;
    }

    V2f operator-() const {
        return V2f(-v[0], -v[1]);
    }

    V2f operator-(const V2f & b) const {
        return V2f(v[0] - b.v[0], v[1] - b.v[1]);
    }

    V2f operator+(const V2f & b) const {
        return V2f(v[0] + b.v[0], v[1] + b.v[1]);
    }

    float dot(const V2f & b) const {
        return v[0] * b.v[0] + v[1] * b.v[1];
    }

    V2f scalar(const float b) const {
        return V2f(v[0] * b, v[1] * b);
    }

    V2f operator*(const float b) const {
        return scalar(b);
    }

    V2f pointwise(const V2f & b) const {
        return V2f(v[0] * b.v[0], v[1] * b.v[1]);
    }

    V2f hadamard(const V2f & b) const {
        return pointwise(b);
    }

    V2f operator*(const V2f & b) const {
        return pointwise(b);
    }

    bool is_zero() const {
        return (v[0] == 0.0) & (v[1] == 0.0);
    }

    V2f clamped(const V2f & lo, const V2f & hi) {
        V2f c;
        c.v[0] = (v[0] < lo.v[0] ? lo.v[0] : (v[0] > hi.v[0] ? hi.v[0] : v[0]));
        c.v[1] = (v[1] < lo.v[1] ? lo.v[1] : (v[1] > hi.v[1] ? hi.v[1] : v[1]));
        return c;
    }

    V2f normalized() {
        /* Zero vectors, and vectors of near zero magnitude, produce zero length,
           and (since 1 / 0 is conditioned to 0) ultimately a zero vector result.
           Vectors of extremely large magnitude produce +infinity length, and (since
           1 / inf is 0) ultimately a zero vector result.
           (Perhaps zero vectors should produce infinite results, but pragmatically,
           zeros are probably easier to handle than infinities.) */
        const float length = sqrt(this->dot(*this));
        const float invlen = (length != 0.0 ? 1.0 / length : 0.0);
        return *this * invlen;
    }

    V2f norm() {
        return normalized();
    }
};

typedef struct V2f V2f;

// HEADEREND

const V2f V2f::ZERO(0.0, 0.0);
const V2f V2f::ONE(1.0, 1.0);
