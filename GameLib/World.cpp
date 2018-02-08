/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "GameOpenGL.h"
#include "RenderUtils.h"

#include <algorithm>
#include <cassert>

namespace /*anonymous*/ {

	void swapf(float & x, float & y)
	{
		float temp = x;
		x = y;
		y = temp;
	}

	float medianOf3(float a, float b, float c)
	{
		if (a < b)
			swapf(a, b);
		if (b < c)
			swapf(b, c);
		if (a < b)
			swapf(a, b);
		return b;
	}
};

namespace Physics {

// W     W    OOO    RRRR     L        DDDD
// W     W   O   O   R   RR   L        D  DDD
// W     W  O     O  R    RR  L        D    DD
// W     W  O     O  R   RR   L        D     D
// W     W  O     O  RRRR     L        D     D
// W  W  W  O     O  R RR     L        D     D
// W  W  W  O     O  R   R    L        D    DD
//  W W W    O   O   R    R   L        D  DDD
//   W W      OOO    R     R  LLLLLLL  DDDD

World::World()
	: mAllShips()
	, mCurrentTime(0.0f)
	, mCollisionTree(BVHNode::AllocateTree())
{
}

// Function of time and x (though time is constant during the update step, so no need to parameterise it)
float World::GetWaterHeight(
	float x,
	GameParameters const & gameParameters) const
{
	float const c1 = sinf(x * 0.1f + mCurrentTime) * 0.5f;
	float const c2 = sinf(x * 0.3f - mCurrentTime * 1.1f) * 0.3f;
	return (c1 + c2) * gameParameters.WaveHeight;
}

float World::GetOceanFloorHeight(
	float x,
	GameParameters const & gameParameters) const
{
	/*x += 1024.f;
	x = x - 2048.f * floorf(x / 2048.f);
	float t = x - floorf(x);
	return oceandepthbuffer[(int)floorf(x)] * (1 - t) + oceandepthbuffer[((int)ceilf(x)) % 2048] * t;*/

	float const c1 = sinf(x * 0.005f) * 10.f;
	float const c2 = sinf(x * 0.015f) * 6.f;
	float const c3 = sin(x * 0.0011f) * 45.f;
	return (c1 +  c2 - c3) - gameParameters.SeaDepth;
}

void World::AddShip(std::unique_ptr<Ship> && newShip)
{
	mAllShips.push_back(std::move(newShip));
}

void World::DestroyAt(
    vec2 const & targetPos, 
    float radius)
{
    for (auto & ship : mAllShips)
    {
        ship->DestroyAt(
            targetPos,
            radius);
    }
}

void World::DrawTo(
    vec2f const & targetPos,
    float strength)
{
    for (auto & ship : mAllShips)
    {
        ship->DrawTo(
            targetPos,
            strength);
    }
}

void World::Update(
	float dt,
	GameParameters const & gameParameters)
{
	mCurrentTime += dt;

	for (auto & ship : mAllShips)
	{
		ship->Update(
			dt,
			gameParameters);
	}
}

void World::Render(	
	float left,
	float right,
	float bottom,
	float top,
	GameParameters const & gameParameters,
	RenderParameters const & renderParameters) const
{
	// Draw the ocean floor
	RenderLand(left, right, bottom, top, gameParameters, renderParameters);
	if (renderParameters.QuickWaterFix)
		RenderWater(left, right, bottom, top, gameParameters, renderParameters);

	// Render all ships
	for (auto const & ship : mAllShips)
	{
		ship->Render(
			gameParameters,
			renderParameters);
	}

	if (!renderParameters.QuickWaterFix)
		RenderWater(left, right, bottom, top, gameParameters, renderParameters);

	glBegin(GL_LINES);
	glLineWidth(1.0f);
	glEnd();
	//buildBVHTree(true, points, collisionTree);
}

///////////////////////////////////////////////////////////////////////////////////
// Private Helpers
///////////////////////////////////////////////////////////////////////////////////

void World::RenderLand(
	float left,
	float right,
	float bottom,
	float /* top */,
	GameParameters const & gameParameters,
	RenderParameters const & /* renderParameters */) const
{
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	float slicewidth = (right - left) / 200.0f;
	for (float slicex = left; slicex < right; slicex += slicewidth)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(slicex, GetOceanFloorHeight(slicex, gameParameters), -1.0f);
		glVertex3f(slicex + slicewidth, GetOceanFloorHeight(slicex + slicewidth, gameParameters), -1.0f);
		glVertex3f(slicex, bottom, -1.0f);
		glVertex3f(slicex + slicewidth, bottom, -1.0f);
		glEnd();
	}
}

void World::RenderWater(
	float left,
	float right,
	float bottom,
	float /* top */,
	GameParameters const & gameParameters,
	RenderParameters const & /* renderParameters */) const
{
	// Cut the water into vertical slices (to get the different heights of waves) and draw it
	glColor4f(0.0f, 0.25f, 1.0f, 0.5f);
	float slicewidth = (right - left) / 100.0f;
	for (float slicex = left; slicex < right; slicex += slicewidth)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(slicex, GetWaterHeight(slicex, gameParameters), -1.0f);
		glVertex3f(slicex + slicewidth, GetWaterHeight(slicex + slicewidth, gameParameters), -1.0f);
		glVertex3f(slicex, bottom, -1.0f);
		glVertex3f(slicex + slicewidth, bottom, -1.0f);
		glEnd();
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Experimental
///////////////////////////////////////////////////////////////////////////////////

void World::BuildBVHTree(bool splitInX, std::vector<Point*> &pointlist, BVHNode *thisnode, int depth)
{
	size_t npoints = pointlist.size();
	if (npoints != 0)
		thisnode->volume = pointlist[0]->GetAABB();
	for (size_t i = 1; i < npoints; i++)
		thisnode->volume.extendTo(pointlist[i]->GetAABB());

	thisnode->volume.render();
	if (npoints <= BVHNode::MAX_N_POINTS || depth >= BVHNode::MAX_DEPTH)
	{
		thisnode->isLeaf = true;
		thisnode->pointCount = npoints;
		for (size_t i = 0; i < npoints; i++)
			thisnode->points[i] = pointlist[i];
	}
	else
	{
		float pivotline = splitInX ?
			medianOf3(pointlist[0]->GetPosition().x, pointlist[npoints / 2]->GetPosition().x, pointlist[npoints - 1]->GetPosition().x) :
			medianOf3(pointlist[0]->GetPosition().y, pointlist[npoints / 2]->GetPosition().y, pointlist[npoints - 1]->GetPosition().y);
		std::vector<Point*> listL;
		std::vector<Point*> listR;
		listL.reserve(npoints / 2);
		listR.reserve(npoints / 2);
		for (size_t i = 0; i < npoints; i++)
		{
			if (splitInX ? pointlist[i]->GetPosition().x < pivotline : pointlist[i]->GetPosition().y < pivotline)
				listL.push_back(pointlist[i]);
			else
				listR.push_back(pointlist[i]);
		}
		BuildBVHTree(!splitInX, listL, thisnode->l, depth + 1);
		BuildBVHTree(!splitInX, listR, thisnode->r, depth + 1);
	}
}

World::BVHNode* World::BVHNode::AllocateTree(int depth)
{
	if (depth <= 0)
		return 0;
	BVHNode *thisnode = new BVHNode;
	thisnode->l = AllocateTree(depth - 1);
	thisnode->r = AllocateTree(depth - 1);
	return thisnode;
}

}