#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "structs.hpp"

class Engine3D
{
public:
    // Create and destroy
    Engine3D();
    ~Engine3D();
    bool init(int _width = 600, int _height = 600);

    // Override methods
    void setup();
    void update(float dt);
    void draw();

    // Method to start engine
    void start();

    // Input data
    Vec2D GetMousePos();

    // Drawing
    void Fill(SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void RenderPoint(Vec2D p, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Triangle
    void RenderTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangleOld(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Rect
    void RenderRect(Vec2D pos, Vec2D size, int thickness = 0, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Circle
    void RenderCircle(Vec2D pos, int radius, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillCircle(Vec2D pos, int radius, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});

    // Performance related
    void SetFPS(int fps);

    // Window title
    std::string windowTitle = "SDL Engine 3D";

    // Running
    bool running = true;

private:
    // SDL Render data
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* windowSurface;

    // Window data
    int width, height = 0;

    // Keyboard input
    const Uint8 *keyboardState;

    // Performance data
    Uint32 delay_ms = 0;
    Uint32 lastTick, nowTick;
    float dt;

    // Camera
    Camera cam;
    float aspectRatio;

    // Matrices
    Mat4x4 matProj;

    Mat4x4 Matrix_Identity();
    Mat4x4 Matrix_RotationX(float theta);
    Mat4x4 Matrix_RotationY(float theta);
    Mat4x4 Matrix_RotationZ(float theta);
    Mat4x4 Matrix_Translation(Vec3D offset);
    Mat4x4 Matrix_Projection(Camera cam);
    Mat4x4 Matrix_PointAt(Vec3D origin, Vec3D target, Vec3D up);
    Mat4x4 Matrix_QuickInverse(Mat4x4 m); // Only for Rotation/Translation Matrices

    // Vector operations
    Vec3D Vector_IntersectPlane(Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd);
    int Triangle_ClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);

    // Rendering related
    std::vector<Light> lights;
    Vec3D lightDirCorrect = {-1.0f, 1.0f, -1.0f};

    //-------------------//
    //      TESTING      //
    //-------------------//
    std::vector<Mesh> sceneMeshes;


    float theta = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
};