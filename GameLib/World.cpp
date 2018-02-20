/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include <algorithm>
#include <cassert>
#include <random>

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
    , mCurrentStepSequenceNumber(0u)
    , mCollisionTree(BVHNode::AllocateTree())
{
    std::seed_seq seed_seq({1, 242, 19730528});
    mRandomEngine = std::ranlux48_base(seed_seq);
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

Point const * World::GetNearestPointAt(
    vec2 const & targetPos,
    float radius) const
{
    Point const * bestPoint = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto const & ship : mAllShips)
    {
        Point const * shipBestPoint = ship->GetNearestPointAt(targetPos, radius);
        if (nullptr != shipBestPoint)
        {
            float distance = (shipBestPoint->GetPosition() - targetPos).length();
            if (distance < bestDistance)
            {
                bestPoint = shipBestPoint;
                bestDistance = distance;
            }
        }
    }

    return bestPoint;
}

void World::Update(
    float dt,
    GameParameters const & gameParameters)
{
    // Update current time
    mCurrentTime += dt;

    // Generate a new step sequence number
    ++mCurrentStepSequenceNumber;
    if (0u == mCurrentStepSequenceNumber)
        mCurrentStepSequenceNumber = 1u;

    // Update all ships
    for (auto & ship : mAllShips)
    {
        ship->Update(
            dt,
            mCurrentStepSequenceNumber,
            gameParameters);
    }

    //buildBVHTree(true, points, collisionTree);

    // Resize clouds vector
    if (gameParameters.NumberOfClouds < mAllClouds.size())
    {
        mAllClouds.resize(gameParameters.NumberOfClouds);
    }
    else
    {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        for (size_t c = mAllClouds.size(); c < gameParameters.NumberOfClouds; ++c)
        {
            mAllClouds.emplace_back(
                dis(mRandomEngine) * 100.0f,    // OffsetX
                // TODO
                //dis(mRandomEngine) * 0.3f,      // SpeedX1
                dis(mRandomEngine) * 2.0f,      // SpeedX1
                dis(mRandomEngine) * 0.01f,     // AmpX
                dis(mRandomEngine) * 0.01f,     // SpeedX2
                dis(mRandomEngine) * 100.0f,    // OffsetY
                dis(mRandomEngine) * 0.001f,    // AmpY
                dis(mRandomEngine) * 0.01f,     // SpeedY
                dis(mRandomEngine) * 0.05f,     // AmpScale
                dis(mRandomEngine) * 0.01f);    // SpeedScale
        }
    }
}

void World::Render(	
    GameParameters const & gameParameters,
    RenderContext & renderContext) const
{
    renderContext.RenderStart();

    // Render the clouds
    RenderClouds(gameParameters, renderContext);

    // Render the ocean floor
    RenderLand(gameParameters, renderContext);

    // Render the water now, if we want to see the ship through the water
    if (renderContext.GetShowShipThroughWater())
        RenderWater(gameParameters, renderContext);

    // Render all ships
    for (auto const & ship : mAllShips)
    {
        ship->Render(
            gameParameters,
            renderContext);
    }

    // Render the water now, if we want to see the ship *in* the water instead
    if (!renderContext.GetShowShipThroughWater())
        RenderWater(gameParameters, renderContext);

    renderContext.RenderEnd();
}

///////////////////////////////////////////////////////////////////////////////////
// Private Helpers
///////////////////////////////////////////////////////////////////////////////////

void World::RenderClouds(
    GameParameters const & gameParameters,
    RenderContext & renderContext) const
{
    renderContext.RenderCloudsStart(mAllClouds.size());

    for (Cloud const & cloud : mAllClouds)
    {
        vec3f cloudValues = cloud.CalculatePosAndScale(mCurrentTime);

        renderContext.RenderCloud(
            cloudValues.x,
            cloudValues.y,
            cloudValues.z);
    }

    renderContext.RenderCloudsEnd();
}

void World::RenderLand(
    GameParameters const & gameParameters,
    RenderContext & renderContext) const
{
    static constexpr size_t SlicesCount = 300;

    float const worldWidth = renderContext.GetVisibleWorldSize().x;

    renderContext.RenderLandStart(SlicesCount);    

    float const sliceWidth = worldWidth / static_cast<float>(SlicesCount);
    float sliceX = renderContext.GetCameraWorldPosition().x - (worldWidth / 2.0f);
    for (size_t i = 0; i <= SlicesCount; ++i, sliceX += sliceWidth)
    {
        renderContext.RenderLand(
            sliceX,
            GetOceanFloorHeight(sliceX, gameParameters));
    }

    renderContext.RenderLandEnd();
}

void World::RenderWater(
    GameParameters const & gameParameters,
    RenderContext &  renderContext) const
{
    static constexpr size_t SlicesCount = 300;

    float const worldWidth = renderContext.GetVisibleWorldSize().x;
    
    renderContext.RenderWaterStart(SlicesCount);

    float const sliceWidth = worldWidth / static_cast<float>(SlicesCount);
    float sliceX = renderContext.GetCameraWorldPosition().x - (worldWidth / 2.0f);
    for (size_t i = 0; i <= SlicesCount; ++i, sliceX += sliceWidth)
    {
        renderContext.RenderWater(
            sliceX,
            GetOceanFloorHeight(sliceX, gameParameters),
            GetWaterHeight(sliceX, gameParameters),
            gameParameters.SeaDepth);
    }

    renderContext.RenderWaterEnd();
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