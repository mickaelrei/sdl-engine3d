#include <iostream>
#include <list>
#include <algorithm>
#include <limits>

#include "engine.hpp"

Engine3D::Engine3D()
{
    window = NULL;
    renderer = NULL;
    windowSurface = NULL;
}

bool Engine3D::init(int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Error initializing video: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );
    if (!window)
    {
        std::cout << "Error creating window: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        std::cout << "Error creating renderer: " << SDL_GetError() << "\n";
        return false;
    }

    windowSurface = SDL_GetWindowSurface(window);
    if (!windowSurface)
    {
        std::cout << "Error getting window surface: " << SDL_GetError() << "\n";
        return false;
    }

    // Set size
    _width = width;
    _height = height;
    depthBuffer = new float[_width * _height];

    // Set matrices
    SetFOV(cam.fov);

    return true;
}

Engine3D::~Engine3D()
{
    if (windowSurface)
    {
        SDL_FreeSurface(windowSurface);
        windowSurface = NULL;
    }

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();
}

void Engine3D::SetFOV(float fov)
{
    cam.fov = fov;
    matProj = Mat4x4::Projection(cam.fov, cam.near, cam.far, _width, _height);
}

// Input
Vec2D Engine3D::GetMousePos()
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    return {(float)x, (float)y};
}

void Engine3D::MouseDown(SDL_MouseButtonEvent button)
{
    switch (button.button)
    {
        case SDL_BUTTON_LEFT:
            mouseState.left = true;
            break;
        case SDL_BUTTON_MIDDLE:
            mouseState.middle = true;
            break;
        case SDL_BUTTON_RIGHT:
            mouseState.right = true;
            break;
        case SDL_BUTTON_X1:
            mouseState.x1 = true;
            break;
        case SDL_BUTTON_X2:
            mouseState.x2 = true;
            break;
    }
}

void Engine3D::MouseUp(SDL_MouseButtonEvent button)
{
    switch (button.button)
    {
        case SDL_BUTTON_LEFT:
            mouseState.left = false;
            break;
        case SDL_BUTTON_MIDDLE:
            mouseState.middle = false;
            break;
        case SDL_BUTTON_RIGHT:
            mouseState.right = false;
            break;
        case SDL_BUTTON_X1:
            mouseState.x1 = false;
            break;
        case SDL_BUTTON_X2:
            mouseState.x2 = false;
            break;
    }
}

// Performance
void Engine3D::SetFPS(int fps)
{
    if (fps < 0)
        delay_ms = 0;
    else
        delay_ms = (Uint32) (1000 / fps);
    printf("Delay (ms): %d\n", delay_ms);
}

// Methods to be overriden
void Engine3D::setup()
{
    // Set camera pos
    cam.position = {0.0f, 0.0f, 6.0f};

    // Set last mouse pos
    lastMousePos = GetMousePos();

    Mesh mesh;
    mesh.size = {.1f, .1f, .1f};
    mesh.LoadFromOBJFile("assets/obj/trex.obj");
    mesh.texture.init("assets/bmp/trex.bmp");
    sceneMeshes.push_back(mesh);

    // Load cubes
    // float size = 1.0f;
    // Mesh mesh;
    // mesh.ToCube({size, size, size});
    // mesh.texture.init("assets/bmp/dirt.bmp");
    // for (int i = -500; i < 500; i++)
    // {
    //     for (int j = 0; j < 1; j++)
    //     {
    //         mesh.position = {i * size, 0.0f, -j * size};
    //         sceneMeshes.push_back(mesh);
    //     }
    // }

    // Add light
    Light light;
    light.direction = {0.0f, 0.0f, -1.0f};
    light.brightness = .7f;
    lights.push_back(light);
}

void Engine3D::update(float dt)
{
    Vec3D right = cam.forward.cross(cam.up).unit();
    float speed = 25.0f;
    float turnSpeed = .01f;

    // Get mouse pos
    Vec2D mousePos = GetMousePos();

    // Get change since last pos
    Vec2D mouseRel = mousePos - lastMousePos;

    // Rotating with mouse
    SDL_SetRelativeMouseMode(mouseState.right ? SDL_TRUE : SDL_FALSE);
    if (mouseState.right)
    {
        // Rotate
        cam.rotate(-mouseRel.y * turnSpeed, mouseRel.x * turnSpeed);

        // Put mouse on screen center
        SDL_WarpMouseInWindow(window, (int)(_width * .5f), (int)(_height * .5f));
        lastMousePos = {_width * .5f, _height * .5f};
    }
    else lastMousePos = mousePos;

    // Moving left, right, front and back
    if (keyboardState[SDL_SCANCODE_A])
        cam.position -= right * speed * dt;
    if (keyboardState[SDL_SCANCODE_D])
        cam.position += right * speed * dt;
    if (keyboardState[SDL_SCANCODE_W])
        cam.position += cam.forward * speed * dt;
    if (keyboardState[SDL_SCANCODE_S])
        cam.position -= cam.forward * speed * dt;

    // Moving up and down
    if (keyboardState[SDL_SCANCODE_Q])
        cam.position.y -= speed * dt;
    if (keyboardState[SDL_SCANCODE_E])
        cam.position.y += speed * dt;

    // Close
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        running = false;

    // Time
    theta += dt * 2.5f;

    // Change fov
    SetFOV(SDL_clamp(cam.fov - scroll * 3.f, 5.0f, 160.0f));

    // Sin wave
    // for (int i = 0; i < sceneMeshes.size(); i++)
    //     sceneMeshes[i].position.y = SDL_sinf(theta + sceneMeshes[i].position.x * .2f) * 3.0f;
}

void Engine3D::draw()
{
    // Clear screen
    Fill();

    // Clear depth buffer
    for (int i = 0; i < _width * _height; i++)
        depthBuffer[i] = std::numeric_limits<float>::infinity();

    float minPitch = -85.0f / 180.0f * M_PI;
    float maxPitch = 85.0f / 180.0f * M_PI;

    // Loop through every scene mesh
    for (auto mesh : sceneMeshes)
    {
        // Apply rotation
        Mat4x4 matRotZ = Mat4x4::AxisAngle({0.0f, 0.0f, 1.0f}, mesh.rotation.z);
        Mat4x4 matRotY = Mat4x4::AxisAngle({0.0f, 1.0f, 0.0f}, mesh.rotation.y);
        Mat4x4 matRotX = Mat4x4::AxisAngle({1.0f, 0.0f, 0.0f}, mesh.rotation.x);

        // Invert Y axis because SDL increases downwards on Y
        Mat4x4 matTrans = Mat4x4::Translation(mesh.position * Vec3D(1.0f, -1.0f, 1.0f));
        Mat4x4 matWorld = Mat4x4::Identity() * matRotZ * matRotY * matRotX * matTrans;

        // Camera look at matrix
        Mat4x4 matCamera = Mat4x4::LookAt(cam.position, cam.position + cam.forward, cam.up);

        // Make view from camera
        Mat4x4 matView = matCamera.QuickInverse();

        // Project triangles
        std::vector<Triangle> trianglesToRaster;
        for (auto& tri : mesh.tris)
        {
            // Scale points
            Triangle scaledTri;
            scaledTri.p[0] = tri.p[0] * mesh.size;
            scaledTri.p[1] = tri.p[1] * mesh.size;
            scaledTri.p[2] = tri.p[2] * mesh.size;

            Triangle triProjected, triTransformed, triViewed;
            for (int i = 0; i < 3; i++)
            {
                triTransformed.p[i] = matWorld * scaledTri.p[i];
                triTransformed.t[i] = tri.t[i];
            }

            // Calculate triangle normal
            Vec3D normal, line1, line2;
            line1 = triTransformed.p[1] - triTransformed.p[0];
            line2 = triTransformed.p[2] - triTransformed.p[0];

            // Cross product
            normal = line1.cross(line2).unit();

            // Get ray from triangle to camera
            Vec3D cameraRay = triTransformed.p[0] - cam.position;

            if (normal.dot(cameraRay) < 0.0f)
            {
                // Illumination
                Vec3D lightDir = lights[0].direction.unit();

                // Get face luminance
                float d = SDL_clamp(normal.dot(-lightDir), 0.1f, 1.0f);

                // Set luminance
                HSL hsl;
                hsl.FromRGB(tri.color);
                hsl.L = d * lights[0].brightness;
                triProjected.color = hsl.ToRGB();

                // Convert from World Space to View Space
                for (int i = 0; i < 3; i++) {
                    triViewed.p[i] = matView * triTransformed.p[i];
                    triViewed.t[i] = triTransformed.t[i];
                }

                // Clip viewed triangle against near plane, this could
                // form two additional triangles
                Triangle clipped[2];
                int clippedTriangles = ClipAgainstPlane(
                    {0.0f, 0.0f, 0.1f},
                    {0.0f, 0.0f, 1.0f},
                    triViewed,
                    clipped[0],
                    clipped[1]
                );

                for (int n = 0; n < clippedTriangles; n++)
                {
                    // Project from 3D to 2D
                    for (int i = 0; i < 3; i++)
                    {
                        triProjected.p[i] = matProj * clipped[n].p[i];
                        triProjected.t[i] = clipped[n].t[i];
                    }

                    // Project texture
                    if (mesh.texture.loaded)
                    {
                        triProjected.t[0].u /= triProjected.p[0].w;
                        triProjected.t[1].u /= triProjected.p[1].w;
                        triProjected.t[2].u /= triProjected.p[2].w;

                        triProjected.t[0].v /= triProjected.p[0].w;
                        triProjected.t[1].v /= triProjected.p[1].w;
                        triProjected.t[2].v /= triProjected.p[2].w;

                        triProjected.t[0].w = 1.0f / triProjected.p[0].w;
                        triProjected.t[1].w = 1.0f / triProjected.p[1].w;
                        triProjected.t[2].w = 1.0f / triProjected.p[2].w;
                    }

                    // Apply position modifiers
                    Vec3D offsetView = {1.0f, 1.0f, 0.0f};
                    for (int i = 0; i < 3; i++)
                    {
                        // Scale into view
                        triProjected.p[i] /= triProjected.p[i].w;

                        // Offset into visible space
                        triProjected.p[i] += offsetView;
                        triProjected.p[i].x *= 0.5f * _width;
                        triProjected.p[i].y *= 0.5f * _height;
                    }

                    // Store triangle for sorting
                    trianglesToRaster.push_back(triProjected);
                }
            }
        }

        // Sort triangles
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](Triangle& t1, Triangle& t2) {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;

            return z1 < z2;
        });

        // Clipping
        for (Triangle& triToRaster : trianglesToRaster)
        {
            // Clip triangles against all four screen edges, this could
            // yield a bunch of triangles
            Triangle clipped[2];
            std::list<Triangle> listTriangles;
            listTriangles.push_back(triToRaster);
            int newTriangles = 1;

            for (int p = 0; p < 4; p++)
            {
                int trisToAdd = 0;
                while (newTriangles > 0)
                {
                    // Take triangle from front of queue
                    Triangle test = listTriangles.front();
                    listTriangles.pop_front();
                    newTriangles--;

                    // Clip it against a plane
                    switch (p)
                        {
                        case 0:
                            trisToAdd = ClipAgainstPlane({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 1:
                            trisToAdd = ClipAgainstPlane({0.0f, (float)_height - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 2:
                            trisToAdd = ClipAgainstPlane({0.0f, 0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 3:
                            trisToAdd = ClipAgainstPlane({(float)_width - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        }

                    // Clipping may yield a variable number of triangles, so
                    // add these new ones to the back of the queue for subsequent
                    // clipping against next planes
                    for (int w = 0; w < trisToAdd; w++)
                        listTriangles.push_back(clipped[w]);
                }
                newTriangles = listTriangles.size();
            }

            // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
            for (auto &t : listTriangles)
            {
                if (mesh.texture.loaded)
                {
                    TexturedTriangle(
                        {t.p[0].x, t.p[0].y},
                        t.t[0],
                        {t.p[1].x, t.p[1].y},
                        t.t[1],
                        {t.p[2].x, t.p[2].y},
                        t.t[2],
                        mesh.texture
                    );
                } else {
                    FillTriangle(
                        {t.p[0].x, t.p[0].y},
                        {t.p[1].x, t.p[1].y},
                        {t.p[2].x, t.p[2].y},
                        t.color
                    );
                }

                if (drawWireframe)
                    RenderTriangle(
                        {t.p[0].x, t.p[0].y},
                        {t.p[1].x, t.p[1].y},
                        {t.p[2].x, t.p[2].y},
                        {255, 255, 255, 255}
                    );
            }
        }
    }
}

// Main loop
void Engine3D::run()
{
    // Initialize keyboard state
    keyboardState = SDL_GetKeyboardState(NULL);

    // Call setup once
    setup();

    // Set variables
    nowTick = SDL_GetPerformanceCounter();
    lastTick = 0;

    // Main loop
    SDL_Event e;
    while (running)
    {
        // Reset some variables
        scroll = 0;

        while (SDL_PollEvent(&e))
        {
            // Closing window
            if (e.type == SDL_QUIT)
                running = false;

            // Mouse buttons
            else if (e.type == SDL_MOUSEBUTTONDOWN)
                MouseDown(e.button);
            else if (e.type == SDL_MOUSEBUTTONUP)
                MouseUp(e.button);

            // Mouse scroll
            if (e.type == SDL_MOUSEWHEEL)
                scroll = e.wheel.y;
        }

        // Update ticks
        lastTick = nowTick;
        nowTick = SDL_GetPerformanceCounter();
        dt = (nowTick - lastTick) / (float)SDL_GetPerformanceFrequency();

        // Call methods
        update(dt);
        draw();

        // Set title
        std::stringstream title;
        title << "SDL Engine 3D | " << (int)(1.0f / dt) << " fps";
        SDL_SetWindowTitle(window, title.str().c_str());

        // Update renderer
        SDL_RenderPresent(renderer);

        // Delay
        SDL_Delay(delay_ms);
    }
}