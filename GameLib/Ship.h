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

	auto const & GetPoints() const { return mPoints; }
	auto const & GetPointAdjencyMap() const { return mAdjacentnodes; }
	auto const & GetSprings() const { return mSprings; }
	auto const & GetTriangles() const { return mTriangles; }

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

	void Render() const;

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

	Ship(World * parentWorld)
		: mParentWorld(parentWorld)
	{
	}

	World * const mParentWorld;
	std::set<Point*> mPoints;
	std::set<Spring*> mSprings;
	std::map<Point *, std::set<Point*>> mAdjacentnodes;
	std::set<Triangle*> mTriangles;
};

template<>
void Ship::RegisterDestruction(Point * element)
{
	// TODO
}

template<>
void Ship::RegisterDestruction(Spring * element)
{
	// TODO
}

}