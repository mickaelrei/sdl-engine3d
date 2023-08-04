#pragma once

#include "vec3d.hpp"

struct Camera
{
    Vec3D position = {0.0f, 0.0f, 0.0f};
    Vec3D forward = {0.0f, 0.0f, 1.0f};
    Vec3D up = {0.0f, 1.0f, 0.0f};

    float near = .1f, far = 1000.0f;
    float fov = 90.0f;
};