#ifndef VEC2_H
#define VEC2_H

#include <string>


struct vec2
{
    double x, y;

    vec2 operator+(const vec2 &rhs) const;
    vec2 operator-(const vec2 &rhs) const;
    vec2 operator*(double rhs) const;
    vec2 operator/(double rhs) const;
    vec2& operator+=(const vec2 &rhs);
    vec2& operator-=(const vec2 &rhs);
    vec2& operator*=(double rhs);
    vec2& operator/=(double rhs);
    bool operator==(const vec2 &rhs) const;
    double dot(const vec2 &rhs) const;
    double length() const;
    vec2 normalise() const;
    std::string toString();

    vec2(double _x = 0, double _y = 0);
};

#endif // VEC2_H
