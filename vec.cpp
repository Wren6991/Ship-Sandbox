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


// Quake <3

inline float fast_inv_sqrt(float x)
{
    union {float f; unsigned long ul;} y;
    y.f = x;
    y.ul = (0xBE6EB50CUL - y.ul) >> 1;
    y.f = 0.5f * y.f * (3.0f - x * y.f * y.f);
    return y.f;
}

inline float fast_sqrt(float x)
{
    union {float f; unsigned long ul;} y;
    y.f = x;
    y.ul = (0xBE6EB50CUL - y.ul) >> 1;
    y.f = 0.5f * y.f * (3.0f - x * y.f * y.f);
    return x * y.f;
}

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

bool vec2f::operator<(const vec2f &rhs) const
{
    return x < rhs.x || (x == rhs.x && y < rhs.y);
}

float vec2f::dot(const vec2f &rhs) const
{
    return x * rhs.x + y * rhs.y;
}

float vec2f::length() const
{
    return sqrtf(x * x + y * y);
}

vec2f vec2f::normalise() const
{
    return *this * fast_inv_sqrt(x * x + y * y);
}

std::string vec2f::toString()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
}

/*vec2f::vec2f(float _x, float _y)
{
    x = _x;
    y = _y;
}*/

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

bool vec3f::operator<(const vec3f &rhs) const
{
    return x < rhs.x || (x == rhs.x && (y < rhs.y || (y == rhs.y && z < rhs.z)));
}

float vec3f::dot(const vec3f &rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

float vec3f::length() const
{
    return sqrtf(x * x + y * y + z * z);
}

vec3f vec3f::normalise() const
{
    return *this * fast_inv_sqrt(x * x + y * y + z * z);
}

std::string vec3f::toString()
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

