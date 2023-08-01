#include "engine.hpp"

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
    matrix.m[0][0] = (_height / _width) * fFovRad;
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
Vec3D Engine3D::Vector_IntersectPlane(Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd, float &t)
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
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
        out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;

        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
        out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;

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
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
        out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
        out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.t[0] = *inside_tex[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.t[1] = out_tri1.t[2];
        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
        out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
        out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;

        return 2; // Return two newly formed triangles which form a quad
    }

    return 0;
}