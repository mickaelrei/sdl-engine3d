#pragma once

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
    // float dot(const Vec2D& other) { };
    // Vec2D cross(const Vec2D& other) { };
    float magnitude() { return SDL_sqrtf(this->x * this->x + this->y * this->y); }
    Vec2D unit() { return *this / this->magnitude(); }
    Vec2D clamp(const Vec2D& _min, const Vec2D& _max)
    {
        return {
            SDL_clamp(this->x, _min.x, _max.x),
            SDL_clamp(this->y, _min.y, _max.y)
        };
    }

    // Printing
    friend std::ostream& operator << (std::ostream &os, const Vec2D &v) {
        return (os << "(" << v.x << ", " << v.y << ")");
    }
};
