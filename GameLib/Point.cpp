/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "RenderUtils.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include <GL/gl.h>

#include <algorithm>
#include <cassert>
// TODO
//#include <cmath>

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

// Just copies parameters into relevant fields:
Point::Point(
	World * parentWorld,
	vec2 position,
	Material const * material,
	double buoyancy)
	: mParentWorld(parentWorld)
	, mTriangles()
	, mPosition(position)
	, mLastPosition(position)
	, mMaterial(material)
	, mForce()
	, mBuoyancy(buoyancy)
	, mWater(0.0)
	, mIsLeaking(false)
{
	mParentWorld->points.push_back(this);
}

Point::~Point()
{
	// get rid of any attached triangles:
	Breach();
	// remove any springs attached to this point:
	for (std::vector<Spring*>::iterator iter = mParentWorld->springs.begin(); iter != mParentWorld->springs.end();)
	{
		Spring *spr = *iter;
		iter++;
		if (spr->a == this || spr->b == this)
		{
			delete spr;
			iter--;
		}
	}
	// remove any references:
	for (unsigned int i = 0; i < mParentWorld->ships.size(); i++)
		mParentWorld->ships[i]->points.erase(this);
	std::vector<Point*>::iterator iter = std::find(mParentWorld->points.begin(), mParentWorld->points.end(), this);
	if (iter != mParentWorld->points.end())
		mParentWorld->points.erase(iter);
}

vec3f Point::GetColour(vec3f basecolour) const
{
	double wetness = fmin(mWater, 1) * 0.7;
	return basecolour * (1 - wetness) + vec3f(0, 0, 0.8) * wetness;
}

double Point::GetPressure()
{
	return mParentWorld->mGravityLength * fmax(-mPosition.y, 0) * 0.1;  // 0.1 = scaling constant, represents 1/ship width
}

AABB Point::GetAABB()
{
	return AABB(mPosition - vec2(Radius, Radius), mPosition + vec2(Radius, Radius));
}

void Point::Breach()
{
	mIsLeaking = true;
	for (std::set<Triangle*>::iterator iter = mTriangles.begin(); iter != mTriangles.end();)
	{
		Triangle *t = *iter;
		iter++;
		delete t;
	}
}

void Point::Update(double dt)
{
	double mass = mMaterial->Mass;
	this->ApplyForce(mParentWorld->mGravity * (mass * (1 + fmin(mWater, 1) * mParentWorld->buoyancy * mBuoyancy)));    // clamp water to 1, so high pressure areas are not heavier.
																								  // Buoyancy:
	if (mPosition.y < mParentWorld->waterheight(mPosition.x))
		this->ApplyForce(mParentWorld->mGravity * (-mParentWorld->buoyancy * mBuoyancy * mass));
	vec2f newLastPosition = mPosition;
	// Water drag:
	if (mPosition.y < mParentWorld->waterheight(mPosition.x))
		mLastPosition += (mPosition - mLastPosition) * (1 - pow(0.6, dt));
	// Apply verlet integration:
	mPosition += (mPosition - mLastPosition) + mForce * (dt * dt / mass);
	// Collision with seafloor:
	float floorheight = mParentWorld->oceanfloorheight(mPosition.x);
	if (mPosition.y < floorheight)
	{
		vec2f dir = vec2f(floorheight - mParentWorld->oceanfloorheight(mPosition.x + 0.01f), 0.01f).normalise();   // -1 / derivative  => perpendicular to surface!
		mPosition += dir * (floorheight - mPosition.y);
	}
	mLastPosition = newLastPosition;
	mForce = vec2f(0, 0);
}

void Point::Render() const
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

}