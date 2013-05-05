#include "vec2.h"

#include <cmath>
#include <sstream>

vec2 vec2::operator+(const vec2 &rhs) const
{
    return vec2(x + rhs.x,
                y + rhs.y);
}

vec2 vec2::operator-(const vec2 &rhs) const
{
    return vec2(x - rhs.x,
                y - rhs.y);
}

vec2 vec2::operator*(double rhs) const
{
    return vec2(x * rhs,
                y * rhs);
}

vec2 vec2::operator/(double rhs) const
{
    return vec2(x / rhs,
                y / rhs);
}

vec2& vec2::operator+=(const vec2 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

vec2& vec2::operator-=(const vec2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

vec2& vec2::operator*=(double rhs)
{
    x *= rhs;
    y *= rhs;
    return *this;
}

vec2& vec2::operator/=(double rhs)
{
    x /= rhs;
    y /= rhs;
    return *this;
}

bool vec2::operator==(const vec2 &rhs) const
{
    return x == rhs.x && y == rhs.y;
}

double vec2::dot(const vec2 &rhs) const
{
    return x * rhs.x + y * rhs.y;
}

double vec2::length() const
{
    return sqrt(x * x + y * y);
}

vec2 vec2::normalise() const
{
    return *this / this->length();
}

std::string vec2::toString()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
}

vec2::vec2(double _x, double _y)
{
    x = _x;
    y = _y;
}
