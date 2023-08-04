#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Other structs
#include "camera.hpp"
#include "hsl.hpp"
#include "mat4x4.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "texuv.hpp"
#include "triangle.hpp"
#include "vec2d.hpp"
#include "vec3d.hpp"

// Lighting
struct Light
{
    Vec3D direction = {0.0f, 0.0f, 0.0f};
    float brightness = 0.0f;
};

struct MouseState
{
    bool left = false;
    bool right = false;
    bool middle = false;
    bool x1 = false;
    bool x2 = false;
};