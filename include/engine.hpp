#pragma once

#include <SDL2/SDL.h>
#include <string>

#include <structs.hpp>

class Engine3D
{
public:
    // Create and destroy
    Engine3D();
    ~Engine3D();
    bool init(int width = 600, int height = 600);

    // Method to start engine
    void run();

    // Method to add objects in scene
    void addMesh(Mesh mesh);
    void addLight(Light light);

    // Override methods
    virtual void setup();
    virtual void update(float dt);

    // Window info
    int getWidth() { return _width; }
    int getHeight() { return _height; }

    // Mouse
    Vec2 GetMousePos();
    MouseState mouseState;
    Sint32 scroll;

    // Default camera control
    float cameraMoveSpeed = 5.0f;
    float cameraRotationSpeed = .1f;
    void DefaultCameraMovement(float dt);
    void DefaultCameraRotation(float dt);

    // Camera
    Camera cam;
    void SetFOV(float fov);

    // List of scene meshes
    std::vector<Mesh> sceneMeshes;

    // Drawing
    void Fill(SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void RenderPoint(Vec2 p, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Triangle
    void RenderTriangle(Vec2 v0, Vec2 v1, Vec2 v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangle(Vec2 v0, Vec2 v1, Vec2 v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangleOld(Vec2 v0, Vec2 v1, Vec2 v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void TexturedTriangle(Vec2 p0, TexUV tex0, Vec2 p1, TexUV tex1, Vec2 p2, TexUV tex2, Texture texture, SDL_Color color = {0, 0, 0, 0});
    // Rect
    void RenderRect(Vec2 pos, Vec2 size, int thickness = 0, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Circle
    void RenderCircle(Vec2 pos, int radius, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillCircle(Vec2 pos, int radius, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});

    // Performance related
    void SetFPS(int fps);

    // Window title
    std::string windowTitle = "SDL Engine 3D";

    // Keyboard input
    const Uint8 *keyboardState;

    // Running
    bool running = true;

private:
    // Default method for drawing all scene meshes
    void draw();

    // SDL Render data
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* windowSurface;

    // Window data
    int _width = 0, _height = 0;

    // Depth buffer
    float *depthBuffer = NULL;

    // Mouse state
    void MouseUp(SDL_MouseButtonEvent button);
    void MouseDown(SDL_MouseButtonEvent button);
    Vec2 lastMousePos;

    // Performance data
    Uint32 delay_ms = 0;
    Uint32 lastTick, nowTick;
    float dt;

    // Matrices
    Mat4 matProj;

    // Rendering related
    std::vector<Light> lights;

    bool drawWireframe = false;

    // Time elapsed since engine start
    float timeElapsed = 0.0f;
};