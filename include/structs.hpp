#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Other structs
#include "texture.hpp"

inline float map_num(float n, float start1, float stop1, float start2, float stop2)
{
    return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

struct TexUV
{
    float u = 0;
    float v = 0;
    float w = 1; // Need a 3rd element to work with perspective changes

    // Printing
    friend std::ostream& operator << (std::ostream &os, const TexUV &v) {
        return (os << "(" << v.u << ", " << v.v << ", " << v.w << ")");
    }
};

struct Vec3D
{
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
        return (os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")");
    }
};

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

struct Triangle
{
    Vec3D p[3];
    TexUV t[3];
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
};

class Mesh
{
public:
    Mesh() {}
    ~Mesh() {}

    std::vector<Triangle> tris;
    Vec3D position = {0.0f, 0.0f, 0.0f};
    Vec3D rotation = {0.0f, 0.0f, 0.0f};
    Vec3D size = {1.0f, 1.0f, 1.0f};
    Texture texture;

    void SetColor(SDL_Color color)
    {
        for (auto& tri : tris)
        {
            tri.color = color;
        }
    }

    bool LoadFromOBJFile(std::string fileName)
    {
        std::ifstream f(fileName);
		if (!f.is_open())
			return false;

        // Check if file has UVs and normals
        bool hasTexture = false;
        bool hasNormals = false;

		// Local cache of verts
		std::vector<Vec3D> verts;
		std::vector<TexUV> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
                    // File has texture
                    hasTexture = true;

					TexUV tex;
					s >> junk >> junk >> tex.u >> tex.v;

                    // Y axis starts at the bottom for UVs, so invert V
                    tex.v = 1.0f - tex.v;
                    tex.w = 1.0f;
					texs.push_back(tex);
				}
                else if (line[1] == 'n')
                {
                    // File has normals
                    hasNormals = true;

                    // Store in vector
                }
				else
				{
					Vec3D vert;
					s >> junk >> vert.x >> vert.y >> vert.z;
					verts.push_back(vert);
				}
			}

            if (line[0] == 'f')
            {
                if (!hasTexture)
                {
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                }
                else
                {
                    s >> junk;

                    std::string tokens[6];
                    int nTokenCount = -1;

                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }
                    tokens[nTokenCount].pop_back();

                    tris.push_back({
                        verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],     // Verts
                        texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1]         // UVs
                    });
                }
            }
		}
		return true;
    }

    void ToCube(Vec3D size = {1.0f, 1.0f, 1.0f})
    {
        tris.clear();

        float halfX = size.x * .5f;
        float halfY = size.y * .5f;
        float halfZ = size.z * .5f;
        tris = {
            // FRONT
            { -halfX, -halfY, -halfZ, 1.0f,    -halfX,  halfY, -halfZ, 1.0f,    halfX,  halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            { -halfX, -halfY, -halfZ, 1.0f,     halfX,  halfY, -halfZ, 1.0f,    halfX, -halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },

            // RIGHT
            {  halfX, -halfY, -halfZ, 1.0f,     halfX,  halfY, -halfZ, 1.0f,    halfX,  halfY,  halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            {  halfX, -halfY, -halfZ, 1.0f,     halfX,  halfY,  halfZ, 1.0f,    halfX, -halfY,  halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },

            // BACK
            {  halfX, -halfY,  halfZ, 1.0f,     halfX,  halfY,  halfZ, 1.0f,   -halfX,  halfY,  halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            {  halfX, -halfY,  halfZ, 1.0f,    -halfX,  halfY,  halfZ, 1.0f,   -halfX, -halfY,  halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },

            // LEFT
            { -halfX, -halfY,  halfZ, 1.0f,    -halfX,  halfY,  halfZ, 1.0f,   -halfX,  halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            { -halfX, -halfY,  halfZ, 1.0f,    -halfX,  halfY, -halfZ, 1.0f,   -halfX, -halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },

            // TOP
            { -halfX,  halfY, -halfZ, 1.0f,    -halfX,  halfY,  halfZ, 1.0f,    halfX,  halfY,  halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            { -halfX,  halfY, -halfZ, 1.0f,     halfX,  halfY,  halfZ, 1.0f,    halfX,  halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },

            // BOTTOM
            {  halfX, -halfY,  halfZ, 1.0f,    -halfX, -halfY,  halfZ, 1.0f,   -halfX, -halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f, 1.0f },
            {  halfX, -halfY,  halfZ, 1.0f,    -halfX, -halfY, -halfZ, 1.0f,    halfX, -halfY, -halfZ, 1.0f,    0.0f, 1.0f, 1.0f,     1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f },
		};
    }

    void ToSphere(float radius, int resolution = 10)
    {
        // Clear tris
        tris.clear();

        // Cache of vertices
        std::vector<Vec3D> vertices;

        // Create vertices
        for (int i = 0; i < resolution; i++)
        {
            float lat = map_num(i, 0, resolution-1, 0, 2 * M_PI);
            for (int j = 0; j < resolution; j++)
            {
                float lon = map_num(j, 0, resolution-1, 0, M_PI);

                Vec3D v;
                v.x = SDL_sinf(lon) * SDL_cosf(lat) * radius * 0.5f;
                v.y = SDL_sinf(lon) * SDL_sinf(lat) * radius * 0.5f;
                v.z = SDL_cosf(lon) * radius * 0.5f;

                vertices.push_back(v);
            }
        }

        // Create faces
        for (int i = 0; i < resolution - 1; i++)
        {
            for (int j = 0; j < resolution - 1; j++)
            {
                // Get indices
                int i0 = i + j * resolution;
                int i1 = (i+1) + j * resolution;
                int i2 = i + (j+1) * resolution;
                int i3 = (i+1) + (j+1) * resolution;

                // Create two triangles
                tris.push_back({
                    vertices[i0],
                    vertices[i1],
                    vertices[i3],
                    TexUV{0.0f, 1.0f, 1.0f},
                    TexUV{0.0f, 0.0f, 1.0f},
                    TexUV{1.0f, 0.0f, 1.0f}
                });
                tris.push_back({
                    vertices[i0],
                    vertices[i3],
                    vertices[i2],
                    TexUV{0.0f, 1.0f, 1.0f},
                    TexUV{1.0f, 0.0f, 1.0f},
                    TexUV{1.0f, 1.0f, 1.0f}
                });
            }
        }
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
    Vec3D pos = {0.0f, 0.0f, 0.0f};
    Vec3D forward = {0.0f, 0.0f, 1.0f};
    Vec3D up = {0.0f, 1.0f, 0.0f};

    float near = .1f, far = 1000.0f;
    float fov = 90.0f;
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

struct MouseState
{
    bool left = false;
    bool right = false;
    bool middle = false;
    bool x1 = false;
    bool x2 = false;
};