/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "GameOpenGL.h"
#include "Log.h"
#include "RenderUtils.h"

#include <algorithm>
#include <cassert>
// TODO
//#include <cmath>

namespace Physics {

//   SSS    H     H  IIIIIII  PPPP
// SS   SS  H     H     I     P   PP
// S        H     H     I     P    PP
// SS       H     H     I     P   PP
//   SSS    HHHHHHH     I     PPPP
//      SS  H     H     I     P
//       S  H     H     I     P
// SS   SS  H     H     I     P
//   SSS    H     H  IIIIIII  P

std::unique_ptr<Ship> Ship::Create(
	World * parentWorld,
	unsigned char const * structureImageData,
	int structureImageWidth,
	int structureImageHeight,
	std::vector<std::unique_ptr<Material const>> const & allMaterials)
{
	// Prepare materials dictionary
	std::map<vec3f, Material const *> colourdict;
	for (auto const & material: allMaterials)
		colourdict[material->Colour] = material.get();

	//
	// Process image points and create points, springs, and triangles for this ship
	//

    Ship *shp = new Ship(parentWorld);

    std::vector<Point*> shipPoints;
    shipPoints.reserve(structureImageWidth * structureImageHeight / 10);
    
    std::vector<Spring*> shipSprings;
    shipSprings.reserve(structureImageWidth * structureImageHeight / 10);

    std::set<Triangle*> shipTriangles;

	size_t leakingPointsCount = 0;

	std::unique_ptr<std::unique_ptr<Point *[]>[]> pointMatrix(new std::unique_ptr<Point *[]>[structureImageWidth]);

	float const halfWidth = static_cast<float>(structureImageWidth) / 2.0f;

	for (int x = 0; x < structureImageWidth; ++x)
	{
        pointMatrix[x] = std::unique_ptr<Point *[]>(new Point *[structureImageHeight]);

		for (int y = 0; y < structureImageHeight; ++y)
		{
			// R G B
			vec3f colour(
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 0] / 255.f,
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 1] / 255.f,
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 2] / 255.f);

			if (colourdict.find(colour) != colourdict.end())
			{
				auto mtl = colourdict[colour];

				Point * pt = new Point(
					shp,
					vec2(static_cast<float>(x) - halfWidth, static_cast<float>(y)),
					mtl,
					mtl->IsHull ? 0.0f : 1.0f);  // no buoyancy if it's a hull section

                pointMatrix[x][y] = pt;
                shipPoints.push_back(pt);
			}
			else
			{
                pointMatrix[x][y] = nullptr;
			}
		}
	}

	// Points have been generated, so fill in all the beams between them.
	// If beam joins two hull nodes, it is a hull beam.
	// If a non-hull node has empty space on one of its four sides, it is automatically leaking.

	static const int Directions[8][2] = {
		{ 1,  0 },	// E
		{ 1, -1 },	// NE
		{ 0, -1 },	// N
		{ -1, -1 },	// NW
		{ -1,  0 },	// W
		{ -1,  1 },	// SW
		{ 0,  1 },	// S
		{ 1,  1 }	// SE
	};

	for (int x = 0; x < structureImageWidth; ++x)
	{
		for (int y = 0; y < structureImageHeight; ++y)
		{
			Point * a = pointMatrix[x][y];
			if (nullptr == a)
				continue;

			bool aIsHull = a->GetMaterial()->IsHull;

			// Check if a is leaking; a is leaking if:
			// - a is not hull, AND
			// - there is at least a hole at E, S, W, N
			if (!aIsHull)
			{
				if ((x < structureImageWidth - 1 && !pointMatrix[x + 1][y])
					|| (y < structureImageHeight - 1 && !pointMatrix[x][y + 1])
					|| (x > 0 && !pointMatrix[x - 1][y])
					|| (y > 0 && !pointMatrix[x][y - 1]))
				{
					a->SetLeaking();

					++leakingPointsCount;
				}
			}

			// First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y),
			// i.e. E, NE, N, NW - this covers each pair of points in each direction
			for (int i = 0; i < 4; ++i)
			{
				int adjx1 = x + Directions[i][0];
				int adjy1 = y + Directions[i][1];
				// Valid coordinates?
				if (adjx1 >= 0 && adjx1 < structureImageWidth && adjy1 >= 0)
				{
					assert(adjy1 < structureImageHeight); // The four directions we're checking do not include S

					Point * b = pointMatrix[adjx1][adjy1]; // adjacent point in direction (i)				
					if (nullptr != b)
					{
						// b is adjacent to a at one of E, NE, N, NW						

						//
						// Create a<->b spring
						// 

						bool springIsHull = aIsHull && b->GetMaterial()->IsHull;
						Material const * const mtl = b->GetMaterial()->IsHull ? a->GetMaterial() : b->GetMaterial();    // the spring is hull iff both nodes are hull; if not we use the non-hull material.

						Spring * spr = new Spring(
							shp,
							a,
							b,
							mtl);

						shipSprings.push_back(spr);

						if (!springIsHull)
						{
							shp->mAdjacentNonHullPoints[a].insert(b);
							shp->mAdjacentNonHullPoints[b].insert(a);
						}

						// Check adjacent point in next CW direction (for constructing triangles)
						int adjx2 = x + Directions[i + 1][0];
						int adjy2 = y + Directions[i + 1][1];
						// Valid coordinates?
						if (adjx2 >= 0 && adjx2 < structureImageWidth && adjy2 >= 0)
						{
							assert(adjy2 < structureImageHeight); // The five directions we're checking do not include S

							Point *c = pointMatrix[adjx2][adjy2];
							if (nullptr != c)
							{
                                Triangle * triangle = new Triangle(
                                    shp,
                                    a,
                                    b,
                                    c);

                                shipTriangles.insert(triangle);
							}
						}
					}
				}
			}
		}
	}

	LogMessage(L"Created ship: W=", structureImageWidth, L", H=", structureImageHeight, ", ",
		shipPoints.size(), L" points, of which ", leakingPointsCount, " leaking, ", shipSprings.size(),
		L" springs, ", shipTriangles.size(), L" triangles, and ", shp->mAdjacentNonHullPoints.size(), " adjacency entries.");

	shp->Initialize(
        std::move(shipPoints),
        std::move(shipSprings),
        std::move(shipTriangles));

	return std::unique_ptr<Ship>(shp);
}

Ship::Ship(World * parentWorld)
    : mParentWorld(parentWorld)
    , mAllPoints()
    , mAllSprings()
    , mAllTriangles()
    , mAdjacentNonHullPoints()
    , mScheduler()
{
}

Ship::~Ship()
{
    // Delete elements that are not unique_ptr's nor are kept
    // in a PointerContainer

    for (Triangle * triangle : mAllTriangles)
    {
        delete triangle;
    }
}

void Ship::DestroyAt(vec2 const & targetPos, float radius)
{
    // Destroy all points within the radius
    for (Point * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            if ((point->GetPosition() - targetPos).length() < radius)
            {
                point->Destroy();
            }
        }
    }
}

void Ship::DrawTo(vec2f const & targetPos)
{
    // Attract all points to a single position
    for (Point * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            vec2f displacement = (targetPos - point->GetPosition());
            float forceMagnitude = 50000.0f / sqrtf(0.1f + displacement.length());
            point->ApplyForce(displacement.normalise() * forceMagnitude);
        }
    }
}

void Ship::LeakWater(
	float dt,
	GameParameters const & gameParameters)
{
	// Stuff some water into all the leaking nodes, if they're not under too much pressure
	for (Point * point : mAllPoints)
	{
        if (!point->IsDeleted())
        {            
            if (point->IsLeaking()
                && point->GetPosition().y < mParentWorld->GetWaterHeight(point->GetPosition().x, gameParameters) // point is in water
                && point->GetWater() < 1.5f)	// Water pressure not already excedent // TBD: should 1.5 be 'pressure'?
            {
                float const pressure = point->GetPressure(gameParameters.GravityMagnitude);
                point->AdjustWater(dt * gameParameters.WaterPressureAdjustment * (pressure - point->GetWater()));
            }
        }
	}
}

void Ship::GravitateWater(
	float dt,
	GameParameters const & gameParameters)
{
	// Water flows into adjacent nodes in a quantity proportional to the cos of angle the beam makes
	// against gravity (parallel with gravity => 1 (full flow), perpendicular = 0)
	for (std::map<Point *, std::set<Point*> >::iterator iter = mAdjacentNonHullPoints.begin();
		iter != mAdjacentNonHullPoints.end(); iter++)
	{
		Point *a = iter->first;
		for (std::set<Point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
		{
			Point *b = *second;
			float cos_theta = (b->GetPosition() - a->GetPosition()).normalise().dot(gameParameters.GravityNormal);
			if (cos_theta > 0.0f)
			{
				float correction = std::min(0.5f * cos_theta * dt, a->GetWater());   // The 0.5 can be tuned, it's just to stop all the water being stuffed into the first node...
				a->AdjustWater(-correction);
				b->AdjustWater(correction);
			}
		}
	}
}

void Ship::BalancePressure(float dt)
{
	// If there's too much water in this node, try and push it into the others
	// (This needs to iterate over multiple frames for pressure waves to spread through water)
	for (std::map<Point*, std::set<Point*> >::iterator iter = mAdjacentNonHullPoints.begin();
		iter != mAdjacentNonHullPoints.end(); iter++)
	{
		Point *a = iter->first;
		if (a->GetWater() < 1)   // if water content is not above threshold, no need to force water out
			continue;

		for (std::set<Point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
		{
			Point *b = *second;
			float correction = (b->GetWater() - a->GetWater()) * 8.0f * dt; // can tune this number; value of 1 means will equalise in 1 second.
			a->AdjustWater(correction);
			b->AdjustWater(-correction);
		}
	}
}

void Ship::Update(
	float dt,
	GameParameters const & gameParameters)
{
	// Advance simulation for points (velocity and forces)
    for (Point * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            point->Update(dt, gameParameters);
        }
    }

	// Iterate the spring relaxation
	DoSprings(dt);

	// Check if any springs exceed their breaking strain
	for (Spring * spring : mAllSprings)
	{
		if (!spring->IsDeleted())
		{
			if (spring->IsBroken(gameParameters.StrengthAdjustment))
			{
				spring->Destroy();
			}
		}
	}


    //
	// Update all water stuff
    //

	LeakWater(dt, gameParameters);
	for (int i = 0; i < 4; i++)
	{
		GravitateWater(dt, gameParameters);
		BalancePressure(dt);
	}

	for (int i = 0; i < 4; i++)
		BalancePressure(dt);


    //
    // Clear up pointer containers, in case there have been deletions
    // during or before this update step
    //

    mAllPoints.shrink_to_fit();
    mAllSprings.shrink_to_fit();

    // The triangle container does not contain deleted items
    assert(mAllTriangles.end() == std::find_if(
        mAllTriangles.begin(),
        mAllTriangles.end(),
        [](Triangle * t)
        {
            return nullptr == t || t->IsDeleted();
        }));
}


void Ship::Render(
	GameParameters const & gameParameters,
	RenderParameters const & renderParameters) const
{
	// Draw all the points and springs
    for (Point const * point : mAllPoints)
    {
        assert(!point->IsDeleted());
        point->Render();
    }

    for (Spring const * spring : mAllSprings)
    {
        assert(!spring->IsDeleted());
        spring->Render(false);
    }

	if (!renderParameters.UseXRayMode)
	{
		// Render triangles
		for (Triangle const * triangle : mAllTriangles)
		{
            assert(!triangle->IsDeleted());
			triangle->Render();
		}
	}

    if (renderParameters.ShowStress)
    {
        for (Spring const * spring : mAllSprings)
        {
            assert(!spring->IsDeleted());
            if (spring->IsStressed(gameParameters.StrengthAdjustment))
            {
                spring->Render(true);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
// Private Helpers
///////////////////////////////////////////////////////////////////////////////////

void Ship::DoSprings(float dt)
{
	if (mAllSprings.empty())
	{
		// Nothing to do!
		return;
	}

	// Calculate number of parallel chunks and size of each chunk
	size_t nParallelChunks = mScheduler.GetNumberOfThreads();
    size_t parallelChunkSize = std::max((mAllSprings.size() / nParallelChunks), (size_t)1);

	assert(parallelChunkSize > 0);

	for (int outiter = 0; outiter < 3; outiter++)
	{
		for (int iteration = 0; iteration < 8; iteration++)
		{
			for (size_t i = mAllSprings.size() - 1; i > 0; /* decremented in loop */)
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
        for (Spring * spring : mAllSprings)
        {
            if (!spring->IsDeleted())
            {
                spring->DoDamping(dampingamount);
            }
        }
	}
}

void Ship::SpringCalculateTask::Process()
{
    for (size_t i = mFirstSpringIndex; i <= mLastSpringIndex; ++i)
    {
        Spring * const spring = mParentShip->mAllSprings[i];
        if (!spring->IsDeleted())
        {
            spring->Update();
        }
    }
}

void Ship::PointIntegrateTask::Process()
{
	for (size_t i = mFirstPointIndex; i <= mLastPointIndex; ++i)
	{
        Point * const point = mParentShip->mAllPoints[i];
        if (!point->IsDeleted())
        {
            point->AdjustPosition(point->GetForce() * mDt);
            point->ZeroForce();
        }
	}
}

}