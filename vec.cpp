#include "vec.h"

#include <cmath>
#include <sstream>

// V     V  EEEEEEE    CCC      222    FFFFFFF
// V     V  E         CC CC    2   22  F
// V     V  E        CC    C        2  F
//  V   V   E        C             2   F
//  V   V   EEEE     C            2    FFFF
//  V   V   E        C           2     F
//   V V    E        CC    C    2      F
//   VVV    E         CC CC    2       F
//    V     EEEEEEE    CCC    2222222  F

vec2f vec2f::operator+(const vec2f &rhs) const
{
    return vec2f(x + rhs.x,
                y + rhs.y);
}

vec2f vec2f::operator-(const vec2f &rhs) const
{
    return vec2f(x - rhs.x,
                y - rhs.y);
}

vec2f vec2f::operator*(float rhs) const
{
    return vec2f(x * rhs,
                y * rhs);
}

vec2f vec2f::operator/(float rhs) const
{
    return vec2f(x / rhs,
                y / rhs);
}

vec2f& vec2f::operator+=(const vec2f &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

vec2f& vec2f::operator-=(const vec2f &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

vec2f& vec2f::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    return *this;
}

vec2f& vec2f::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    return *this;
}

bool vec2f::operator==(const vec2f &rhs) const
{
    return x == rhs.x && y == rhs.y;
}

float vec2f::dot(const vec2f &rhs) const
{
    return x * rhs.x + y * rhs.y;
}

float vec2f::length() const
{
    return sqrt(x * x + y * y);
}

vec2f vec2f::normalise() const
{
    return *this / this->length();
}

std::string vec2f::toString()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
}

vec2f::vec2f(float _x, float _y)
{
    x = _x;
    y = _y;
}


// V     V  EEEEEEE    CCC      333    FFFFFFF
// V     V  E         CC CC   33   33  F
// V     V  E        CC    C  3     3  F
//  V   V   E        C             33  F
//  V   V   EEEE     C           33    FFFF
//  V   V   E        C             33  F
//   V V    E        CC    C  3     3  F
//   VVV    E         CC CC   33   33  F
//    V     EEEEEEE    CCC      333    F



vec3f vec3f::operator+(const vec3f &rhs) const
{
    return vec3f(x + rhs.x,
                 y + rhs.y,
                 z + rhs.z);
}

vec3f vec3f::operator-(const vec3f &rhs) const
{
    return vec3f(x - rhs.x,
                 y - rhs.y,
                 z - rhs.z);
}

vec3f vec3f::operator*(float rhs) const
{
    return vec3f(x * rhs,
                 y * rhs,
                 z * rhs);
}

vec3f vec3f::operator/(float rhs) const
{
    return vec3f(x / rhs,
                 y / rhs,
                 z / rhs);
}

vec3f& vec3f::operator+=(const vec3f &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

vec3f& vec3f::operator-=(const vec3f &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

vec3f& vec3f::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

vec3f& vec3f::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    y /= rhs;
    return *this;
}

bool vec3f::operator==(const vec3f &rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

float vec3f::dot(const vec3f &rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

float vec3f::length() const
{
    return sqrt(x * x + y * y + z * z);
}

vec3f vec3f::normalise() const
{
    return *this * (1.f / this->length());
}

std::string vec3f::toString()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

vec3f::vec3f(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}
