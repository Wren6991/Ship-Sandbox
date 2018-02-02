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

int imin(int a, int b)
{
	return a < b ? a : b;
}

int imax(int a, int b)
{
	return a > b ? a : b;
}

void World::update(double dt)
{
	time += dt;
	// Advance simulation for points: (velocity and forces)
	for (unsigned int i = 0; i < points.size(); i++)
		points[i]->Update(dt);
	// Iterate the spring relaxation (can tune this parameter, or make it scale automatically depending on free time)
	doSprings(dt);
	// Check if any springs exceed their breaking strain:
	for (std::vector<Spring*>::iterator iter = springs.begin(); iter != springs.end();)
	{
		Spring *spr = *iter;
		iter++;
		if (spr->isBroken())        // have to delete after erasure - else there is a possibility of
			delete spr;             // other objects accessing a bad pointer during this cleanup
	}
	// Tell each ship to update all of its water stuff
	for (unsigned int i = 0; i < ships.size(); i++)
		ships[i]->update(dt);
}

void World::doSprings(double dt)
{
	int nchunks = springScheduler.GetNumberOfThreads();
	int springChunkSize = springs.size() / nchunks + 1;
	for (int outiter = 0; outiter < 3; outiter++)
	{
		for (int iteration = 0; iteration < 8; iteration++)
		{
			for (int i = springs.size() - 1; i > 0; i -= springChunkSize)
			{
				springScheduler.Schedule(new springCalculateTask(this, imax(i - springChunkSize, 0), i));
			}

			springScheduler.WaitForAllTasks();
		}
		float dampingamount = (1 - pow(0.0, dt)) * 0.5;
		for (unsigned int i = 0; i < springs.size(); i++)
			springs[i]->damping(dampingamount);
	}
}

World::springCalculateTask::springCalculateTask(World *_wld, int _first, int _last)
{
	wld = _wld;
	first = _first;
	last = _last;
}

void World::springCalculateTask::Process()
{
	for (int i = first; i <= last; i++)
		wld->springs[i]->update();
}

World::pointIntegrateTask::pointIntegrateTask(World *_wld, int _first, int _last, float _dt)
{
	wld = _wld;
	first = _first;
	last = _last;
	dt = _dt;
}

void World::pointIntegrateTask::Process()
{
	for (int i = first; i <= last; i++)
	{
		wld->points[i]->mPosition += wld->points[i]->mForce * dt;
		wld->points[i]->mForce = vec2(0, 0);
	}

}

void World::render(double left, double right, double bottom, double top) const
{
	// Draw the ocean floor
	renderLand(left, right, bottom, top);
	if (quickwaterfix)
		renderWater(left, right, bottom, top);
	// Draw all the points and springs
	for (unsigned int i = 0; i < points.size(); i++)
		points[i]->Render();
	for (unsigned int i = 0; i < springs.size(); i++)
		springs[i]->render();
	if (!xraymode)
		for (unsigned int i = 0; i < ships.size(); i++)
			ships[i]->render();
	if (showstress)
		for (unsigned int i = 0; i < springs.size(); i++)
			if (springs[i]->isStressed())
				springs[i]->render(true);
	if (!quickwaterfix)
		renderWater(left, right, bottom, top);
	glBegin(GL_LINES);
	glLineWidth(1.f);
	glEnd();
	//buildBVHTree(true, points, collisionTree);
}

void swapf(float &x, float &y)
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

void World::buildBVHTree(bool splitInX, std::vector<Point*> &pointlist, BVHNode *thisnode, int depth)
{
	int npoints = pointlist.size();
	if (npoints)
		thisnode->volume = pointlist[0]->GetAABB();
	for (unsigned int i = 1; i < npoints; i++)
		thisnode->volume.extendTo(pointlist[i]->GetAABB());

	thisnode->volume.render();
	if (npoints <= BVHNode::MAX_N_POINTS || depth >= BVHNode::MAX_DEPTH)
	{
		thisnode->isLeaf = true;
		thisnode->pointCount = npoints;
		for (int i = 0; i < npoints; i++)
			thisnode->points[i] = pointlist[i];
	}
	else
	{
		float pivotline = splitInX ?
			medianOf3(pointlist[0]->mPosition.x, pointlist[npoints / 2]->mPosition.x, pointlist[npoints - 1]->mPosition.x) :
			medianOf3(pointlist[0]->mPosition.y, pointlist[npoints / 2]->mPosition.y, pointlist[npoints - 1]->mPosition.y);
		std::vector<Point*> listL;
		std::vector<Point*> listR;
		listL.reserve(npoints / 2);
		listR.reserve(npoints / 2);
		for (int i = 0; i < npoints; i++)
		{
			if (splitInX ? pointlist[i]->mPosition.x < pivotline : pointlist[i]->mPosition.y < pivotline)
				listL.push_back(pointlist[i]);
			else
				listR.push_back(pointlist[i]);
		}
		buildBVHTree(!splitInX, listL, thisnode->l, depth + 1);
		buildBVHTree(!splitInX, listR, thisnode->r, depth + 1);
	}
}

void World::renderLand(double left, double right, double bottom, double top) const
{
	glColor4f(0.5, 0.5, 0.5, 1);
	double slicewidth = (right - left) / 200.0;
	for (double slicex = left; slicex < right; slicex += slicewidth)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(slicex, oceanfloorheight(slicex), -1);
		glVertex3f(slicex + slicewidth, oceanfloorheight(slicex + slicewidth), -1);
		glVertex3f(slicex, bottom, -1);
		glVertex3f(slicex + slicewidth, bottom, -1);
		glEnd();
	}
}

void World::renderWater(double left, double right, double bottom, double top) const
{
	// Cut the water into vertical slices (to get the different heights of waves) and draw it
	glColor4f(0, 0.25, 1, 0.5);
	double slicewidth = (right - left) / 100.0;
	for (double slicex = left; slicex < right; slicex += slicewidth)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(slicex, waterheight(slicex), -1);
		glVertex3f(slicex + slicewidth, waterheight(slicex + slicewidth), -1);
		glVertex3f(slicex, bottom, -1);
		glVertex3f(slicex + slicewidth, bottom, -1);
		glEnd();
	}
}

float World::oceanfloorheight(float x) const
{
	/*x += 1024.f;
	x = x - 2048.f * floorf(x / 2048.f);
	float t = x - floorf(x);
	return oceandepthbuffer[(int)floorf(x)] * (1 - t) + oceandepthbuffer[((int)ceilf(x)) % 2048] * t;*/
	return (sinf(x * 0.005f) * 10.f + sinf(x * 0.015f) * 6.f - sin(x * 0.0011f) * 45.f) - seadepth;
}

// Function of time and x (though time is constant during the update step, so no need to parameterise it)
float World::waterheight(float x) const
{
	return (sinf(x * 0.1f + time) * 0.5f + sinf(x * 0.3f - time * 1.1f) * 0.3f) * waveheight;
}

// Destroy all points within a 0.5m radius (could parameterise the radius but...)
void World::destroyAt(vec2 pos, float radius)
{
	for (std::vector<Point*>::iterator iter = points.begin(); iter != points.end();)
	{
		Point *p = *iter;
		iter++;
		if ((p->mPosition - pos).length() < radius)
		{
			delete p;           // have to remove reference before deleting, else other cleanup code will use bad memory!
			iter--;
		}
	}
}

// Attract all points to a single position
void World::drawTo(vec2f target)
{
	for (std::vector<Point*>::iterator iter = points.begin(); iter != points.end(); iter++)
	{
		vec2f & position = (*iter)->mPosition;
		vec2f dir = (target - position);
		double magnitude = 50000 / sqrt(0.1 + dir.length());
		(*iter)->ApplyForce(dir.normalise() * magnitude);
	}
}

void World::AddShip(std::unique_ptr<Ship> && newShip)
{
	ships.push_back(std::move(newShip));
}

// Copy parameters and set up initial params:
World::World(vec2f gravity, double _buoyancy, double _strength)
	: mGravity(gravity)
	, mGravityNormal(mGravity.normalise())
	, mGravityLength(mGravity.length())
{
	time = 0;
	buoyancy = _buoyancy;
	strength = _strength;
	waterpressure = 0.3;
	waveheight = 1.0;
	seadepth = 150;
	collisionTree = BVHNode::allocateTree();
}

// Destroy everything in the set order
World::~World()
{
	// DESTROY THE WORLD??? Y/N
	for (unsigned int i = 0; i < springs.size(); i++)
		delete springs[i];
	springs.clear();
	for (unsigned int i = 0; i < points.size(); i++)
		delete points[i];
	/*
	for (unsigned int i = 0; i < ships.size(); i++)
		delete ships[i];
	*/
}

World::BVHNode* World::BVHNode::allocateTree(int depth)
{
	if (depth <= 0)
		return 0;
	BVHNode *thisnode = new BVHNode;
	thisnode->l = allocateTree(depth - 1);
	thisnode->r = allocateTree(depth - 1);
	return thisnode;
}

}