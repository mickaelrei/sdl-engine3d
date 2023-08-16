#pragma once

#include <ostream>
#include <algorithm>
#include <SDL2/SDL.h>

class Vec3
{
public:
    Vec3();
    Vec3(float _x, float _y, float _z, float _w = 1.0f);
    ~Vec3();

    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1; // Need a 4th element to work better with matrix operations

    // Vec3 to Vec3
    Vec3 operator + (const Vec3& other);
    Vec3 operator - (const Vec3& other);
    Vec3 operator * (const Vec3& other);
    Vec3 operator / (const Vec3& other);

    // Vec3 to float
    Vec3 operator * (const float f);
    Vec3 operator / (const float f);

    // Assignment operators
    void operator += (const Vec3& other);
    void operator -= (const Vec3& other);
    void operator *= (const Vec3& other);
    void operator /= (const Vec3& other);
    void operator *= (const float& f);
    void operator /= (const float& f);

    // Negative vector
    Vec3 operator - ();

    // Methods
    float dot(const Vec3& other);
    float magnitude();

    Vec3 cross(const Vec3& other);
    Vec3 unit();
    Vec3 clamp(const Vec3& _min, const Vec3& _max);
    Vec3 lerp(Vec3 &other, const float t);

    static float angle(Vec3 v0, Vec3 v1);
    static float distance(Vec3 v0, Vec3 v1);
};

std::ostream& operator << (std::ostream &os, const Vec3 &v);
