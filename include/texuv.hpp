#pragma once

#include <ostream>

class TexUV
{
public:
    TexUV() {}
    TexUV(float _u, float _v, float _w = 1.0f)
        : u(_u),
          v(_v),
          w(_w) {}

    float u = 0;
    float v = 0;
    float w = 1; // Need a 3rd element to work with perspective changes

    // Printing
    friend std::ostream& operator << (std::ostream &os, const TexUV &v) {
        return (os << "(" << v.u << ", " << v.v << ", " << v.w << ")");
    }
};