#include <camera.hpp>

Camera::Camera()
{
    
}

Camera::Camera(Vec3 pos)
{
    position = pos;
}

// Rotation
void Camera::rotate(float angleX, float angleY)
{
    // Rotate on X
    Vec3 newForward = Mat4::AxisAngle(forward.cross(up).unit(), angleX) * forward;

    // Avoid gimball-lock when looking totally up or totally down
    if (std::abs(Vec3::angle(newForward, up) - M_PI_2) <= 85.0f / 180.0f * M_PI)
    {
        forward = newForward;
    }

    // Rotate on Y
    forward = Mat4::AxisAngle(up, angleY) * forward;
}