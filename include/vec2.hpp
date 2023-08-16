#pragma once

#include <algorithm>
#include <ostream>
#include <SDL2/SDL.h>

class Vec2
{
public:
    Vec2();
    Vec2(float _x, float _y);
    ~Vec2();

    float x, y = 0;

    // Vec2 to Vec2 operators
    Vec2 operator + (const Vec2& other);
    Vec2 operator - (const Vec2& other);
    Vec2 operator * (const Vec2& other);
    Vec2 operator / (const Vec2& other);

    // Vec2 to number operators
    Vec2 operator * (const float& f);
    Vec2 operator / (const float& f);

    // Assignment operators
    void operator += (const Vec2& other);
    void operator -= (const Vec2& other);
    void operator *= (const Vec2& other);
    void operator /= (const Vec2& other);

    // Methods
    float magnitude();
    Vec2 unit();
    Vec2 clamp(const Vec2& _min, const Vec2& _max);
};

std::ostream& operator << (std::ostream &os, const Vec2 &v);
