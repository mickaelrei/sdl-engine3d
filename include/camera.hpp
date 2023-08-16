#pragma once

#define _USE_MATH_DEFINES
#include <mat4.hpp>
#include <cmath>

class Camera
{
public:
    Camera();
    Camera(Vec3 pos);

    // Rotation
    void rotate(float angleX, float angleY);

    // Orientation info
    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 forward = {0.0f, 0.0f, -1.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};

    // Basic camera info
    float near = .1f, far = 1000.0f;
    float fov = 90.0f;
};