/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "AABB.h"
#include "Material.h"
#include "Physics.h"
#include "Vectors.h"

#include <set>

namespace Physics
{	

class Ship; 
class Spring; 
class Triangle;
class World;

class Point
{
public:

	Point(
		World * parentWorld, 
		vec2 position, 
		Material const * material, 
		double buoyancy);

	~Point();

	vec2 const & GetPos() const { return mPosition; }
	vec3f GetColour(vec3f basecolour) const;
	Material const * GetMaterial() const { return mMaterial; }
	double GetPressure();
	bool GetIsLeaking() const { return mIsLeaking;  }
	std::set<Triangle *> const & GetTriangles() const { return mTriangles; }
	AABB GetAABB();

	void ApplyForce(vec2 const & force) { mForce += force; }
	void Breach();  // set to leaking and remove any incident triangles
	void Update(double dt);
	void Render() const;

private:
		
	friend class Ship;
	friend class Spring;
	friend class Triangle;
	friend class World;

	World * const mParentWorld;
	std::set<Triangle *> mTriangles;

	static constexpr float Radius = 0.4f;
		
	vec2 mPosition;
	vec2 mLastPosition;
	Material const * mMaterial;
	vec2 mForce;

	double mBuoyancy;
	double mWater;
	bool mIsLeaking;
};

}
