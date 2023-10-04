#include <vec3.hpp>
#include <cmath>

Vec3::Vec3()
{

}


Vec3::Vec3(float _x, float _y, float _z, float _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

Vec3::~Vec3()
{

}

// Vec3 to Vec3
Vec3 Vec3::operator + (const Vec3& other)
{
    return { this->x + other.x, this->y + other.y, this->z + other.z };
}
Vec3 Vec3::operator - (const Vec3& other)
{
    return { this->x - other.x, this->y - other.y, this->z - other.z };
}
Vec3 Vec3::operator * (const Vec3& other)
{
    return { this->x * other.x, this->y * other.y, this->z * other.z };
}
Vec3 Vec3::operator / (const Vec3& other)
{
    return { this->x / other.x, this->y / other.y, this->z / other.z };
}

// Vec3 to float
Vec3 Vec3::operator * (const float f)
{
    return { this->x * f, this->y * f, this->z * f };
}
Vec3 Vec3::operator / (const float f)
{
    return { this->x / f, this->y / f, this->z / f };
}

// Assignment operators
void Vec3::operator += (const Vec3& other)
{
    this->x += other.x; this->y += other.y; this->z += other.z;
}
void Vec3::operator -= (const Vec3& other)
{
    this->x -= other.x; this->y -= other.y; this->z -= other.z;
}
void Vec3::operator *= (const Vec3& other)
{
    this->x *= other.x; this->y *= other.y; this->z *= other.z;
}
void Vec3::operator /= (const Vec3& other)
{
    this->x /= other.x; this->y /= other.y; this->z /= other.z;
}
void Vec3::operator *= (const float& f)
{
    this->x *= f; this->y *= f; this->z *= f;
}
void Vec3::operator /= (const float& f)
{
    this->x /= f; this->y /= f; this->z /= f;
}

// Negative vector
Vec3 Vec3::operator - ()
{
    return { -this->x, -this->y, -this->z };
}

// Methods
float Vec3::dot(const Vec3& other)
{
    return this->x * other.x + this->y * other.y + this->z * other.z;
}

float Vec3::magnitude()
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vec3 Vec3::cross(const Vec3& other)
{
    Vec3 v;
    v.x = y * other.z - z * other.y;
    v.y = z * other.x - x * other.z;
    v.z = x * other.y - y * other.x;
    return v;
}

Vec3 Vec3::unit()
{
    return *this / this->magnitude();
}

Vec3 Vec3::clamp(const Vec3& _min, const Vec3& _max)
{
    return {
        std::clamp(this->x, _min.x, _max.x),
        std::clamp(this->y, _min.y, _max.y),
        std::clamp(this->z, _min.z, _max.z)
    };
}

Vec3 Vec3::lerp(Vec3 &other, const float t)
{
    return *this + (other - *this) * t;
}

float Vec3::angle(Vec3 v0, Vec3 v1)
{
    return std::acos(v0.dot(v1));
}

float Vec3::distance(Vec3 v0, Vec3 v1)
{
    return (v0 - v1).magnitude();
}

// Printing
std::ostream& operator << (std::ostream &os, const Vec3 &v)
{
    return (os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")");
}