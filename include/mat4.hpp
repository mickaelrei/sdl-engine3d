#pragma once

#include <cmath>
#include <vec3.hpp>

class Mat4
{
public:
    Mat4() {}

    // Field
    float m[4][4] = {0};

    // Common operations
    static Mat4 Identity();

    static Mat4 RotationX(float angle);
    static Mat4 RotationY(float angle);
    static Mat4 RotationZ(float angle);

    static Mat4 Translation(Vec3 offset);
    static Mat4 Projection(float fov, float near, float far, int width, int height);
    static Mat4 LookAt(Vec3 origin, Vec3 target, Vec3 up);
    static Mat4 AxisAngle(Vec3 axis, float angle);

    // Used to invert camera lookAt matrix
    Mat4 QuickInverse();

    // Matrix to Vec3 mult
    Vec3 operator * (const Vec3& v);

    // Matrix to Matrix mult
    Mat4 operator * (const Mat4& other);
};
