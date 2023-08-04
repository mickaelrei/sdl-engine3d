#pragma once

#include "vec3d.hpp"
#include "camera.hpp"

class Mat4x4
{
public:
    Mat4x4() {}

    // Common operations
    static Mat4x4 Identity()
    {
        Mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static Mat4x4 RotationX(float theta)
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

    static Mat4x4 RotationY(float theta)
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

    static Mat4x4 RotationZ(float theta)
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

    static Mat4x4 Translation(Vec3D offset)
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

    static Mat4x4 Projection(Camera cam, int width, int height)
    {
        float fFovRad = 1.0f / SDL_tanf(cam.fov * 0.5f / 180.0f * 3.14159f);
        Mat4x4 matrix;
        matrix.m[0][0] = (height / width) * fFovRad;
        matrix.m[1][1] = fFovRad;
        matrix.m[2][2] = cam.far / (cam.far - cam.near);
        matrix.m[3][2] = (-cam.far * cam.near) / (cam.far - cam.near);
        matrix.m[2][3] = -1.0f;
        matrix.m[3][3] = 0.0f;
        return matrix;
    }

    static Mat4x4 PointAt(Vec3D origin, Vec3D target, Vec3D up)
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

    Mat4x4 QuickInverse()
    {
        Mat4x4 matrix;
        matrix.m[0][0] = m[0][0];
        matrix.m[0][1] = m[1][0];
        matrix.m[0][2] = m[2][0];
        matrix.m[0][3] = 0.0f;

        matrix.m[1][0] = m[0][1];
        matrix.m[1][1] = m[1][1];
        matrix.m[1][2] = m[2][1];
        matrix.m[1][3] = 0.0f;

        matrix.m[2][0] = m[0][2];
        matrix.m[2][1] = m[1][2];
        matrix.m[2][2] = m[2][2];
        matrix.m[2][3] = 0.0f;

        matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    // Field
    float m[4][4] = {0};

    // Matrix to Vec3D mult
    Vec3D operator * (const Vec3D& v)
    {
        Vec3D outV;
        outV.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] +       m[3][0];
        outV.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] +       m[3][1];
        outV.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] +       m[3][2];
        outV.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];

        return outV;
    }

    // Matrix to Matrix mult
    Mat4x4 operator * (const Mat4x4& other)
    {
        Mat4x4 matrix;
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                matrix.m[r][c] = m[r][0] * other.m[0][c] + m[r][1] * other.m[1][c] + m[r][2] * other.m[2][c] + m[r][3] * other.m[3][c];

        return matrix;
    }
};
