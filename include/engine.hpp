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

    // Override methods
    void setup();
    void update(float dt);
    void draw();

    // Method to start engine
    void run();

    // Window info
    int getWidth() { return _width; }
    int getHeight() { return _height; }

    // Input data
    Vec2 GetMousePos();

    // Default camera control
    float cameraMoveSpeed = 5.0f;
    float cameraRotationSpeed = .1f;
    void DefaultCameraMovement(float dt);
    void DefaultCameraRotation(float dt);

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

    // Running
    bool running = true;

private:
    // SDL Render data
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* windowSurface;

    // Window data
    int _width = 0, _height = 0;

    // Depth buffer
    float *depthBuffer = NULL;

    // Keyboard input
    const Uint8 *keyboardState;

    // Mouse state
    void MouseUp(SDL_MouseButtonEvent button);
    void MouseDown(SDL_MouseButtonEvent button);
    MouseState mouseState;
    Vec2 lastMousePos;
    Sint32 scroll;

    // Performance data
    Uint32 delay_ms = 0;
    Uint32 lastTick, nowTick;
    float dt;

    // Camera
    Camera cam;

    // Matrices
    void SetFOV(float fov);
    Mat4 matProj;

    // Rendering related
    std::vector<Light> lights;

    // List of scene meshes
    std::vector<Mesh> sceneMeshes;

    bool drawWireframe = false;

    // Time elapsed since engine start
    float timeElapsed = 0.0f;

    //-------------------//
    //      TESTING      //
    //-------------------//

    // Cylinder resolution (quality)
    int resolution = 24;

    // Cylinder radiuses
    float clockRadius = 20.0f;
    float secondArmRadius = 0.5f;
    float minuteArmRadius = 0.8f;
    float hourArmRadius = 1.0f;

    // Cylinder heights
    float secondArmLength = 12.0f;
    float minuteArmLength = 18.0f;
    float hourArmLength = 9.0f;

    // Arm Y position
    float armPosY = 1.0f;

    // Clock floor height
    float clockHeight = 2.0f;

    // Colors
    SDL_Color clockColor = {255, 255, 255, 255};
    SDL_Color secondArmColor = {255, 0, 0, 255};
    SDL_Color minuteArmColor = {50, 50, 50, 255};
    SDL_Color hourArmColor = {50, 50, 50, 255};

    // Hour mark info
    float hourMarkRadius = .2f;
    float hourMarkLength = 3.0f;
    float hourMarkBorderOffset = 1.0f;
    SDL_Color hourMarkColor = {15, 15, 15, 255};

    // Time info
    float secondArmAngle = M_PIf * 0.1f;
    float minuteArmAngle = M_PIf * 1.3f;
    float hourArmAngle = M_PIf * .2f;
};