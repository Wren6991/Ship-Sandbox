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
	float buoyancy,
    int elementIndex)
	: ShipElement(parentShip)
	, mPosition(position)
	, mLastPosition(position)
	, mMaterial(material)
	, mForce()
	, mBuoyancy(buoyancy)
    , mIsLeaking(false)
	, mWater(0.0f)
    , mLight(0.0f)
    , mElementIndex(elementIndex)
    , mConnectedSprings()
    , mConnectedTriangles()
    , mConnectedElectricalElement(nullptr)
    , mConnectedComponentId(0u)
    , mCurrentConnectedComponentDetectionStepSequenceNumber(0u)

{
}

void Point::Destroy()
{
    //
    // Destroy all springs attached to this point
    //

    for (Spring * spring : mConnectedSprings)
    {
        assert(!spring->IsDeleted());
        spring->Destroy(this);
    }

    mConnectedSprings.clear();

    //
	// Destroy connected triangles
    //

    DestroyConnectedTriangles();

    assert(0u == mConnectedTriangles.size());

    //
    // Destroy connected electrical elements
    //

    if (nullptr != mConnectedElectricalElement)
    { 
        mConnectedElectricalElement->Destroy();
        mConnectedElectricalElement = nullptr;
    }
    

    //
    // Remove ourselves
    //

    ShipElement::Destroy();
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
    float dragCoefficient,
	GameParameters const & gameParameters)
{    
    // Save current position, which will be the next LastPosition
    vec2f const newLastPosition = mPosition;

    // Get height of water at this point
    float const waterHeightAtThisPoint = GetParentShip()->GetParentWorld()->GetWaterHeight(mPosition.x, gameParameters);

    // Get our mass
	float const mass = mMaterial->Mass;

    // Calculate current velocity (already relative to dt, not to 1.0 sec)
    vec2f velocity = mPosition - mLastPosition;

    //
    // 1 - Apply forces:
    //  Force1: Gravity on (point mass + water mass) (for all points)
    //  Force2: Buoyancy of point mass (only for underwater points)
    //

    // Clamp water to 1, so high pressure areas are not heavier
    float const effectiveBuoyancy = gameParameters.BuoyancyAdjustment * mBuoyancy;
    float effectiveMassMultiplier = 1.0f + fminf(mWater, 1.0f) * effectiveBuoyancy; 
    if (mPosition.y < waterHeightAtThisPoint)
    {
        // Also consider buoyancy of own mass
        effectiveMassMultiplier -= effectiveBuoyancy;        
    }

    AddForce(gameParameters.Gravity * mass * effectiveMassMultiplier);


    //
    // 2 - Apply water drag:
    //  Proportional to current velocity
    //
    // TBD: should probably consider normal to surface at this point, so that masses
    // would also have a horizontal movement component when sinking
    //

    if (mPosition.y < waterHeightAtThisPoint)
    {
        // TODO: ain't this a force?
        velocity -= velocity * dragCoefficient;
    }


    //
	// 3 - Apply verlet integration
    //
    // x += v*dt + a*dt^2
    //
    // (Note: to be checked)
    //

	mPosition += velocity + (mForce / mass) * (dt * dt );


    //
 	// 4 - Handle collision with seafloor
    //

	float const floorheight = GetParentShip()->GetParentWorld()->GetOceanFloorHeight(mPosition.x, gameParameters);
	if (mPosition.y < floorheight)
	{
        // Calculate normal to surface
		vec2f surfaceNormal = vec2f(
            floorheight - GetParentShip()->GetParentWorld()->GetOceanFloorHeight(mPosition.x + 0.01f, gameParameters), 
            0.01f).normalise();   

        // Move point back along normal (this is *not* a bounce)
		mPosition += surfaceNormal * (floorheight - mPosition.y);
 	}
	
 
	//
	// Finalize
	//

    // Remember previous position
    mLastPosition = newLastPosition;

    // Reset force
    ZeroForce();
}

void Point::DestroyConnectedTriangles()
{
    for (Triangle * triangle : mConnectedTriangles)
    {
        assert(!triangle->IsDeleted());
        triangle->Destroy(this);
    }

    mConnectedTriangles.clear();
}

}