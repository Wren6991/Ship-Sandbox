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

World::World(vec2f gravity)
	: mGravity(gravity)
	, mGravityNormal(mGravity.normalise())
	, mGravityMagnitude(mGravity.length())
	, mCurrentTime(0.0f)
	// Repository
	, mPoints()
	, mSprings()
	, mShips()
	, mScheduler()
	, mCollisionTree(BVHNode::AllocateTree())
{
}

// Destroy everything in the set order
World::~World()
{
	// DESTROY THE WORLD??? Y/N
	for (unsigned int i = 0; i < mSprings.size(); i++)
		delete mSprings[i];
	mSprings.clear();
	for (unsigned int i = 0; i < mPoints.size(); i++)
		delete mPoints[i];
	/*
	for (unsigned int i = 0; i < mShips.size(); i++)
		delete mShips[i];
	*/
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
	mShips.push_back(std::move(newShip));
}

void World::DestroyAt(vec2 const & targetPos, float radius)
{
	// Destroy all points within the radius

	for (std::vector<Point*>::iterator iter = mPoints.begin(); iter != mPoints.end();)
	{
		Point *p = *iter;
		iter++;
		if ((p->GetPosition() - targetPos).length() < radius)
		{
			delete p;           // have to remove reference before deleting, else other cleanup code will use bad memory!
			iter--;
		}
	}
}

void World::DrawTo(vec2f const & targetPos)
{
	// Attract all points to a single position

	for (std::vector<Point*>::iterator iter = mPoints.begin(); iter != mPoints.end(); iter++)
	{
		vec2f & position = (*iter)->GetPosition();
		vec2f dir = (targetPos - position);
		float magnitude = 50000.0f / sqrtf(0.1f + dir.length());
		(*iter)->ApplyForce(dir.normalise() * magnitude);
	}
}

void World::Update(
	float dt,
	GameParameters const & gameParameters)
{
	mCurrentTime += dt;

	// Advance simulation for points: (velocity and forces)
	for (unsigned int i = 0; i < mPoints.size(); i++)
		mPoints[i]->Update(dt, gameParameters);

	// Iterate the spring relaxation (can tune this parameter, or make it scale automatically depending on free time)
	DoSprings(dt);

	// Check if any springs exceed their breaking strain:
	for (std::vector<Spring*>::iterator iter = mSprings.begin(); iter != mSprings.end();)
	{
		Spring *spr = *iter;
		iter++;
		if (spr->GetIsBroken(gameParameters.StrengthAdjustment)) // have to delete after erasure - else there is a possibility of
			delete spr;             // other objects accessing a bad pointer during this cleanup
	}

	// Tell each ship to update all of its water stuff
	for (unsigned int i = 0; i < mShips.size(); i++)
		mShips[i]->Update(dt, gameParameters);
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

	// Draw all the points and springs
	for (unsigned int i = 0; i < mPoints.size(); i++)
		mPoints[i]->Render();
	for (unsigned int i = 0; i < mSprings.size(); i++)
		mSprings[i]->Render(false); 
	if (!renderParameters.UseXRayMode)
		for (unsigned int i = 0; i < mShips.size(); i++)
			mShips[i]->Render();
	if (renderParameters.ShowStress)
		for (unsigned int i = 0; i < mSprings.size(); i++)
			if (mSprings[i]->GetIsStressed(gameParameters.StrengthAdjustment))
				mSprings[i]->Render(true);

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

void World::DoSprings(float dt)
{
	if (mSprings.empty())
	{
		// Nothing to do!
		return;
	}

	// Calculate number of parallel chunks and size of each chunk
	int nParallelChunks = mScheduler.GetNumberOfThreads();
	size_t parallelChunkSize = (mSprings.size() / nParallelChunks) + 1;

	assert(parallelChunkSize > 0);

	for (int outiter = 0; outiter < 3; outiter++)
	{
		for (int iteration = 0; iteration < 8; iteration++)
		{
			for (size_t i = mSprings.size() - 1; i > 0; /* decremented in loop */)
			{
				size_t thisChunkSize = (i >= parallelChunkSize) ? parallelChunkSize : i;
				assert(thisChunkSize > 0);
				
				mScheduler.Schedule(
					new SpringCalculateTask(
						this, 
						i - thisChunkSize, 
						i));
				
				i -= thisChunkSize;
			}

			mScheduler.WaitForAllTasks();
		}

		float dampingamount = (1 - powf(0.0f, static_cast<float>(dt))) * 0.5f;
		for (size_t i = 0; i < mSprings.size(); i++)
			mSprings[i]->DoDamping(dampingamount);
	}
}

void World::SpringCalculateTask::Process()
{
	for (size_t i = mFirstSpringIndex; i <= mLastSpringIndex; i++)
		mParentWorld->mSprings[i]->Update();
}

void World::PointIntegrateTask::Process()
{
	for (size_t i = mFirstPointIndex; i <= mLastPointIndex; i++)
	{
		mParentWorld->mPoints[i]->AdjustPosition(mParentWorld->mPoints[i]->GetForce() * mDt);
		mParentWorld->mPoints[i]->ZeroForce();
	}
}

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