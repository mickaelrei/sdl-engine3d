#pragma once

#include <ostream>

class TexUV
{
public:
    TexUV();
    TexUV(float _u, float _v, float _w = 1.0f);

    float u = 0;
    float v = 0;
    float w = 1; // Need a 3rd element to work with perspective changes
};

std::ostream& operator << (std::ostream &os, const TexUV &v);