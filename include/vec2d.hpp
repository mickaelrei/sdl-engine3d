#pragma once

#include <algorithm>
#include <SDL2/SDL.h>

struct Vec2D
{
    float x, y = 0;

    // Vec2D to Vec2D operators
    Vec2D operator + (const Vec2D& other) { return { this->x + other.x, this->y + other.y }; }
    Vec2D operator - (const Vec2D& other) { return { this->x - other.x, this->y - other.y }; }
    Vec2D operator * (const Vec2D& other) { return { this->x * other.x, this->y * other.y }; }
    Vec2D operator / (const Vec2D& other) { return { this->x / other.x, this->y / other.y }; }

    // Vec2D to number operators
    Vec2D operator * (const float& f) { return { this->x * f, this->y * f }; }
    Vec2D operator / (const float& f) { return { this->x / f, this->y / f }; }

    // Assignment operators
    void operator += (const Vec2D& other) { this->x += other.x; this->y += other.y; }
    void operator -= (const Vec2D& other) { this->x -= other.x; this->y -= other.y; }
    void operator *= (const Vec2D& other) { this->x *= other.x; this->y *= other.y; }
    void operator /= (const Vec2D& other) { this->x /= other.x; this->y /= other.y; }

    // Comparison operators
    bool operator >  (const Vec2D& other) { return this->x >  other.x && this->y >  other.y; }
    bool operator <  (const Vec2D& other) { return this->x <  other.x && this->x <  other.y; }
    bool operator == (const Vec2D& other) { return this->x == other.x && this->y == other.y; }
    bool operator >= (const Vec2D& other) { return this->x >= other.x && this->y >= other.y; }
    bool operator <= (const Vec2D& other) { return this->x <= other.x && this->y <= other.y; }

    // Methods
    float magnitude() { return std::sqrt(this->x * this->x + this->y * this->y); }
    Vec2D unit() { return *this / this->magnitude(); }
    Vec2D clamp(const Vec2D& _min, const Vec2D& _max)
    {
        return {
            std::clamp(this->x, _min.x, _max.x),
            std::clamp(this->y, _min.y, _max.y)
        };
    }

    // Printing
    friend std::ostream& operator << (std::ostream &os, const Vec2D &v) {
        return (os << "(" << v.x << ", " << v.y << ")");
    }
};
