/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cmath>
#include <cstddef>
#include <string>

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

    inline bool operator!=(vec2f const & rhs) const
    {
        return !(*this == rhs);
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

    inline float squareLength() const
    {
        return x * x + y * y;
    }

	inline vec2f normalise() const
	{
        return (*this) / sqrtf(x * x + y * y);
	}

    inline vec2f normalise(float length) const
    {
        return (*this) / length;
    }

    std::string toString() const;

	inline vec2f(
		float _x = 0,
		float _y = 0)
		: x(_x)
		, y(_y)
	{
	}
};

static_assert(offsetof(vec2f, x) == 0);
static_assert(offsetof(vec2f, y) == sizeof(float));
static_assert(sizeof(vec2f) == 2 * sizeof(float));

using vec2 = vec2f;

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

    inline bool operator!=(vec3f const & rhs) const
    {
        return !(*this == rhs);
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
		return (*this) / sqrtf(x * x + y * y + z * z);
	}

    std::string toString() const;

	inline constexpr vec3f(
		float _x = 0,
		float _y = 0,
		float _z = 0)
		: x(_x)
		, y(_y)
		, z(_z)
	{
	}
};

static_assert(offsetof(vec3f, x) == 0);
static_assert(offsetof(vec3f, y) == sizeof(float));
static_assert(offsetof(vec3f, z) == 2 * sizeof(float));
static_assert(sizeof(vec3f) == 3 * sizeof(float));

using vec3 = vec3f;
