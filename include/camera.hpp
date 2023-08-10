#pragma once

#define _USE_MATH_DEFINES
#include "mat4x4.hpp"
#include <cmath>

class Camera
{
public:
    Camera() {}
    Camera(Vec3D pos) { position = pos; }

    // Rotation
    void rotate(float angleX, float angleY, float angleZ = 0.0f)
    {
        // Rotate on X
        Vec3D newForward = Mat4x4::AxisAngle(forward.cross(up).unit(), angleX) * forward;

        // Avoid gimball-lock when looking totally up or totally down
        if (std::abs(Vec3D::angle(newForward, up) - M_PI_2) <= 85.0f / 180.0f * M_PI)
        {
            forward = newForward;
        }

        // Rotate on Y
        forward = Mat4x4::AxisAngle(up, angleY) * forward;

        // Rotate on Z
        up = Mat4x4::AxisAngle(forward, angleZ) * up;
    }

    // Info
    Vec3D position = {0.0f, 0.0f, 0.0f};
    Vec3D forward = {0.0f, 0.0f, -1.0f};
    Vec3D up = {0.0f, 1.0f, 0.0f};

    float near = .1f, far = 1000.0f;
    float fov = 90.0f;
};