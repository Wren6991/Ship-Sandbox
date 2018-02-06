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
#include "PointerContainer.h"
#include "Scheduler.h"
#include "Vectors.h"

#include <set>

namespace Physics
{

class Ship
{
public:

	// TODO: nuke
	friend class Point;
	friend class Spring;
	friend class Triangle;

	static std::unique_ptr<Ship> Create(
		World * parentWorld,
		unsigned char const * structureImageData,
		int structureImageWidth,
		int structureImageHeight,
		std::vector<std::unique_ptr<Material const>> const & allMaterials);

    ~Ship();

	void Destroy();

	World const * GetParentWorld() const { return mParentWorld; }
	World * GetParentWorld() { return mParentWorld; }

	auto const & GetPoints() const { return mAllPoints; }
    auto & GetPoints() { return mAllPoints; }

	auto const & GetPointAdjencyMap() const { return mAdjacentNonHullPoints; } // TODO: will go once these move to Point itself

	auto const & GetSprings() const { return mAllSprings; }
    auto & GetSprings() { return mAllSprings; }

	auto const & GetTriangles() const { return mAllTriangles; }

    void DestroyAt(
        vec2 const & targetPos,
        float radius);

    void DrawTo(vec2 const & targetPos);

	void LeakWater(
		float dt,
		GameParameters const & gameParameters);

	void GravitateWater(
		float dt,
		GameParameters const & gameParameters);

	void BalancePressure(float dt);

	void Update(
		float dt,
		GameParameters const & gameParameters);

	void Render(
		GameParameters const & gameParameters,
		RenderParameters const & renderParameters) const;

public:

	/*
	 * Invoked when an elements has been destroyed. Notifies the ship that the element
	 * can be removed (at the most appropriate time) from the ship's main container
	 * of element pointers, and that the pointer can be deleted.
	 *
	 * Implemented differently for the different elements.
	 */
	template<typename TElement>
	void RegisterDestruction(TElement * element);

private:

    Ship(World * parentWorld);

	void Initialize(
        std::vector<Point *> && allPoints,
		std::vector<Spring *> && allSprings,
        std::set<Triangle *> && allTriangles)
	{
        mAllPoints.initialize(std::move(allPoints));
		mAllSprings.initialize(std::move(allSprings));
        mAllTriangles = allTriangles;
	}

	void DoSprings(float dt);

	struct SpringCalculateTask : Scheduler::ITask
	{
	public:

		SpringCalculateTask(
			Ship * parentShip,
			size_t startSpringIndex,
			size_t endSpringIndex)
			: mParentShip(parentShip)
			, mStartSpringIndex(startSpringIndex)
			, mEndSpringIndex(endSpringIndex)
		{
		}

		virtual ~SpringCalculateTask()
		{
		}

		virtual void Process();

	private:

		Ship * const mParentShip;
		size_t const mStartSpringIndex;
		size_t const mEndSpringIndex; // 1 past last
	};

	struct PointIntegrateTask : Scheduler::ITask
	{
	public:

		PointIntegrateTask(
			Ship * parentShip,
			size_t firstPointIndex,
			size_t lastPointIndex,
			float dt)
			: mParentShip(parentShip)
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

		Ship * const mParentShip;
		size_t const mFirstPointIndex;
		size_t const mLastPointIndex;
		float const mDt;
	};

private:

	World * const mParentWorld;

	// Repository
    PointerContainer<Point> mAllPoints;
	PointerContainer<Spring> mAllSprings;
    std::set<Triangle *> mAllTriangles;

    // Indices	
    // TODO: each arc here is a non-hull spring, and lives
    // as long as the spring lives; 
    // We might then get rid of these and just visit the non-hull springs,
    // provided that we adjust GravitateWater and BalancePressure
    // for the fact that each arc is now visited only once
	std::map<Point *, std::set<Point*>> mAdjacentNonHullPoints;
	
	// The scheduler we use for parallelizing updates
	Scheduler mScheduler;
};

template<>
inline void Ship::RegisterDestruction(Point * /* element */)
{
    // Just tell the pointer container, he'll take care of it later
    mAllPoints.register_deletion();
}

template<>
inline void Ship::RegisterDestruction(Spring * /* element */)
{
	// Just tell the pointer container, he'll take care of it later
	mAllSprings.register_deletion();
}

template<>
inline void Ship::RegisterDestruction(Triangle * element)
{
    // Remove from set
    mAllTriangles.erase(element);
}

}