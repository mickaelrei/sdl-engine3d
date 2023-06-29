#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

struct Vec3D
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

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
    float dot(const Vec3D& other) { return this->x * other.x + this->y * other.y + this->z * other.z; }
    Vec3D cross(const Vec3D& other) {
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

    // Printing
    friend std::ostream& operator << (std::ostream &os, const Vec3D &v) {
        return (os << "(" << v.x << ", " << v.y << ", " << v.z << ")");
    }
};

struct Vec2D
{
    float x, y = 0;

    Vec2D(const float& _x, const float& _y) { x = _x; y = _y; }
    Vec2D(const Vec2D& v) { x = v.x; y = v.y; }

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

struct Triangle
{
    Vec3D p[3];
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
};

struct Mesh
{
    std::vector<Triangle> tris;

    bool LoadFromOBJFile(std::string fileName)
    {
        std::ifstream f(fileName);
        if (!f.is_open())
            return false;

        // Vertices
        std::vector<Vec3D> vertices;

        // Keep reading until end of file
        while (!f.eof())
        {
            char line[128];
            f.getline(line, 128);

            // Get line
            std::stringstream s;
            s << line;

            // First character
            std::string junk;

            if (line[0] == 'v')
            {
                // Create vertex
                Vec3D v;
                s >> junk >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }

            if (line[0] == 'f')
            {
                // Add face/triangle
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tris.push_back({
                    vertices[f[0] - 1],
                    vertices[f[1] - 1],
                    vertices[f[2] - 1]
                });
            }
        }

        return true;
    }
};

struct Mat4x4
{
    float m[4][4] = {0};

    // Matrix to Vec3D mult
    Vec3D operator * (const Vec3D& v)
    {
        Vec3D outV;
        outV.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] +       m[3][0];
        outV.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] +       m[3][1];
        outV.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] +       m[3][2];
        outV.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];

        return outV;
    }

    // Matrix to Matrix mult
    Mat4x4 operator * (const Mat4x4& other)
    {
        Mat4x4 matrix;
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                matrix.m[r][c] = m[r][0] * other.m[0][c] + m[r][1] * other.m[1][c] + m[r][2] * other.m[2][c] + m[r][3] * other.m[3][c];

        return matrix;
    }
};

struct Camera
{
    Vec3D pos;
    Vec3D forward;
    Vec3D up = {0.0f, 1.0f, 0.0f};

    float near, far;
    float fov;
};

// Color
struct HSL
{
	int H;
	float S;
	float L;

    void FromRGB(SDL_Color color)
    {
        HSL hsl;

        float r = (color.r / 255.0f);
        float g = (color.g / 255.0f);
        float b = (color.b / 255.0f);

        float min = std::min(std::min(r, g), b);
        float max = std::max(std::max(r, g), b);
        float delta = max - min;

        L = (max + min) / 2;

        if (delta == 0)
        {
            H = 0;
            S = 0.0f;
        }
        else
        {
            S = (hsl.L <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));

            float hue;

            if (r == max)
            {
                hue = ((g - b) / 6) / delta;
            }
            else if (g == max)
            {
                hue = (1.0f / 3) + ((b - r) / 6) / delta;
            }
            else
            {
                hue = (2.0f / 3) + ((r - g) / 6) / delta;
            }

            if (hue < 0)
                hue += 1;
            if (hue > 1)
                hue -= 1;

            H = (int)(hue * 360);
        }
    }

    float HueToRGB(float v1, float v2, float vH) {
        if (vH < 0)
            vH += 1;

        if (vH > 1)
            vH -= 1;

        if ((6 * vH) < 1)
            return (v1 + (v2 - v1) * 6 * vH);

        if ((2 * vH) < 1)
            return v2;

        if ((3 * vH) < 2)
            return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

        return v1;
    }

    SDL_Color ToRGB()
    {
        unsigned char r = 0;
        unsigned char g = 0;
        unsigned char b = 0;

        if (S == 0)
        {
            r = g = b = (unsigned char)(L * 255);
        }
        else
        {
            float v1, v2;
            float hue = (float)H / 360;

            v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
            v1 = 2 * L - v2;

            r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
            g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
            b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
        }

        return {r, g, b, SDL_ALPHA_OPAQUE};
    }

    // Printing
    friend std::ostream& operator << (std::ostream &os, const HSL &c) {
        return (os << "(" << c.H << ", " << c.S << ", " << c.L << ")");
    }
};

// Lighting
struct Light
{
    Vec3D dir = {0};
    float brightness = 0.0f;
};