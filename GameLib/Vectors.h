/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cmath>
#include <string>

namespace /* anonymous */ {

	// Quake <3

	inline float fast_sqrt(float x)
	{
		union { float f; unsigned long ul; } y;
		y.f = x;
		y.ul = (0xBE6EB50CUL - y.ul) >> 1;
		y.f = 0.5f * y.f * (3.0f - x * y.f * y.f);
		return x * y.f;
	}

	inline float fast_inv_sqrt(float x)
	{
		union { float f; unsigned long ul; } y;
		y.f = x;
		y.ul = (0xBE6EB50CUL - y.ul) >> 1;
		y.f = 0.5f * y.f * (3.0f - x * y.f * y.f);
		return y.f;
	}
}

struct vec2f
{
public:
	
    float x, y;

	inline vec2f operator+(vec2f const & rhs) const
	{
		return vec2f(
			x + rhs.x, 
			y + rhs.y);
	}

	inline vec2f operator-(vec2f const & rhs) const
	{
		return vec2f(
			x - rhs.x, 
			y - rhs.y);
	}

	inline vec2f operator*(float rhs) const
	{
		return vec2f(
			x * rhs,
			y * rhs);
	}

	inline vec2f operator/(float rhs) const
	{
		return vec2f(
			x / rhs,
			y / rhs);
	}

	inline vec2f & operator+=(vec2f const & rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	inline vec2f & operator-=(vec2f const & rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	inline vec2f & operator*=(float rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	inline vec2f& operator/=(float rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}

	inline bool operator==(vec2f const & rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	// (lexicographic comparison only)
	inline bool operator<(vec2f const & rhs) const
	{
		return x < rhs.x || (x == rhs.x && y < rhs.y);
	}

	inline float dot(vec2f const & rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	inline float length() const
	{
		return sqrtf(x * x + y * y);
	}

	inline vec2f normalise() const
	{
		return (*this) * fast_inv_sqrt(x * x + y * y);
	}

    std::wstring toString();

	inline vec2f(
		float _x = 0,
		float _y = 0)
		: x(_x)
		, y(_y)
	{
	}
};

typedef vec2f vec2;

struct vec3f
{
public:
	
    float x, y, z;

	inline vec3f operator+(vec3f const & rhs) const
	{
		return vec3f(
			x + rhs.x,
			y + rhs.y,
			z + rhs.z);
	}

    inline vec3f operator-(vec3f const & rhs) const
	{
		return vec3f(
			x - rhs.x,
			y - rhs.y,
			z - rhs.z);
	}

	inline vec3f operator*(float rhs) const
	{
		return vec3f(
			x * rhs,
			y * rhs,
			z * rhs);
	}

	inline vec3f operator/(float rhs) const
	{
		return vec3f(
			x / rhs,
			y / rhs,
			z / rhs);
	}

	inline vec3f & operator+=(vec3f const & rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline vec3f & operator-=(vec3f const & rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	inline vec3f & operator*=(float rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	inline vec3f & operator/=(float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	inline bool operator==(vec3f const & rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	// (lexicographic comparison only)
	inline bool operator<(vec3f const & rhs) const
	{
		return x < rhs.x || (x == rhs.x && (y < rhs.y || (y == rhs.y && z < rhs.z)));
	}
    
	float dot(vec3f const & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
    
	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	vec3f normalise() const
	{
		return (*this) * fast_inv_sqrt(x * x + y * y + z * z);
	}

    std::wstring toString();

	inline vec3f(
		float _x = 0,
		float _y = 0,
		float _z = 0)
		: x(_x)
		, y(_y)
		, z(_z)
	{
	}
};

typedef vec3f vec3;
