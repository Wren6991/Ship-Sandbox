/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameParameters.h"
#include "Material.h"
#include "Physics.h"
#include "RenderParameters.h"
#include "Scheduler.h"
#include "Vectors.h"

#include <memory>
#include <set>
#include <vector>

namespace Physics
{

class World
{
public:

	World(vec2 gravity = vec2(0.0f, -9.8f));

	~World();

	vec2 const & GetGravity() const { return mGravity; };
	vec2 const & GetGravityNormal() const { return mGravityNormal; };
	float const GetGravityMagnitude() const { return mGravityMagnitude;  };

	float GetWaterHeight(		
		float x,
		GameParameters const & gameParameters) const;

	float GetOceanFloorHeight(
		float x,
		GameParameters const & gameParameters) const;

	void AddShip(std::unique_ptr<Ship> && newShip);

	void DestroyAt(vec2 const & targetPos, float radius);
	void DrawTo(vec2 const & targetPos);

	void Update(
		float dt,
		GameParameters const & gameParameters);

	void Render(		
		float left,
		float right,
		float bottom,
		float top,
		GameParameters const & gameParameters,
		RenderParameters const & renderParameters) const;

private:

	// TODO: nuke
	friend class Point;
	friend class Spring;

	struct SpringCalculateTask : Scheduler::ITask
	{
	public:

		SpringCalculateTask(
			World * parentWorld,
			size_t firstSpringIndex,
			size_t lastSpringIndex)
			: mParentWorld(parentWorld)
			, mFirstSpringIndex(firstSpringIndex)
			, mLastSpringIndex(lastSpringIndex)
		{
		}

		virtual ~SpringCalculateTask()
		{
		}

		virtual void Process();

	private:

		World * const mParentWorld;
		size_t const mFirstSpringIndex;
		size_t const mLastSpringIndex;
	};

	struct PointIntegrateTask : Scheduler::ITask
	{
	public:

		PointIntegrateTask(
			World * parentWorld,
			size_t firstPointIndex,
			size_t lastPointIndex,
			float dt)
			: mParentWorld(parentWorld)
			, mFirstPointIndex(firstPointIndex)
			, mLastPointIndex(lastPointIndex)
			, mDt(dt)
		{
		}

		virtual ~PointIntegrateTask()
		{
		}

		virtual void Process();

	private:

		World * const mParentWorld;
		size_t const mFirstPointIndex;
		size_t const mLastPointIndex;
		float const mDt;
	};

	vec2 const mGravity;
	vec2 const mGravityNormal;
	float const mGravityMagnitude;

	float mCurrentTime;

	// Repository
	std::vector<Point*> mPoints;
	std::vector<Spring*> mSprings;
	std::vector<std::unique_ptr<Ship>> mShips;

	Scheduler mScheduler;

	void DoSprings(float dt);
	
	void RenderLand(
		float left,
		float right,
		float bottom,
		float top,
		GameParameters const & gameParameters,
		RenderParameters const & renderParameters) const;

	void RenderWater(
		float left,
		float right,
		float bottom,
		float top,
		GameParameters const & gameParameters,
		RenderParameters const & renderParameters) const;

	// TODO: public or private?
	float const *oceandepthbuffer;

private:

	//
	// TODO: experimental
	//

	struct BVHNode
	{
		AABB volume;
		BVHNode *l, *r;
		bool isLeaf;
		int pointCount;
		static const int MAX_DEPTH = 15;
		static const int MAX_N_POINTS = 10;
		Point* points[MAX_N_POINTS];
		static BVHNode * AllocateTree(int depth = MAX_DEPTH);
	};

	void BuildBVHTree(bool splitInX, std::vector<Point*> &pointlist, BVHNode *thisnode, int depth = 1);

	BVHNode * mCollisionTree;
};

}
