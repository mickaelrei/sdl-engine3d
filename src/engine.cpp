#include <iostream>
#include <list>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <chrono>

#include <engine.hpp>

float degToRad(float deg)
{
    return M_PIf * deg / 180.0f;
}

float radToDeg(float rad)
{
    return rad / M_PIf * 180.0f;
}

// Common vector operations
Vec3 IntersectPlane(Vec3 &plane_p, Vec3 &plane_n, Vec3 &lineStart, Vec3 &lineEnd, float &t)
{
    plane_n = plane_n.unit();
    float plane_d = -plane_n.dot(plane_p);
    float ad = lineStart.dot(plane_n);
    float bd = lineEnd.dot(plane_n);
    t = (-plane_d - ad) / (bd - ad);
    Vec3 lineStartToEnd = lineEnd - lineStart;
    Vec3 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int ClipAgainstPlane(Vec3 plane_p, Vec3 plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = plane_n.unit();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3 &p)
    {
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3* inside_points[3];  int nInsidePointCount = 0;
    Vec3* outside_points[3]; int nOutsidePointCount = 0;
    TexUV* inside_tex[3]; int nInsideTexCount = 0;
    TexUV* outside_tex[3]; int nOutsideTexCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) {
        inside_points[nInsidePointCount++] = &in_tri.p[0];
        inside_tex[nInsideTexCount++] = &in_tri.t[0];
    } else {
        outside_points[nOutsidePointCount++] = &in_tri.p[0];
        outside_tex[nOutsideTexCount++] = &in_tri.t[0];
    }
    if (d1 >= 0) {
        inside_points[nInsidePointCount++] = &in_tri.p[1];
        inside_tex[nInsideTexCount++] = &in_tri.t[1];
    } else {
        outside_points[nOutsidePointCount++] = &in_tri.p[1];
        outside_tex[nOutsideTexCount++] = &in_tri.t[1];
    }
    if (d2 >= 0) {
        inside_points[nInsidePointCount++] = &in_tri.p[2];
        inside_tex[nInsideTexCount++] = &in_tri.t[2];
    } else {
        outside_points[nOutsidePointCount++] = &in_tri.p[2];
        outside_tex[nOutsideTexCount++] = &in_tri.t[2];
    }

    // Now classify triangle points, and break the input triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.color = in_tri.color;

        // The inside point is valid, so keep that...
        out_tri1.p[0] = *inside_points[0];
        out_tri1.t[0] = *inside_tex[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        float t;
        out_tri1.p[1] = IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
        out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
        out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

        out_tri1.p[2] = IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
        out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
        out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.color = in_tri.color;
        out_tri2.color = in_tri.color;

        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];
        out_tri1.t[0] = *inside_tex[0];
        out_tri1.t[1] = *inside_tex[1];

        float t;
        out_tri1.p[2] = IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
        out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
        out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.t[0] = *inside_tex[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.t[1] = out_tri1.t[2];

        out_tri2.p[2] = IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
        out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
        out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
        out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;

        return 2; // Return two newly formed triangles which form a quad
    }

    return 0;
}

// Constructor
Engine3D::Engine3D()
{
    window = NULL;
    renderer = NULL;
    windowSurface = NULL;
}

// Initialize
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

    // Set last mouse pos
    lastMousePos = {.5f * _width, .5f * _height};

    return true;
}

// Add objects
void Engine3D::addMesh(Mesh mesh)
{
    sceneMeshes.push_back(mesh);
}

void Engine3D::addLight(Light light)
{
    lights.push_back(light);
}

// Destructor
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

// Methods to be overriden
void Engine3D::setup()
{
    
}

void Engine3D::update(float dt)
{
    timeElapsed += dt * 1.0f;
}

// Change FOV
void Engine3D::SetFOV(float fov)
{
    cam.fov = fov;
    matProj = Mat4::Projection(cam.fov, cam.near, cam.far, _width, _height);
}

// Input
Vec2 Engine3D::GetMousePos()
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

// Default camera controls
void Engine3D::DefaultCameraMovement(float dt)
{
    Vec3 right = cam.forward.cross(cam.up).unit();
    Vec3 up = right.cross(cam.forward).unit();

     // Moving left, right, front and back
    if (keyboardState[SDL_SCANCODE_A])
        cam.position -= right * cameraMoveSpeed * dt;
    if (keyboardState[SDL_SCANCODE_D])
        cam.position += right * cameraMoveSpeed * dt;
    if (keyboardState[SDL_SCANCODE_W])
        cam.position += cam.forward * cameraMoveSpeed * dt;
    if (keyboardState[SDL_SCANCODE_S])
        cam.position -= cam.forward * cameraMoveSpeed * dt;

    // Moving up and down
    if (keyboardState[SDL_SCANCODE_Q])
        cam.position -= up * cameraMoveSpeed * dt;
    if (keyboardState[SDL_SCANCODE_E])
        cam.position += up * cameraMoveSpeed * dt;
}

void Engine3D::DefaultCameraRotation(float dt)
{
    // Get mouse pos
    Vec2 mousePos = GetMousePos();

    // Get change since last pos
    Vec2 mouseRel = mousePos - lastMousePos;

    // Rotating with mouse
    SDL_SetRelativeMouseMode(mouseState.right ? SDL_TRUE : SDL_FALSE);
    if (mouseState.right)
    {
        // Rotate
        cam.rotate(degToRad(-mouseRel.y * cameraRotationSpeed), degToRad(-mouseRel.x * cameraRotationSpeed));

        // Put mouse on screen center
        SDL_WarpMouseInWindow(window, (int)(_width * .5f), (int)(_height * .5f));
        lastMousePos = {_width * .5f, _height * .5f};
    }
    else lastMousePos = mousePos;
}

void Engine3D::draw()
{
    // Clear screen
    Fill();

    // Clear depth buffer
    for (int i = 0; i < _width * _height; i++)
        depthBuffer[i] = std::numeric_limits<float>::infinity();

    // Loop through every scene mesh
    for (auto& mesh : sceneMeshes)
    {
        // Apply rotation
        Mat4 matRotZ = Mat4::AxisAngle({0.0f, 0.0f, 1.0f}, mesh.rotation.z);
        Mat4 matRotY = Mat4::AxisAngle({0.0f, 1.0f, 0.0f}, mesh.rotation.y);
        Mat4 matRotX = Mat4::AxisAngle({1.0f, 0.0f, 0.0f}, mesh.rotation.x);

        Mat4 matTrans = Mat4::Translation(mesh.position * Vec3(1.0f, 1.0f, 1.0f));
        Mat4 matWorld = Mat4::Identity() * matRotZ * matRotY * matRotX * matTrans;

        // Camera look at matrix
        Mat4 matCamera = Mat4::LookAt(cam.position, cam.position + cam.forward, cam.up);

        // Make view from camera
        Mat4 matView = matCamera.QuickInverse();

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
            Vec3 normal, line1, line2;
            line1 = triTransformed.p[1] - triTransformed.p[0];
            line2 = triTransformed.p[2] - triTransformed.p[0];

            // Cross product
            normal = line1.cross(line2).unit();

            // Get ray from triangle to camera
            Vec3 cameraRay = triTransformed.p[0] - cam.position;

            if (normal.dot(cameraRay) < 0.0f)
            {
                // Calculate color based on illumination
                // Illumination
                Vec3 lightDir = lights[0].direction.unit();

                // Get face luminance
                float d = std::clamp(normal.dot(-lightDir), 0.1f, 1.0f);

                // Set luminance
                HSL hsl;

                // If has texture and is base color
                if (mesh.texture.loaded && mesh.texture.isBaseColor)
                {
                    hsl.FromRGB(mesh.texture.baseColor);
                }
                else
                {
                    hsl.FromRGB(tri.color);
                }

                // Multiply by light brightness
                hsl.L *= d * lights[0].brightness;
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
                    Vec3 offsetView = {1.0f, 1.0f, 0.0f};
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
                        mesh.texture,
                        t.color
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
        // printf("DT: %f\n", dt);

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

// Drawing
void Engine3D::Fill(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void Engine3D::RenderPoint(Vec2 p, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPointF(renderer, p.x, p.y);
}

void Engine3D::RenderTriangle(Vec2 p0, Vec2 p1, Vec2 p2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderDrawLine(renderer, (int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y);
    SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    SDL_RenderDrawLine(renderer, (int)p2.x, (int)p2.y, (int)p0.x, (int)p0.y);
}

/* My attempt (not perfect), reading from:
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
*/
void Engine3D::FillTriangleOld(Vec2 p0, Vec2 p1, Vec2 p2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Create array
    std::array<Vec2, 3> vecList = {p0, p1, p2};

    // Sort by Y
    std::sort(vecList.begin(), vecList.end(), [](const Vec2& first, const Vec2& second) -> bool {
        return first.y < second.y;
    });

    // TOP PART
    int diffY = vecList[1].y - vecList[0].y;

    // Increment on X towards x1
    float incX0 = (vecList[1].x - vecList[0].x) / (float)diffY;

    // Get X coordinate on Y-level of v1 on line from v0 to tex1.v
    float x3 = vecList[0].x + (vecList[1].y - vecList[0].y) / (vecList[2].y - vecList[0].y) * (vecList[2].x - vecList[0].x);

    // Increment on X towards x3
    float incX1 = (x3 - vecList[0].x) / (float)diffY;

    // Scan lines for top part
    for (int i = 0; i <= diffY; i++)
    {
        int y = vecList[0].y + i;
        float x0 = vecList[0].x + incX0 * i;
        float x1 = vecList[0].x + incX1 * i;

        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }

    // BOTTOM PART
    diffY = vecList[2].y - vecList[1].y;

    // Increment on X from v1 towards x2
    incX0 = (vecList[2].x - vecList[1].x) / (float)diffY;

    // Increment on X towards x2
    incX1 = (vecList[2].x - x3) / (float)diffY;

    // Scan lines for bottom part
    for (int i = 0; i <= diffY; i++)
    {
        int y = vecList[1].y + i;
        float x0 = vecList[1].x + incX0 * i;
        float x1 = x3 + incX1 * i;

        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }
}

/* Taken from:
https://github.com/OneLoneCoder/Javidx9/blob/master/ConsoleGameEngine/olcConsoleGameEngine.h
on line 537
*/
void Engine3D::FillTriangle(Vec2 p0, Vec2 p1, Vec2 p2, SDL_Color color)
{
    int x1 = (int)p0.x;
    int x2 = (int)p1.x;
    int x3 = (int)p2.x;
    int y1 = (int)p0.y;
    int y2 = (int)p1.y;
    int y3 = (int)p2.y;
    auto swap = [](int &x, int &y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) RenderPoint({(float)i, (float)ny}, color); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { swap(y1, y2); swap(x1, x2); }
    if (y1>y3) { swap(y1, y3); swap(x1, x3); }
    if (y2>y3) { swap(y2, y3); swap(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        swap(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx>t1x) minx = t1x;
        if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x;
        if (maxx<t2x) maxx = t2x;
        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                        // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
    next:
        // Second half
        dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
        else signx1 = 1;
        dy1 = (int)(y3 - y2);
        t1x = x2;

        if (dy1 > dx1) {   // swap values
            swap(dy1, dx1);
            changed1 = true;
        }
        else changed1 = false;

        e1 = (int)(dx1 >> 1);

        for (int i = 0; i <= dx1; i++) {
            t1xp = 0; t2xp = 0;
            if (t1x<t2x) { minx = t1x; maxx = t2x; }
            else { minx = t2x; maxx = t1x; }
            // process first line until y value is about to change
            while (i<dx1) {
                e1 += dy1;
                while (e1 >= dx1) {
                    e1 -= dx1;
                    if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                    else          goto next3;
                }
                if (changed1) break;
                else   	   	  t1x += signx1;
                if (i<dx1) i++;
            }
        next3:
            // process second line until y value is about to change
            while (t2x != x3) {
                e2 += dy2;
                while (e2 >= dx2) {
                    e2 -= dx2;
                    if (changed2) t2xp = signx2;
                    else          goto next4;
                }
                if (changed2)     break;
                else              t2x += signx2;
            }
        next4:

            if (minx>t1x) minx = t1x;
            if (minx>t2x) minx = t2x;
            if (maxx<t1x) maxx = t1x;
            if (maxx<t2x) maxx = t2x;
            drawline(minx, maxx, y);   										
            if (!changed1) t1x += signx1;
            t1x += t1xp;
            if (!changed2) t2x += signx2;
            t2x += t2xp;
            y += 1;
            if (y>y3) return;
        }
}

void Engine3D::TexturedTriangle(Vec2 p0, TexUV tex0, Vec2 p1, TexUV tex1, Vec2 p2, TexUV tex2, Texture texture, SDL_Color color)
{
    // Convert positions to int
    int x1 = (int)p0.x;
    int x2 = (int)p1.x;
    int x3 = (int)p2.x;
    int y1 = (int)p0.y;
    int y2 = (int)p1.y;
    int y3 = (int)p2.y;

    if (y2 < y1)
    {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(tex0.u, tex1.u);
        std::swap(tex0.v, tex1.v);
        std::swap(tex0.w, tex1.w);
    }

    if (y3 < y1)
    {
        std::swap(y1, y3);
        std::swap(x1, x3);
        std::swap(tex0.u, tex2.u);
        std::swap(tex0.v, tex2.v);
        std::swap(tex0.w, tex2.w);
    }

    if (y3 < y2)
    {
        std::swap(y2, y3);
        std::swap(x2, x3);
        std::swap(tex1.u, tex2.u);
        std::swap(tex1.v, tex2.v);
        std::swap(tex1.w, tex2.w);
    }

    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    float dv1 = tex1.v - tex0.v;
    float du1 = tex1.u - tex0.u;
    float dw1 = tex1.w - tex0.w;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    float dv2 = tex2.v - tex0.v;
    float du2 = tex2.u - tex0.u;
    float dw2 = tex2.w - tex0.w;

    float tex_u, tex_v, tex_w;

    float dax_step = 0, dbx_step = 0,
          du1_step = 0, dv1_step = 0,
          du2_step = 0, dv2_step = 0,
          dw1_step = 0, dw2_step = 0;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    if (dy1) du1_step = du1 / (float)abs(dy1);
    if (dy1) dv1_step = dv1 / (float)abs(dy1);
    if (dy1) dw1_step = dw1 / (float)abs(dy1);

    if (dy2) du2_step = du2 / (float)abs(dy2);
    if (dy2) dv2_step = dv2 / (float)abs(dy2);
    if (dy2) dw2_step = dw2 / (float)abs(dy2);

    if (dy1)
    {
        for (int i = y1; i <= y2; i++)
        {
            int ax = x1 + (float)(i - y1) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;

            if (ax == bx) continue;

            float tex_su = tex0.u + (float)(i - y1) * du1_step;
            float tex_sv = tex0.v + (float)(i - y1) * dv1_step;
            float tex_sw = tex0.w + (float)(i - y1) * dw1_step;

            float tex_eu = tex0.u + (float)(i - y1) * du2_step;
            float tex_ev = tex0.v + (float)(i - y1) * dv2_step;
            float tex_ew = tex0.w + (float)(i - y1) * dw2_step;

            if (ax > bx)
            {
                std::swap(ax, bx);
                std::swap(tex_su, tex_eu);
                std::swap(tex_sv, tex_ev);
                std::swap(tex_sw, tex_ew);
            }

            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                if (tex_w < depthBuffer[i * _width + j])
                {
                    int x = (int)(tex_u / tex_w * texture.width);
                    int y = (int)(tex_v / tex_w * texture.height);

                    SDL_Color pointColor;
                    if (texture.isBaseColor)
                        pointColor = color;
                    else
                        pointColor = texture.GetColorAt(x, y);
                    RenderPoint({(float)j, (float)i}, pointColor);

                    depthBuffer[i * _width + j] = tex_w;
                }
                t += tstep;
            }

        }
    }

    dy1 = y3 - y2;
    dx1 = x3 - x2;
    dv1 = tex2.v - tex1.v;
    du1 = tex2.u - tex1.u;
    dw1 = tex2.w - tex1.w;

    if (dy1) dax_step = dx1 / (float)abs(dy1);
    if (dy2) dbx_step = dx2 / (float)abs(dy2);

    du1_step = 0, dv1_step = 0;
    if (dy1) du1_step = du1 / (float)abs(dy1);
    if (dy1) dv1_step = dv1 / (float)abs(dy1);
    if (dy1) dw1_step = dw1 / (float)abs(dy1);

    if (dy1)
    {
        for (int i = y2; i <= y3; i++)
        {
            int ax = x2 + (float)(i - y2) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;

            if (ax == bx) continue;

            float tex_su = tex1.u + (float)(i - y2) * du1_step;
            float tex_sv = tex1.v + (float)(i - y2) * dv1_step;
            float tex_sw = tex1.w + (float)(i - y2) * dw1_step;

            float tex_eu = tex0.u + (float)(i - y1) * du2_step;
            float tex_ev = tex0.v + (float)(i - y1) * dv2_step;
            float tex_ew = tex0.w + (float)(i - y1) * dw2_step;

            if (ax > bx)
            {
                std::swap(ax, bx);
                std::swap(tex_su, tex_eu);
                std::swap(tex_sv, tex_ev);
                std::swap(tex_sw, tex_ew);
            }

            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                if (tex_w < depthBuffer[i * _width + j])
                {
                    int x = (int)(tex_u / tex_w * texture.width);
                    int y = (int)(tex_v / tex_w * texture.height);

                    SDL_Color pointColor;
                    if (texture.isBaseColor)
                        pointColor = color;
                    else
                        pointColor = texture.GetColorAt(x, y);
                    RenderPoint({(float)j, (float)i}, pointColor);

                    depthBuffer[i * _width + j] = tex_w;
                }
                t += tstep;
            }
        }
    }
}

void Engine3D::RenderRect(Vec2 pos, Vec2 size, int thickness, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect rect{(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
    if (thickness < 0)
        return;
    else if (thickness == 0)
        SDL_RenderFillRect(renderer, &rect);
    else if (thickness == 1)
        SDL_RenderDrawRect(renderer, &rect);
    else
    {
        // Thickness can't be over half the size of the bigger axis
        thickness = std::min({thickness, (int)std::min({size.x, size.y})});

        // Left
        int i;
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x + i, pos.y, pos.x + i, pos.y + size.y);
        }

        // Right
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x + size.x - i, pos.y, pos.x + size.x - i, pos.y + size.y);
        }

        // Top
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x, pos.y + i, pos.x + size.x, pos.y + i);
        }

        // Bottom
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x, pos.y + size.y - i, pos.x + size.x, pos.y + size.y - i);
        }
    }
}