/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"
#include "Physics.h"
#include "Scheduler.h"
#include "Vectors.h"

#include <set>

namespace Physics
{

class Point;
class Ship;
class Spring;
class Triangle;

class World
{
	friend class Point;
	friend class Spring;
	friend class Ship;

	struct springCalculateTask : Scheduler::ITask
	{
		springCalculateTask(World *_wld, int _first, int _last);
		World *wld;
		int first, last;
		virtual void Process();
	};

	struct pointIntegrateTask : Scheduler::ITask
	{
		pointIntegrateTask(World *_wld, int _first, int _last, float _dt);
		World *wld;
		float dt;
		int first, last;
		virtual void Process();
	};

	struct BVHNode
	{
		AABB volume;
		BVHNode *l, *r;
		bool isLeaf;
		int pointCount;
		static const int MAX_DEPTH = 15;
		static const int MAX_N_POINTS = 10;
		Point* points[MAX_N_POINTS];
		static BVHNode *allocateTree(int depth = MAX_DEPTH);
	};


	Scheduler springScheduler;
	std::vector<Point*> points;
	std::vector<Spring*> springs;
	std::vector<std::unique_ptr<Ship>> ships;
	BVHNode *collisionTree;
	float waterheight(float x) const;
	float oceanfloorheight(float x) const;
	void doSprings(double dt);
	vec2 const mGravity;
	vec2 const mGravityNormal;
	float const mGravityLength;
	void buildBVHTree(bool splitInX, std::vector<Point*> &pointlist, BVHNode *thisnode, int depth = 1);
public:
	float const *oceandepthbuffer;
	float buoyancy;
	float strength;
	float waterpressure;
	float waveheight;
	float seadepth;
	bool showstress;
	bool quickwaterfix;
	bool xraymode;
	float time;
	void update(double dt);
	void render(double left, double right, double bottom, double top) const;
	void renderLand(double left, double right, double bottom, double top) const;
	void renderWater(double left, double right, double bottom, double top) const;
	void destroyAt(vec2 pos, float radius);
	void drawTo(vec2 target);

	void AddShip(std::unique_ptr<Ship> && newShip);

	World(vec2 gravity = vec2(0, -9.8), double _buoyancy = 4, double _strength = 0.01);
	~World();
};

}
