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
	, mPosition(position)
	, mLastPosition(position)
	, mMaterial(material)
	, mForce()
	, mBuoyancy(buoyancy)
	, mWater(0.0f)
	, mIsLeaking(false)
    , mConnectedTriangles()
{
}

void Point::Destroy()
{
    // Destroy all springs attached to this point
    for (Spring * spring : mConnectedSprings)
    {
        assert(!spring->IsDeleted());
        spring->DestroyFromPoint(this);
    }

	// Destroy connected triangles
    DestroyConnectedTriangles();
    
    // Remove ourselves
    ShipElement::Destroy();
}

vec3f Point::GetColour(vec3f const & baseColour) const
{
	static const vec3f WetColour(0.0f, 0.0f, 0.8f);

	float colorWetness = fminf(mWater, 1.0f) * 0.7f;
	return baseColour * (1.0f - colorWetness) + WetColour * colorWetness;
}

void Point::Breach()
{
    // Start leaking
	mIsLeaking = true;

    // Destroy all of our connected triangles
    DestroyConnectedTriangles();
}

void Point::Update(
	float dt,
	GameParameters const & gameParameters)
{    
    float const waterHeightAtThisPoint = GetParentShip()->GetParentWorld()->GetWaterHeight(mPosition.x, gameParameters);
	float const mass = mMaterial->Mass;

    // Save current position, which will be the next LastPosition
    vec2f const newLastPosition = mPosition;

    //
    // 1 - Apply forces:
    //  Force1: Gravity on point mass + water mass (for all points)
    //  Force2: Buoyancy of point mass (only for underwater points)
    //

    float effectiveMassMultiplier = 1.0f + fminf(mWater, 1.0f) * gameParameters.BuoyancyAdjustment * mBuoyancy; // clamp water to 1, so high pressure areas are not heavier.
    if (mPosition.y < waterHeightAtThisPoint)
    {
        // Also consider buoyancy of own mass
        effectiveMassMultiplier -= gameParameters.BuoyancyAdjustment * mBuoyancy;
    }

    this->ApplyForce(gameParameters.Gravity * mass * effectiveMassMultiplier);
	

    //
	// 2 - Water drag
    //
    // (Note: to be checked)
    //

	if (mPosition.y < waterHeightAtThisPoint)
		mLastPosition += (mPosition - mLastPosition) * (1.0f - powf(0.6f, dt));

    //
	// 3 - Apply verlet integration
    //
    // x += v*dt + a*dt^2
    //
    // (Note: to be checked)
    //

	mPosition += (mPosition - mLastPosition) + mForce * (dt * dt / mass);


    //
 	// 4 - Handle collision with seafloor
    //
    // (Note: to be checked)
    //

	float const floorheight = GetParentShip()->GetParentWorld()->GetOceanFloorHeight(mPosition.x, gameParameters);
	if (mPosition.y < floorheight)
	{
		vec2f dir = vec2f(floorheight - GetParentShip()->GetParentWorld()->GetOceanFloorHeight(mPosition.x + 0.01f, gameParameters), 0.01f).normalise();   // -1 / derivative  => perpendicular to surface!
		mPosition += dir * (floorheight - mPosition.y);
 	}
	
 
	//
	// Finalize
	//

    // Remember previous position
    mLastPosition = newLastPosition;

    // Reset force
	mForce = vec2f(0, 0);
}

void Point::DestroyConnectedTriangles()
{
    for (Triangle * triangle : mConnectedTriangles)
    {
        assert(!triangle->IsDeleted());
        triangle->DestroyFromPoint(this);
    }

    mConnectedTriangles.clear();
}

}