#include <vec2.hpp>

Vec2::Vec2()
{

}

Vec2::Vec2(float _x, float _y)
{
    x = _x;
    y = _y;
}

Vec2::~Vec2()
{

}

// Vec2 to Vec2 operators
Vec2 Vec2::operator + (const Vec2& other)
{
    return { this->x + other.x, this->y + other.y };
}

Vec2 Vec2::operator - (const Vec2& other)
{
    return { this->x - other.x, this->y - other.y };
}

Vec2 Vec2::operator * (const Vec2& other)
{
    return { this->x * other.x, this->y * other.y };
}

Vec2 Vec2::operator / (const Vec2& other)
{
    return { this->x / other.x, this->y / other.y };
}

// Vec2 to number operators
Vec2 Vec2::operator * (const float& f)
{
    return { this->x * f, this->y * f };
}

Vec2 Vec2::operator / (const float& f)
{
    return { this->x / f, this->y / f };
}

// Assignment operators
void Vec2::operator += (const Vec2& other)
{
    this->x += other.x; this->y += other.y;
}

void Vec2::operator -= (const Vec2& other)
{
    this->x -= other.x; this->y -= other.y;
}

void Vec2::operator *= (const Vec2& other)
{
    this->x *= other.x; this->y *= other.y;
}

void Vec2::operator /= (const Vec2& other)
{
    this->x /= other.x; this->y /= other.y;
}

// Methods
float Vec2::magnitude()
{
    return std::sqrt(this->x * this->x + this->y * this->y);
}

Vec2 Vec2::unit()
{
    return *this / this->magnitude();
}

Vec2 Vec2::clamp(const Vec2& _min, const Vec2& _max)
{
    return {
        std::clamp(this->x, _min.x, _max.x),
        std::clamp(this->y, _min.y, _max.y)
    };
}

// Printing
std::ostream& operator << (std::ostream &os, const Vec2 &v)
{
    return (os << "(" << v.x << ", " << v.y << ")");
}