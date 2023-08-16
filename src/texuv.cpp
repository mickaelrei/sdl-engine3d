#include <texuv.hpp>

TexUV::TexUV()
{

}

TexUV::TexUV(float _u, float _v, float _w)
{
    u = _u;
    v = _v;
    w = _w;
}

std::ostream& operator << (std::ostream &os, const TexUV &v)
{
    return (os << "(" << v.u << ", " << v.v << ", " << v.w << ")");
}