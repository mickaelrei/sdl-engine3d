#include <iostream>
#include <list>
#include <algorithm>

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
    matProj = Matrix_Projection(cam);

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
    // Set last mouse pos
    lastMousePos = GetMousePos();

    // drawWireframe = true;

    // Set camera info
    cam.pos = {0.0f, 0.0f, -3.7f};

    // Load messi
    Mesh mesh;
    mesh.LoadFromOBJFile("assets/obj/roblox_dummy.obj");
    // mesh.texture.init("assets/bmp/messi.bmp");
    sceneMeshes.push_back(mesh);

    // Load cubes
    // float size = 1.0f;
    // Mesh mesh;
    // mesh.ToCube({size, size, size});
    // mesh.size = {size, size, size};
    // mesh.texture.init("assets/bmp/dirt.bmp");
    // for (float i = -10.0f; i < 10.0f; i+=.1f)
    // {
    //     for (int j = 0; j < 1; j++)
    //     {
    //         mesh.position = {i * size, 0.0f, j * size};
    //         sceneMeshes.push_back(mesh);
    //     }
    // }

    // Load mesh
    // Mesh dogMesh;
    // if (!dogMesh.LoadFromOBJFile("assets/obj3ds/jack_russell.obj"))
    // {
    //     printf("Error loading object from OBJ file\n");
    //     running = false;
    //     return;
    // }

    // // Load another mesh
    // Mesh neymarMesh;
    // if (!neymarMesh.LoadFromOBJFile("assets/obj3ds/neymar.obj"))
    // {
    //     printf("Error loading object from OBJ file\n");
    //     running = false;
    //     return;
    // }

    // // Change positions, sizes and rotations
    // dogMesh.position = {0.0f, 3.2f, -0.45f};
    // dogMesh.rotation = {0.0f, M_PI * 1.0f, 0.0f};
    // dogMesh.size = {.2f, .2f, .2f};

    // neymarMesh.position = {0.0f, 0.0f, 0.0f};
    // neymarMesh.rotation = {M_PI * -.5f, M_PI * 1.0f, 0.0f};

    // // Add both meshes
    // sceneMeshes.push_back(dogMesh);
    // sceneMeshes.push_back(neymarMesh);

    // Add light
    Light light;
    light.dir = {0.0f, 0.0f, 1.0f};
    light.brightness = .7f;
    lights.push_back(light);
}

void Engine3D::update(float dt)
{
    Vec3D right = cam.forward.cross(cam.up);
    float speed = 20.1f;
    float turnSpeed = .01f;

    // Rotating with keyboard
    // if (keyboardState[SDL_SCANCODE_UP])
    //     pitch -= turnSpeed * dt * .25f;
    // if (keyboardState[SDL_SCANCODE_DOWN])
    //     pitch += turnSpeed * dt * .25f;
    // if (keyboardState[SDL_SCANCODE_LEFT])
    //     yaw -= turnSpeed * dt;
    // if (keyboardState[SDL_SCANCODE_RIGHT])
    //     yaw += turnSpeed * dt;

    // Get mouse pos
    Vec2D mousePos = GetMousePos();

    // Get change since last pos
    Vec2D mouseRel = mousePos - lastMousePos;
    lastMousePos = mousePos;

    // Rotating with mouse
    SDL_SetRelativeMouseMode(mouseState.right ? SDL_TRUE : SDL_FALSE);
    if (mouseState.right)
        yaw += mouseRel.x * turnSpeed;

    // Moving left, right, front and back
    if (keyboardState[SDL_SCANCODE_A])
        cam.pos -= right * speed * dt;
    if (keyboardState[SDL_SCANCODE_D])
        cam.pos += right * speed * dt;
    if (keyboardState[SDL_SCANCODE_W])
        cam.pos += cam.forward * speed * dt;
    if (keyboardState[SDL_SCANCODE_S])
        cam.pos -= cam.forward * speed * dt;

    // Moving up and down
    if (keyboardState[SDL_SCANCODE_Q])
        cam.pos.y -= speed * dt;
    if (keyboardState[SDL_SCANCODE_E])
        cam.pos.y += speed * dt;

    // Close
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        running = false;
    

    theta += dt * 2.5f;

    // for (int i = 0; i < 10; i++)
    // {
    //     for (int j = 0; j < 10; j++)
    //     {
    //         sceneMeshes[i * 10 + j].position.y = SDL_sinf(theta + j * .3f + i * .3f) * 3.0f;
    //     }
    // }

    for (int i = 0; i < sceneMeshes.size(); i++)
    {
        sceneMeshes[i].rotation.y += dt;
    }
}

void Engine3D::draw()
{
    // printf("Draw\n");
    // Clear screen
    Fill();

    // Clear depth buffer
    for (int i = 0; i < _width * _height; i++)
        depthBuffer[i] = 999999999999999999.9f;

    // Loop through every scene mesh
    for (auto mesh : sceneMeshes)
    {
        // Apply rotation
        Mat4x4 matRotZ = Matrix_RotationZ(mesh.rotation.z);
        Mat4x4 matRotX = Matrix_RotationX(mesh.rotation.x);
        Mat4x4 matRotY = Matrix_RotationY(mesh.rotation.y);

        Mat4x4 matTrans = Matrix_Translation(mesh.position);
        Mat4x4 matWorld = Matrix_Identity() * matRotZ * matRotX * matRotY * matTrans;

        // Camera
        Vec3D camTarget = {0.0f, 0.0f, 1.0f};
        Mat4x4 matCameraRot = Matrix_RotationY(yaw);
        cam.forward = matCameraRot * camTarget;
        camTarget = cam.pos + cam.forward;
        Mat4x4 matCamera = Matrix_PointAt(cam.pos, camTarget, cam.up);

        // Make view from camera
        Mat4x4 matView = Matrix_QuickInverse(matCamera);

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
            Vec3D cameraRay = triTransformed.p[0] - cam.pos;

            if (normal.dot(cameraRay) < 0.0f)
            {
                // Illumination
                Vec3D lightDir = lights[0].dir.unit();

                // Get face luminance
                float d = SDL_clamp(normal.dot(lightDir * lightDirCorrect), 0.1f, 1.0f);

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
                int clippedTriangles = Triangle_ClipAgainstPlane(
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
                    // triProjected.color = clipped[n].color;
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
                            trisToAdd = Triangle_ClipAgainstPlane({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 1:
                            trisToAdd = Triangle_ClipAgainstPlane({0.0f, (float)_height - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 2:
                            trisToAdd = Triangle_ClipAgainstPlane({0.0f, 0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                            break;
                        case 3:
                            trisToAdd = Triangle_ClipAgainstPlane({(float)_width - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
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
    float theta = .01f;
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN)
                MouseDown(e.button);
            else if (e.type == SDL_MOUSEBUTTONUP)
                MouseUp(e.button);
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