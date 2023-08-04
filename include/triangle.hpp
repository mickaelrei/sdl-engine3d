#pragma once

#include <SDL2/SDL.h>

#include "vec3d.hpp"
#include "texuv.hpp"

struct Triangle
{
    Vec3D p[3];
    TexUV t[3];
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
};