#ifndef VEC2_H
#define VEC2_H

#include <string>


struct vec2f
{
    float x, y;

    vec2f operator+(const vec2f &rhs) const;
    vec2f operator-(const vec2f &rhs) const;
    vec2f operator*(float rhs) const;
    vec2f operator/(float rhs) const;
    vec2f& operator+=(const vec2f &rhs);
    vec2f& operator-=(const vec2f &rhs);
    vec2f& operator*=(float rhs);
    vec2f& operator/=(float rhs);
    bool operator==(const vec2f &rhs) const;
    bool operator<(const vec2f &rhs) const; // (lexicographic comparison only)
    float dot(const vec2f &rhs) const;
    float length() const;
    vec2f normalise() const;
    std::string toString();

    vec2f(float _x = 0, float _y = 0);
};

inline vec2f::vec2f(float _x, float _y)
{
    x = _x;
    y = _y;
}

typedef vec2f vec2;

struct vec3f
{
    float x, y, z;

    vec3f operator+(const vec3f &rhs) const;
    vec3f operator-(const vec3f &rhs) const;
    vec3f operator*(float rhs) const;
    vec3f operator/(float rhs) const;
    vec3f& operator+=(const vec3f &rhs);
    vec3f& operator-=(const vec3f &rhs);
    vec3f& operator*=(float rhs);
    vec3f& operator/=(float rhs);
    bool operator==(const vec3f &rhs) const;
    bool operator<(const vec3f &rhs) const; // (lexicographic comparison only)
    float dot(const vec3f &rhs) const;
    float length() const;
    vec3f normalise() const;
    std::string toString();

    vec3f(float _x = 0, float _y = 0, float _z = 0);
};

inline vec3f::vec3f(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}


#endif // VEC2_H
