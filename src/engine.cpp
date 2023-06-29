#include <iostream>
#include <array>
#include <list>
#include <algorithm>

#include "engine.hpp"

Engine3D::Engine3D()
{
    window = NULL;
    renderer = NULL;
    windowSurface = NULL;
}

bool Engine3D::init(int _width, int _height)
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
        _width,
        _height,
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

    width = _width;
    height = _height;

    return true;
}

Engine3D::~Engine3D()
{
    if (windowSurface != NULL)
    {
        SDL_FreeSurface(windowSurface);
        windowSurface = NULL;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (window != NULL)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
}

// Matrix operations
Mat4x4 Engine3D::Matrix_Identity()
{
    Mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_RotationX(float theta)
{
    Mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] =  SDL_cosf(theta);
    matrix.m[1][2] =  SDL_sinf(theta);
    matrix.m[2][1] = -SDL_sinf(theta);
    matrix.m[2][2] =  SDL_cosf(theta);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_RotationY(float theta)
{
    Mat4x4 matrix;
    matrix.m[0][0] =  SDL_cosf(theta);
    matrix.m[0][2] =  SDL_sinf(theta);
    matrix.m[2][0] = -SDL_sinf(theta);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] =  SDL_cosf(theta);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_RotationZ(float theta)
{
    Mat4x4 matrix;
    matrix.m[0][0] =  SDL_cosf(theta);
    matrix.m[0][1] =  SDL_sinf(theta);
    matrix.m[1][0] = -SDL_sinf(theta);
    matrix.m[1][1] =  SDL_cosf(theta);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_Translation(Vec3D offset)
{
    Mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = offset.x;
    matrix.m[3][1] = offset.y;
    matrix.m[3][2] = offset.z;
    return matrix;
}

Mat4x4 Engine3D::Matrix_Projection(Camera cam)
{
    float fFovRad = 1.0f / SDL_tanf(cam.fov * 0.5f / 180.0f * 3.14159f);
    Mat4x4 matrix;
    matrix.m[0][0] = aspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = cam.far / (cam.far - cam.near);
    matrix.m[3][2] = (-cam.far * cam.near) / (cam.far - cam.near);
    matrix.m[2][3] = -1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_PointAt(Vec3D origin, Vec3D target, Vec3D up)
{
    // Calculate forward vector
    Vec3D forward = (target - origin).unit();

    // Calculate new up vector
    Vec3D a = forward * up.dot(forward);
    Vec3D newUp = (up - a).unit();

    // Calculate right direction
    Vec3D right = newUp.cross(forward);

    // Dimensioning and translation matrix
    Mat4x4 matrix;
    matrix.m[0][0] = right.x;
    matrix.m[0][1] = right.y;
    matrix.m[0][2] = right.z;
    matrix.m[0][3] = 0.0f;

    matrix.m[1][0] = newUp.x;
    matrix.m[1][1] = newUp.y;
    matrix.m[1][2] = newUp.z;
    matrix.m[1][3] = 0.0f;

    matrix.m[2][0] = forward.x;
    matrix.m[2][1] = forward.y;
    matrix.m[2][2] = forward.z;
    matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = origin.x;
    matrix.m[3][1] = origin.y;
    matrix.m[3][2] = origin.z;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4x4 Engine3D::Matrix_QuickInverse(Mat4x4 m)
{
    Mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;

    matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;

    matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Vec3D operations
Vec3D Engine3D::Vector_IntersectPlane(Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd)
{
    plane_n = plane_n.unit();
    float plane_d = -plane_n.dot(plane_p);
    float ad = lineStart.dot(plane_n);
    float bd = lineEnd.dot(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec3D lineStartToEnd = lineEnd - lineStart;
    Vec3D lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Engine3D::Triangle_ClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = plane_n.unit();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3D &p)
    {
        Vec3D n = p.unit();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3D* inside_points[3];  int nInsidePointCount = 0;
    Vec3D* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

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

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

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
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }

    return 0;
}

// Drawing
void Engine3D::Fill(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void Engine3D::RenderPoint(Vec2D p, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPointF(renderer, p.x, p.y);
}

void Engine3D::RenderTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderDrawLine(renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
    SDL_RenderDrawLine(renderer, (int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
    SDL_RenderDrawLine(renderer, (int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y);
}

/* My attempt (not perfect), reading from:
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
*/
void Engine3D::FillTriangleOld(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Create array
    std::array<Vec2D, 3> vecList = {v0, v1, v2};

    // Sort by Y
    std::sort(vecList.begin(), vecList.end(), [](const Vec2D& first, const Vec2D& second) -> bool {
        return first.y < second.y;
    });

    // TOP PART
    int diffY = vecList[1].y - vecList[0].y;

    // Increment on X towards x1
    float incX0 = (vecList[1].x - vecList[0].x) / (float)diffY;

    // Get X coordinate on Y-level of v1 on line from v0 to v2
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
void Engine3D::FillTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    int x1 = (int)v0.x;
    int x2 = (int)v1.x;
    int x3 = (int)v2.x;
    int y1 = (int)v0.y;
    int y2 = (int)v1.y;
    int y3 = (int)v2.y;
    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) RenderPoint({(float)i, (float)ny}, color); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
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
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
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
        SWAP(dy1, dx1);
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

        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        drawline(minx, maxx, y);   										
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y>y3) return;
    }
}

void Engine3D::RenderRect(Vec2D pos, Vec2D size, int thickness, SDL_Color color)
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

// Input
Vec2D Engine3D::GetMousePos()
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    return {(float)x, (float)y};
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
    // Set camera info
    cam.forward = {0.0f, 0.0f, 1.0f};
    cam.near = .1f;
    cam.far = 1000.0f;
    cam.fov = 90.0f;
    aspectRatio = height / width;

    // Set matrices
    matProj = Matrix_Projection(cam);

    // Load mesh
    if (!meshCube.LoadFromOBJFile("obj3ds/cargo_plane.obj"))
    {
        printf("Error loading object from OBJ file\n");
        running = false;
    }

    // Set colors
    for (auto& tri : meshCube.tris)
    {
        // tri.color = {255, 0, 0, 255};
    }

    // Add light
    Light light;
    light.dir = {0.0f, 0.0f, 1.0f};
    light.brightness = 1.0f;
    lights.push_back(light);

    // Change FPS
    SetFPS(60);
}

void Engine3D::update(float dt)
{
    Vec3D right = cam.forward.cross(cam.up);
    float speed = 15.0f;
    float turnSpeed = 3.0f;
    if (keyboardState[SDL_SCANCODE_UP])
        pitch -= turnSpeed * dt * .25f;
    if (keyboardState[SDL_SCANCODE_DOWN])
        pitch += turnSpeed * dt * .25f;
    if (keyboardState[SDL_SCANCODE_LEFT])
        yaw -= turnSpeed * dt;
    if (keyboardState[SDL_SCANCODE_RIGHT])
        yaw += turnSpeed * dt;
    if (keyboardState[SDL_SCANCODE_A])
        cam.pos -= right * speed * dt;
    if (keyboardState[SDL_SCANCODE_D])
        cam.pos += right * speed * dt;
    if (keyboardState[SDL_SCANCODE_W])
        cam.pos += cam.forward * speed * dt;
    if (keyboardState[SDL_SCANCODE_S])
        cam.pos -= cam.forward * speed * dt;
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        running = false;

    theta += dt;
}

void Engine3D::draw()
{
    Fill();

    float rotZ = 1.0f;
    float rotX = 0.5f;

    // Apply rotation
    Mat4x4 matRotZ = Matrix_RotationZ(theta);
    Mat4x4 matRotX = Matrix_RotationX(theta * .5f);
    Mat4x4 matRotY = Matrix_RotationY(theta * .25f);

    Mat4x4 matTrans = Matrix_Translation({0.0f, 0.0f, 13.0f});
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
    for (auto tri : meshCube.tris)
    {
        Triangle triProjected, triTransformed, triViewed;
        for (int i = 0; i < 3; i++)
            triTransformed.p[i] = matWorld * tri.p[i];

        // Calculate triangle normal
        Vec3D normal, line1, line2;
        line1 = triTransformed.p[1] - triTransformed.p[0];
        line2 = triTransformed.p[2] - triTransformed.p[0];

        // Cross product
        normal = line1.cross(line2).unit();

        // Get ray from triangle to camera
        Vec3D cameraRay = triTransformed.p[0] - cam.pos;

        // if (normal.z < 0)
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
            for (int i = 0; i < 3; i++)
                triViewed.p[i] = matView * triTransformed.p[i];

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
                }

                // Apply position modifiers
                Vec3D offsetView = {1.0f, 1.0f, 0.0f};
                for (int i = 0; i < 3; i++)
                {
                    // Scale into view
                    triProjected.p[i] /= triProjected.p[i].w;

                    // Offset into visible space
                    triProjected.p[i] += offsetView;
                    triProjected.p[i].x *= 0.5f * width;
                    triProjected.p[i].y *= 0.5f * height;
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
                        trisToAdd = Triangle_ClipAgainstPlane({0.0f, (float)height - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
					case 2:
                        trisToAdd = Triangle_ClipAgainstPlane({0.0f, 0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
					case 3:
                        trisToAdd = Triangle_ClipAgainstPlane({(float)width - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
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
            FillTriangle(
                {t.p[0].x, t.p[0].y},
                {t.p[1].x, t.p[1].y},
                {t.p[2].x, t.p[2].y},
                t.color
            );
            // RenderTriangle(
            //     {t.p[0].x, t.p[0].y},
            //     {t.p[1].x, t.p[1].y},
            //     {t.p[2].x, t.p[2].y},
            //     {0, 0, 0, 255}
            // );
        }
    }
}

// Start
void Engine3D::start()
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
        }

        // Update ticks
        lastTick = nowTick;
        nowTick = SDL_GetPerformanceCounter();
        dt = (nowTick - lastTick) / (float)SDL_GetPerformanceFrequency();

        // Call methods
        update(dt);
        draw();

        // Update renderer
        SDL_RenderPresent(renderer);

        // Delay
        SDL_Delay(delay_ms);
    }
}