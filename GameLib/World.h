/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "AABB.h"
#include "GameParameters.h"
#include "IGameEventHandler.h"
#include "Material.h"
#include "Physics.h"
#include "RenderContext.h"
#include "Vectors.h"

#include <cstdint>
#include <memory>
#include <random>
#include <set>
#include <vector>

namespace Physics
{

class World
{
public:

	World(std::shared_ptr<IGameEventHandler> gameEventHandler);

	float GetWaterHeight(		
		float x,
		GameParameters const & gameParameters) const;

    bool IsUnderwater(
        Point const & point,
        GameParameters const & gameParameters) const;
	
    float GetOceanFloorHeight(
		float x,
		GameParameters const & gameParameters) const;

    IGameEventHandler * GetGameEventHandler()
    {
        return mGameEventHandler.get();
    }

	void AddShip(std::unique_ptr<Ship> && newShip);

    unsigned int GetNextShipId()
    {
        // TODO: refactor
        return static_cast<int>(mAllShips.size());
    }

	void DestroyAt(
		vec2 const & targetPos, 
		float radius,
        GameParameters const & gameParameters);

	void DrawTo(
        vec2 const & targetPos,
        float strength);

    Point const * GetNearestPointAt(
        vec2 const & targetPos,
        float radius) const;

	void Update(
		float dt,
		GameParameters const & gameParameters);

	void Render(		
        GameParameters const & gameParameters,
		RenderContext & renderContext) const;

private:

    void RenderClouds(
        GameParameters const & gameParameters,
        RenderContext & renderContext) const;

	void UploadLandAndWater(
		GameParameters const & gameParameters,
        RenderContext & renderContext) const;

private:

	// Repository
	std::vector<std::unique_ptr<Ship>> mAllShips;
    std::vector<Cloud> mAllClouds;

	// The current time 
	float mCurrentTime;

    // The current step sequence number; used to avoid zero-ing out things.
    // Guaranteed to never be zero, but expected to rollover
    uint64_t mCurrentStepSequenceNumber;

    // Our random generator
    std::ranlux48_base mRandomEngine;

    // The game event handler
    std::shared_ptr<IGameEventHandler> mGameEventHandler;

private:

	//
	// TODO: experimental
	//

    // float const *oceandepthbuffer;

	struct BVHNode
	{
		AABB volume;
		BVHNode *l, *r;
		bool isLeaf;
		size_t pointCount;
		static const int MAX_DEPTH = 15;
		static const int MAX_N_POINTS = 10;
		Point* points[MAX_N_POINTS];
		static BVHNode * AllocateTree(int depth = MAX_DEPTH);
	};

	void BuildBVHTree(bool splitInX, std::vector<Point*> &pointlist, BVHNode *thisnode, int depth = 1);

	BVHNode * mCollisionTree;
};

}
