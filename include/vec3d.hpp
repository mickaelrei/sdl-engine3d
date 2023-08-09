#pragma once

#include <SDL2/SDL.h>

class Vec3D
{
public:
    Vec3D() {}
    Vec3D(float _x, float _y, float _z, float _w = 1.0f)
        : x(_x),
          y(_y),
          z(_z),
          w(_w) {}

    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1; // Need a 4th element to work better with matrix operations

    // Vec3D(const float& _x = 0, const float& _y = 0, const float& _z = 0) { x = _x; y = _y; z = _z; }
    // Vec3D(const Vec3D& v) { x = v.x; y = v.y; z = v.z; }

    // Vec3D to Vec3D
    Vec3D operator + (const Vec3D& other) { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
    Vec3D operator - (const Vec3D& other) { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
    Vec3D operator * (const Vec3D& other) { return { this->x * other.x, this->y * other.y, this->z * other.z }; }
    Vec3D operator / (const Vec3D& other) { return { this->x / other.x, this->y / other.y, this->z / other.z }; }

    // Vec3D to float
    Vec3D operator * (const float f) { return { this->x * f, this->y * f, this->z * f }; }
    Vec3D operator / (const float f) { return { this->x / f, this->y / f, this->z / f }; }

    // Assignment operators
    void operator += (const Vec3D& other) { this->x += other.x; this->y += other.y; this->z += other.z; }
    void operator -= (const Vec3D& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; }
    void operator *= (const Vec3D& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; }
    void operator /= (const Vec3D& other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; }
    void operator *= (const float& f) { this->x *= f; this->y *= f; this->z *= f; }
    void operator /= (const float& f) { this->x /= f; this->y /= f; this->z /= f; }

    // Negative vector
    Vec3D operator - () { return { -this->x, -this->y, -this->z }; }

    // Comparison operators
    bool operator >  (const Vec3D& other) { return this->x >  other.x && this->y >  other.y && this->z >  other.z; }
    bool operator <  (const Vec3D& other) { return this->x <  other.x && this->y <  other.y && this->z <  other.z; }
    bool operator == (const Vec3D& other) { return this->x == other.x && this->y == other.y && this->z == other.z; }
    bool operator >= (const Vec3D& other) { return this->x >= other.x && this->y >= other.y && this->z >= other.z; }
    bool operator <= (const Vec3D& other) { return this->x <= other.x && this->y <= other.y && this->z <= other.z; }

    // Methods
    float dot(const Vec3D& other)
    {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }
    Vec3D cross(const Vec3D& other)
    {
        Vec3D v;
		v.x = y * other.z - z * other.y;
		v.y = z * other.x - x * other.z;
		v.z = x * other.y - y * other.x;
		return v;
    }
    float magnitude() { return SDL_sqrtf(this->x * this->x + this->y * this->y + this->z * this->z); }
    Vec3D unit() { return *this / this->magnitude(); }
    Vec3D clamp(const Vec3D& _min, const Vec3D& _max)
    {
        return {
            SDL_clamp(this->x, _min.x, _max.x),
            SDL_clamp(this->y, _min.y, _max.y),
            SDL_clamp(this->z, _min.z, _max.z)
        };
    }
    Vec3D lerp(Vec3D &other, const float t)
    {
        return *this + (other - *this) * t;
    }
    static float angle(Vec3D v0, Vec3D v1)
    {
        return (float)SDL_acos((double)v0.dot(v1));
    }

    // Printing
    friend std::ostream& operator << (std::ostream &os, const Vec3D &v) {
        return (os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")");
    }
};
