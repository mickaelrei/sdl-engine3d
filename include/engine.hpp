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
    bool init(int width = 600, int height = 600);

    // Override methods
    void setup();
    void update(float dt);
    void draw();

    // Method to start engine
    void run();

    // Window info
    // void setWidth(int width) { _width = width; aspectRatio = _height / _width; }
    // void setHeight(int height) { _height = height; aspectRatio = _height / _width; }
    int getWidth() { return _width; }
    int getHeight() { return _height; }

    // Input data
    Vec2D GetMousePos();

    // Drawing
    void Fill(SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void RenderPoint(Vec2D p, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    // Triangle
    void RenderTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void FillTriangleOld(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color = {0, 0, 0, SDL_ALPHA_OPAQUE});
    void TexturedTriangle(Vec2D p0, TexUV tex0, Vec2D p1, TexUV tex1, Vec2D p2, TexUV tex2, Texture texture);
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
    int _width = 0, _height = 0;

    // Depth buffer
    float *depthBuffer = NULL;

    // Keyboard input
    const Uint8 *keyboardState;

    // Mouse state
    void MouseUp(SDL_MouseButtonEvent button);
    void MouseDown(SDL_MouseButtonEvent button);
    MouseState mouseState;
    Vec2D lastMousePos;
    Sint32 scroll;

    // Performance data
    Uint32 delay_ms = 0;
    Uint32 lastTick, nowTick;
    float dt;

    // Camera
    Camera cam;

    // Matrices
    void SetFOV(float fov);
    Mat4x4 matProj;

    // Rendering related
    std::vector<Light> lights;

    // List of scene meshes
    std::vector<Mesh> sceneMeshes;

    //-------------------//
    //      TESTING      //
    //-------------------//

    bool drawWireframe = false;

    // Camera manipulation
    float theta = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
};

// Common vector operations
static Vec3D IntersectPlane(Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd, float &t)
{
    plane_n = plane_n.unit();
    float plane_d = -plane_n.dot(plane_p);
    float ad = lineStart.dot(plane_n);
    float bd = lineEnd.dot(plane_n);
    t = (-plane_d - ad) / (bd - ad);
    Vec3D lineStartToEnd = lineEnd - lineStart;
    Vec3D lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

static int ClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = plane_n.unit();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3D &p)
    {
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3D* inside_points[3];  int nInsidePointCount = 0;
    Vec3D* outside_points[3]; int nOutsidePointCount = 0;
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
