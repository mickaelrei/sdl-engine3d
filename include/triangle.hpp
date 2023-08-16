#pragma once

#include <SDL2/SDL.h>

#include <vec3.hpp>
#include <texuv.hpp>

struct Triangle
{
    // 3 vertices (counter-clockwise winding-order)
    Vec3 p[3];

    // 3 UVs
    TexUV t[3];

    // Color, in case of no texture
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
};