/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "AABB.h"
#include "GameOpenGL.h"
#include "GameParameters.h"
#include "Material.h"
#include "Physics.h"
#include "RenderUtils.h"
#include "Vectors.h"

#include <set>

namespace Physics
{	

class Point : public ShipElement<Point> 
{
public:

	Point(
		Ship * parentShip,
		vec2 const & position,
		Material const * material,
		float buoyancy);

	~Point();

	inline auto const & GetTriangles() const { return mTriangles;  }
	inline void AddTriangle(Triangle * triangle) { mTriangles.insert(triangle); }
	inline void RemoveTriangle(Triangle * triangle) { mTriangles.erase(triangle); }

	inline vec2 const & GetPosition() const { return mPosition; }
	inline vec2 & GetPosition() { return mPosition; }
	inline void AdjustPosition(vec2 const & dPosition) { mPosition += dPosition; }
	
	inline vec2 const & GetLastPosition() const { return mLastPosition; }
	inline vec2 & GetLastPosition() { return mLastPosition; }

	inline Material const * GetMaterial() const { return mMaterial; }

	inline float GetMass() const { return mMaterial->Mass; }

	inline vec2 const & GetForce() const { return mForce; }
	inline void ZeroForce() { mForce = vec2(0, 0); }

	inline float GetBuoyancy() const { return mBuoyancy;  }

	// TBD: WaterPressure?
	inline float GetWater() const { return mWater; }
	inline void AdjustWater(float dWater) { mWater += dWater; }

	inline bool IsLeaking() const { return mIsLeaking;  }
	inline void SetLeaking() { mIsLeaking = true; }

	vec3f GetColour(vec3f const & baseColour) const;

	float GetPressure(float gravityMagnitude) const;

	inline AABB GetAABB() const
	{
		return AABB(mPosition - AABBRadius, mPosition + AABBRadius);
	}

	inline void ApplyForce(vec2 const & force)
	{ 
		mForce += force; 
	}	

	void Breach();

	void Update(
		float dt,
		GameParameters const & gameParameters);

	inline void Render() const
	{
		// Put a blue blob on leaking nodes (was more for debug purposes, but looks better IMO)
		if (mIsLeaking)
		{
			glColor3f(0, 0, 1);
			glBegin(GL_POINTS);
			glVertex3f(mPosition.x, mPosition.y, -1);
			glEnd();
		}
	}

private:

	static vec2 const AABBRadius;	

	std::set<Triangle *> mTriangles;

	vec2 mPosition;
	vec2 mLastPosition;

	Material const * mMaterial;
	vec2 mForce;
	float mBuoyancy;
	// TBD: WaterPressure?
	float mWater;
	bool mIsLeaking;
};

}
