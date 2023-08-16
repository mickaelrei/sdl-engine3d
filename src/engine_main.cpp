#include <iostream>
#include <list>
#include <algorithm>
#include <limits>
#include <cstdlib>

#include <engine.hpp>

float degToRad(float deg)
{
    return M_PI * deg / 180.0f;
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

    // Set last mouse pos
    lastMousePos = {.5f * _width, .5f * _height};

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

// Methods to be overriden
void Engine3D::setup()
{
    // Set camera pos
    cam.position = {-3.0f, 5.0f, 5.0f};
    cam.rotate(-M_PI_4, -M_PI_4);

    // Add cubes to rubiks cube
    float size = 1.0f;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                Mesh mesh = Mesh::MinecraftCube();
                mesh.size = {size, size, size};
                // HSL color = {(float)(rand() % 360), 1.0, .5};
                // mesh.SetColor(color.ToRGB());
                mesh.texture.init("assets/bmp/grass.bmp");
                mesh.position = {i * size, j * size, k * size};

                rubiksCube[i][j][k] = mesh;
                sceneMeshes.push_back(mesh);
            }
        }
    }

    // Add light
    Light light;
    light.direction = {0.0f, 0.0f, -1.0f};
    light.brightness = .7f;
    lights.push_back(light);
}

Vec3 rotatePointAroundAxisX(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newY = point.y * std::cos(angle) - point.z * std::sin(angle);
	float newZ = point.z * std::cos(angle) + point.y * std::sin(angle);

	// Translate back to original place
	return Vec3(point.x, newY, newZ) + pivot;
}

Vec3 rotatePointAroundAxisY(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newX = point.x * std::cos(angle) - point.z * std::sin(angle);
	float newZ = point.z * std::cos(angle) + point.x * std::sin(angle);

	// Translate back to original place
	return Vec3(newX, point.y, newZ) + pivot;
}

Vec3 rotatePointAroundAxisZ(Vec3 point, Vec3 pivot, float angle)
{
    // Translate point into origin
	point -= pivot;

	// Rotate
	float newX = point.x * std::cos(angle) - point.y * std::sin(angle);
	float newY = point.y * std::cos(angle) + point.x * std::sin(angle);

	// Translate back to original place
	return Vec3(newX, newY, point.z) + pivot;
}

int getRubiksCubeIndex(int x, int y, int z)
{
    return x * 9 + y * 3 + z;
}

void Engine3D::update(float dt)
{
    DefaultCameraMovement(dt);
    DefaultCameraRotation(dt);

    // Close
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        running = false;

    // Rubiks cube input logic
    if (keyboardState[SDL_SCANCODE_T] && canMove) {
        canMove = false;
        axis = 0;
        initialRotation = sceneMeshes[getRubiksCubeIndex(0, 0, 0)].rotation.x;
        rotating = 0;
        t = 0.f;
    }
    // if (keyboardState[SDL_SCANCODE_Y] && canMove) {
    //     canMove = false;
    //     axis = 0;
    //     initialRotation = sceneMeshes[getRubiksCubeIndex(1, 0, 0)].rotation.x;
    //     rotating = 1;
    //     t = 0.f;
    // }
    if (keyboardState[SDL_SCANCODE_U] && canMove) {
        canMove = false;
        axis = 0;
        initialRotation = sceneMeshes[getRubiksCubeIndex(2, 0, 0)].rotation.x;
        rotating = 2;
        t = 0.f;
    }

    // Rubiks cube input logic
    if (keyboardState[SDL_SCANCODE_G] && canMove) {
        canMove = false;
        axis = 1;
        initialRotation = sceneMeshes[getRubiksCubeIndex(0, 0, 0)].rotation.y;
        rotating = 0;
        t = 0.f;
    }
    // if (keyboardState[SDL_SCANCODE_H] && canMove) {
    //     canMove = false;
    //     axis = 1;
    //     initialRotation = sceneMeshes[getRubiksCubeIndex(0, 1, 0)].rotation.y;
    //     rotating = 1;
    //     t = 0.f;
    // }
    if (keyboardState[SDL_SCANCODE_J] && canMove) {
        canMove = false;
        axis = 1;
        initialRotation = sceneMeshes[getRubiksCubeIndex(0, 2, 0)].rotation.y;
        rotating = 2;
        t = 0.f;
    }

    // Rubiks cube input logic
    if (keyboardState[SDL_SCANCODE_B] && canMove) {
        canMove = false;
        axis = 2;
        initialRotation = sceneMeshes[getRubiksCubeIndex(0, 0, 0)].rotation.z;
        rotating = 0;
        t = 0.f;
    }
    // if (keyboardState[SDL_SCANCODE_N] && canMove) {
    //     canMove = false;
    //     axis = 2;
    //     initialRotation = sceneMeshes[getRubiksCubeIndex(0, 0, 1)].rotation.z;
    //     rotating = 1;
    //     t = 0.f;
    // }
    if (keyboardState[SDL_SCANCODE_M] && canMove) {
        canMove = false;
        axis = 2;
        initialRotation = sceneMeshes[getRubiksCubeIndex(0, 0, 2)].rotation.z;
        rotating = 2;
        t = 0.f;
    }

    // Animate movement
    if (!canMove) {
        t = std::clamp(t + dt / animDuration, 0.0f, 1.0f);

        int centerIdx;
        switch (axis)
        {
        case 0:
            centerIdx = getRubiksCubeIndex(rotating, 1, 1);
            break;
        case 1: 
            centerIdx = getRubiksCubeIndex(1, rotating, 1);
            break;
        case 2:
            centerIdx = getRubiksCubeIndex(1, 1, rotating);
            break;
        default:
            break;
        }

        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                int cubeIdx;
                switch (axis)
                {
                case 0:
                    cubeIdx = getRubiksCubeIndex(rotating, j, k);
                    break;
                case 1: 
                    cubeIdx = getRubiksCubeIndex(j, rotating, k);
                    break;
                case 2:
                    cubeIdx = getRubiksCubeIndex(j, k, rotating);
                    break;
                default:
                    break;
                }

                // Get initial position before animation
                Vec3 initialPos;
                switch (axis)
                {
                case 0:
                    initialPos = rotatePointAroundAxisX(
                        {(float)rotating, (float)j, (float)k},
                        sceneMeshes[centerIdx].position,
                        initialRotation
                    );
                    break;
                case 1: 
                    initialPos = rotatePointAroundAxisY(
                        {(float)j, (float)rotating, (float)k},
                        sceneMeshes[centerIdx].position,
                        initialRotation
                    );
                    break;
                case 2:
                    initialPos = rotatePointAroundAxisZ(
                        {(float)j, (float)k, (float)rotating},
                        sceneMeshes[centerIdx].position,
                        initialRotation
                    );
                    break;
                default:
                    break;
                }

                // Move
                float angle = M_PI_2 * (1 - std::pow((1 - t), 3.f));

                Vec3 newPos;
                switch (axis)
                {
                case 0:
                    newPos = rotatePointAroundAxisX(
                        initialPos,
                        sceneMeshes[centerIdx].position,
                        angle
                    );
                    break;
                case 1:
                    newPos = rotatePointAroundAxisY(
                        initialPos,
                        sceneMeshes[centerIdx].position,
                        angle
                    );
                    break;
                case 2:
                    newPos = rotatePointAroundAxisZ(
                        initialPos,
                        sceneMeshes[centerIdx].position,
                        angle
                    );
                    break;
                default:
                    break;
                }

                sceneMeshes[cubeIdx].position = newPos;

                // Rotate
                switch (axis)
                {
                case 0:
                    sceneMeshes[cubeIdx].rotation.x = initialRotation + angle;
                    break;
                case 1:
                    sceneMeshes[cubeIdx].rotation.y = initialRotation - angle;
                    break;
                case 2:
                    sceneMeshes[cubeIdx].rotation.z = initialRotation + angle;
                    break;
                }
            }
        }

        if (t == 1.0f) {
            canMove = true;
            axis = -1;
            t = 0.f;
            initialRotation = 0.f;
            rotating = -1;
        }
    }

    // Time
    timeElapsed += dt;

    // Change fov based on scroll
    SetFOV(std::clamp(cam.fov - scroll * 3.f, 5.0f, 160.0f));
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
                // Illumination
                Vec3 lightDir = lights[0].direction.unit();

                // Get face luminance
                float d = std::clamp(normal.dot(-lightDir), 0.1f, 1.0f);

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