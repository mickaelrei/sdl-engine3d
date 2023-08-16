#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include <vec3.hpp>
#include <texture.hpp>
#include <triangle.hpp>

class Mesh
{
public:
    Mesh();
    ~Mesh();

    std::vector<Triangle> tris;
    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 rotation = {0.0f, 0.0f, 0.0f};
    Vec3 size = {1.0f, 1.0f, 1.0f};
    Texture texture;

    // Set color of all triangles
    void SetColor(SDL_Color color);

    // Load from .obj file
    static Mesh FromOBJFile(std::string fileName);

    // Common shapes
    static Mesh Cube();
    static Mesh MinecraftCube();
    static Mesh Sphere(float radius, int resolution = 10);
    static Mesh Cone(float baseRadius, float height, int resolution = 10);
    static Mesh Cylinder(float baseRadius, float height, int resolution = 10);
};
