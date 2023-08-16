#include <mat4.hpp>

// Common operations
Mat4 Mat4::Identity()
{
    Mat4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Mat4 Mat4::RotationX(float angle)
{
    Mat4 matrix;
    matrix.m[0][0] =  1.0f;
    matrix.m[1][1] =  std::cos(angle);
    matrix.m[1][2] =  std::sin(angle);
    matrix.m[2][1] = -std::sin(angle);
    matrix.m[2][2] =  std::cos(angle);
    matrix.m[3][3] =  1.0f;
    return matrix;
}

Mat4 Mat4::RotationY(float angle)
{
    Mat4 matrix;
    matrix.m[0][0] =  std::cos(angle);
    matrix.m[2][0] =  std::sin(angle);
    matrix.m[0][2] = -std::sin(angle);
    matrix.m[1][1] =  1.0f;
    matrix.m[2][2] =  std::cos(angle);
    matrix.m[3][3] =  1.0f;
    return matrix;
}

Mat4 Mat4::RotationZ(float angle)
{
    Mat4 matrix;
    matrix.m[0][0] =  std::cos(angle);
    matrix.m[0][1] =  std::sin(angle);
    matrix.m[1][0] = -std::sin(angle);
    matrix.m[1][1] =  std::cos(angle);
    matrix.m[2][2] =  1.0f;
    matrix.m[3][3] =  1.0f;
    return matrix;
}

Mat4 Mat4::Translation(Vec3 offset)
{
    Mat4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = offset.x;
    matrix.m[3][1] = offset.y;
    matrix.m[3][2] = offset.z;
    return matrix;
}

Mat4 Mat4::Projection(float fov, float near, float far, int width, int height)
{
    float fovRad = 1.0f / std::tan(fov * 0.5f / 180.0f * 3.14159f);
    Mat4 matrix;
    matrix.m[0][0] = (height / width) * fovRad;
    matrix.m[1][1] = fovRad;
    matrix.m[2][2] = far / (far - near);
    matrix.m[3][2] = (-far * near) / (far - near);
    matrix.m[2][3] = -1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

Mat4 Mat4::LookAt(Vec3 origin, Vec3 target, Vec3 up)
{
    // Calculate forward vector
    Vec3 forward = (target - origin).unit();

    // Calculate new up vector
    Vec3 a = forward * up.dot(forward);
    Vec3 newUp = (up - a).unit();

    // Calculate right direction
    Vec3 right = newUp.cross(forward);

    // Dimensioning and translation matrix
    Mat4 matrix;
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

Mat4 Mat4::AxisAngle(Vec3 axis, float angle)
{
    // Normalize axis
    axis = axis.unit();

    // Create matrix
    Mat4 mat;
    mat.m[0][0] = std::cos(angle) + std::pow(axis.x, 2.0f) *    (1 - std::cos(angle));
    mat.m[1][0] = axis.x * axis.y *  (1 - std::cos(angle)) - axis.z * std::sin(angle);
    mat.m[2][0] = axis.x * axis.z *  (1 - std::cos(angle)) + axis.y * std::sin(angle);

    mat.m[0][1] = axis.y * axis.x *  (1 - std::cos(angle)) + axis.z * std::sin(angle);
    mat.m[1][1] = std::cos(angle) + std::pow(axis.y, 2.0f) *    (1 - std::cos(angle));
    mat.m[2][1] = axis.y * axis.z *  (1 - std::cos(angle)) - axis.x * std::sin(angle);

    mat.m[0][2] = axis.z * axis.x *  (1 - std::cos(angle)) - axis.y * std::sin(angle);
    mat.m[1][2] = axis.z * axis.y *  (1 - std::cos(angle)) + axis.x * std::sin(angle);
    mat.m[2][2] = std::cos(angle) + std::pow(axis.z, 2.0f) *    (1 - std::cos(angle));

    mat.m[3][3] = 1.0f;

    return mat;
}

// Used to invert camera lookAt matrix
Mat4 Mat4::QuickInverse()
{
    Mat4 matrix;
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

// Matrix to Vec3 mult
Vec3 Mat4::operator * (const Vec3& v)
{
    Vec3 outV;
    outV.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] +       m[3][0];
    outV.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] +       m[3][1];
    outV.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] +       m[3][2];
    outV.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];

    return outV;
}

// Matrix to Matrix mult
Mat4 Mat4::operator * (const Mat4& other)
{
    Mat4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m[r][0] * other.m[0][c] + m[r][1] * other.m[1][c] + m[r][2] * other.m[2][c] + m[r][3] * other.m[3][c];

    return matrix;
}