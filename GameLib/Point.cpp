/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include <cassert>
#include <cmath>

namespace Physics {

// PPPP       OOO    IIIIIII  N     N  TTTTTTT
// P   PP    O   O      I     NN    N     T
// P    PP  O     O     I     N N   N     T
// P   PP   O     O     I     N N   N     T
// PPPP     O     O     I     N  N  N     T
// P        O     O     I     N   N N     T
// P        O     O     I     N   N N     T
// P         O   O      I     N    NN     T
// P          OOO    IIIIIII  N     N     T

vec2 const Point::AABBRadius = vec2(0.4f, 0.4f);

Point::Point(
	Ship * parentShip,
	vec2 const & position,
	Material const * material,
	float buoyancy)
	: ShipElement(parentShip)
	, mTriangles()
	, mPosition(position)
	, mLastPosition(position)
	, mMaterial(material)
	, mForce()
	, mBuoyancy(buoyancy)
	, mWater(0.0f)
	, mIsLeaking(false)
{
	// TODO: NUKE and make this inline
	parentShip->mParentWorld->mPoints.push_back(this);
}

Point::~Point()
{
	// Get rid of attached triangles and start leaking
	Breach();

	// remove any springs attached to this point:
	for (auto iter = GetParentShip()->mParentWorld->mSprings.begin(); iter != GetParentShip()->mParentWorld->mSprings.end();)
	{
		Spring *spr = *iter;
		iter++;
		if (spr->GetPointA() == this || spr->GetPointB() == this)
		{
			delete spr;
			iter--;
		}
	}
	// remove any references:
	GetParentShip()->mPoints.erase(this);
	std::vector<Point*>::iterator iter = std::find(GetParentShip()->mParentWorld->mPoints.begin(), GetParentShip()->mParentWorld->mPoints.end(), this);
	if (iter != GetParentShip()->mParentWorld->mPoints.end())
		GetParentShip()->mParentWorld->mPoints.erase(iter);
}

vec3f Point::GetColour(vec3f const & baseColour) const
{
	static const vec3f WetColour(0.0f, 0.0f, 0.8f);

	float colorWetness = fminf(mWater, 1.0f) * 0.7f;
	return baseColour * (1.0f - colorWetness) + WetColour * colorWetness;
}

float Point::GetPressure(float gravityMagnitude) const
{
	// TBD: pressure is always zero? 
	return gravityMagnitude * fmaxf(-mPosition.y, 0.0f) * 0.1f;  // 0.1 = scaling constant, represents 1/ship width
}

void Point::Breach()
{
	mIsLeaking = true;

	for (auto iter = mTriangles.begin(); iter != mTriangles.end();)
	{
		Triangle *t = *iter;
		iter++;
		delete t;
	}
}

void Point::Update(
	float dt,
	GameParameters const & gameParameters)
{
	// TODO: potential to optimize by calculating/invoking things only once

	float mass = mMaterial->Mass;
	this->ApplyForce(gameParameters.Gravity * (mass * (1.0f + fminf(mWater, 1.0f) * gameParameters.BuoyancyAdjustment * mBuoyancy)));    // clamp water to 1, so high pressure areas are not heavier.
	// Buoyancy:
	if (mPosition.y < GetParentShip()->mParentWorld->GetWaterHeight(mPosition.x, gameParameters))
		this->ApplyForce(gameParameters.Gravity * (-gameParameters.BuoyancyAdjustment * mBuoyancy * mass));
	vec2f newLastPosition = mPosition;
	// Water drag:
	if (mPosition.y < GetParentShip()->mParentWorld->GetWaterHeight(mPosition.x, gameParameters))
		mLastPosition += (mPosition - mLastPosition) * (1.0f - powf(0.6f, dt));
	// Apply verlet integration:
	mPosition += (mPosition - mLastPosition) + mForce * (dt * dt / mass);
	// Collision with seafloor:
	float floorheight = GetParentShip()->mParentWorld->GetOceanFloorHeight(mPosition.x, gameParameters);
	if (mPosition.y < floorheight)
	{
		vec2f dir = vec2f(floorheight - GetParentShip()->mParentWorld->GetOceanFloorHeight(mPosition.x + 0.01f, gameParameters), 0.01f).normalise();   // -1 / derivative  => perpendicular to surface!
		mPosition += dir * (floorheight - mPosition.y);
	}
	mLastPosition = newLastPosition;

	//
	// Reset force
	//

	mForce = vec2f(0, 0);
}

}