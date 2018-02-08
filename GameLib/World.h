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
#include "Vectors.h"

#include <memory>
#include <set>
#include <vector>

namespace Physics
{

class World
{
public:

	World();

	float GetWaterHeight(		
		float x,
		GameParameters const & gameParameters) const;

	float GetOceanFloorHeight(
		float x,
		GameParameters const & gameParameters) const;

	void AddShip(std::unique_ptr<Ship> && newShip);

	void DestroyAt(
		vec2 const & targetPos, 
		float radius);

	void DrawTo(
        vec2 const & targetPos,
        float strength);

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

private:

	// Repository
	std::vector<std::unique_ptr<Ship>> mAllShips;

	// The current time
	float mCurrentTime;

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
