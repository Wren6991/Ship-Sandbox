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
#include <limits>
#include <queue>
#include <set>

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
	std::map<std::array<uint8_t, 3u>, Material const *> rgbColourMap;
	for (auto const & material: allMaterials)
        rgbColourMap[material->RgbColour] = material.get();

	//
	// Process image points and create points, springs, and triangles for this ship
	//

    Ship *ship = new Ship(parentWorld);

    std::vector<ElectricalElement*> shipElectricalElements;

    std::vector<Point*> shipPoints;
    shipPoints.reserve(structureImageWidth * structureImageHeight / 10);
    
    std::vector<Spring*> shipSprings;
    shipSprings.reserve(structureImageWidth * structureImageHeight / 10);

    std::vector<Triangle*> shipTriangles;
    shipTriangles.reserve(structureImageWidth * structureImageHeight / 10);

	size_t leakingPointsCount = 0;

	std::unique_ptr<std::unique_ptr<Point *[]>[]> pointMatrix(new std::unique_ptr<Point *[]>[structureImageWidth]);

	float const halfWidth = static_cast<float>(structureImageWidth) / 2.0f;

	for (int x = 0; x < structureImageWidth; ++x)
	{
        pointMatrix[x] = std::unique_ptr<Point *[]>(new Point *[structureImageHeight]);

        // From bottom to top
		for (int y = 0; y < structureImageHeight; ++y)
		{
			// R G B
            std::array<uint8_t, 3u> rgbColour = {
                structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 0],
                structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 1],
                structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 2]};

            auto srchIt = rgbColourMap.find(rgbColour);
			if (srchIt != rgbColourMap.end())
			{
                auto mtl = srchIt->second;

                //
                // Create point
                //

				Point * point = new Point(
					ship,
					vec2(static_cast<float>(x) - halfWidth, static_cast<float>(y)),
					mtl,
					mtl->IsHull ? 0.0f : 1.0f);  // no buoyancy if it's a hull section

                pointMatrix[x][y] = point;
                shipPoints.push_back(point);

                //
                // Create electrical element
                //

                if (!!mtl->Electrical)
                {
                    switch (mtl->Electrical->ElementType)
                    {
                        case Material::ElectricalProperties::ElectricalElementType::Cable:
                        {
                            shipElectricalElements.emplace_back(new Cable(ship, point));
                            break;
                        }

                        case Material::ElectricalProperties::ElectricalElementType::Generator:
                        {
                            shipElectricalElements.emplace_back(new Generator(ship, point));
                            break;
                        }

                        case Material::ElectricalProperties::ElectricalElementType::Lamp:
                        {
                            shipElectricalElements.emplace_back(new Lamp(ship, point));
                            break;
                        }
                    }
                }
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

						Material const * const mtl = b->GetMaterial()->IsHull ? a->GetMaterial() : b->GetMaterial();    // the spring is hull iff both nodes are hull; if not we use the non-hull material.

						Spring * spr = new Spring(
							ship,
							a,
							b,
							mtl);

						shipSprings.push_back(spr);

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
                                    ship,
                                    a,
                                    b,
                                    c);

                                shipTriangles.push_back(triangle);
							}
						}
					}
				}
			}
		}
	}

	ship->Initialize(
        std::move(shipElectricalElements),
        std::move(shipPoints),
        std::move(shipSprings),
        std::move(shipTriangles));

	return std::unique_ptr<Ship>(ship);
}

Ship::Ship(World * parentWorld)
    : mParentWorld(parentWorld)
    , mAllPoints()
    , mAllSprings()
    , mAllTriangles()
    , mScheduler()
{
}

Ship::~Ship()
{
    // Delete elements that are not unique_ptr's nor are kept
    // in a PointerContainer
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

    mAllPoints.shrink_to_fit();
    mAllSprings.shrink_to_fit();
    mAllTriangles.shrink_to_fit();
    mAllElectricalElements.shrink_to_fit();
}

void Ship::DrawTo(
    vec2f const & targetPos,
    float strength)
{
    // Attract all points to a single position
    for (Point * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            vec2f displacement = (targetPos - point->GetPosition());
            float forceMagnitude = strength / sqrtf(0.1f + displacement.length());
            point->ApplyForce(displacement.normalise() * forceMagnitude);
        }
    }
}

Point const * Ship::GetNearestPointAt(vec2 const & targetPos) const
{
    Point const * bestPoint = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (Point const * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            float distance = (point->GetPosition() - targetPos).length();
            if (distance < bestDistance)
            {
                bestPoint = point;
                bestDistance = distance;
            }
        }
    }

    return bestPoint;
}

void Ship::LeakWaterAndZeroLight(
	float dt,
	GameParameters const & gameParameters)
{
	// We use this point loop for everything that needs to be done on all points
	for (Point * point : mAllPoints)
	{
        if (!point->IsDeleted())
        {            
            // Stuff some water into all the leaking nodes that are underwater, if the external pressure is larger
            if (point->IsLeaking())
            {
                float waterLevel = mParentWorld->GetWaterHeight(
                    point->GetPosition().x, 
                    gameParameters);                

                float const externalWaterPressure = point->GetExternalWaterPressure(
                    waterLevel,
                    gameParameters);

                if (externalWaterPressure > point->GetWater())
                {
                    point->AdjustWater(dt * gameParameters.WaterPressureAdjustment * (externalWaterPressure - point->GetWater()));
                }
            }

            // Zero-out the quantity of light at this point
            point->ZeroLight();
        }
	}
}

void Ship::GravitateWater(
	float dt,
	GameParameters const & gameParameters)
{
    //
    // Water flows into adjacent nodes in a quantity proportional to the cos of angle the spring makes
    // against gravity (parallel with gravity => 1 (full flow), perpendicular = 0, parallel-opposite => -1 (goes back))
    //
    // Note: we don't take any shortcuts when a point has no water, as that would cause the speed of the 
    // simulation to change over time.
    //

    // Visit all connected non-hull points - i.e. non-hull springs
    for (Spring * spring : mAllSprings)
    {
        if (!spring->IsDeleted())
        {
            if (!spring->GetMaterial()->IsHull)
            {
                Point * const pointA = spring->GetPointA();
                assert(!pointA->IsDeleted());

                Point * const pointB = spring->GetPointB();
                assert(!pointB->IsDeleted());

                // cos_theta > 0 => pointA above pointB
                float cos_theta = (pointB->GetPosition() - pointA->GetPosition()).normalise().dot(gameParameters.GravityNormal);                
                
                // The 0.60 can be tuned, it's just to stop all the water being stuffed into the lowest node...
                float correction = 0.60f * cos_theta * dt * (cos_theta > 0.0f ? pointA->GetWater() : pointB->GetWater());
                pointA->AdjustWater(-correction);
                pointB->AdjustWater(correction);
            }
        }
    }
}

void Ship::BalancePressure(float dt)
{
    //
    // If there's too much water in this node, try and push it into the others
    // (This needs to iterate over multiple frames for pressure waves to spread through water)
    //
    // Note: we don't take any shortcuts when a point has no water, as that would cause the speed of the 
    // simulation to change over time.
    //

    // Visit all connected non-hull points - i.e. non-hull springs
    for (Spring * spring : mAllSprings)
    {
        if (!spring->IsDeleted())
        {
            if (!spring->GetMaterial()->IsHull)
            {
                Point * const pointA = spring->GetPointA();
                assert(!pointA->IsDeleted());
                float const aWater = pointA->GetWater();

                Point * const pointB = spring->GetPointB();
                assert(!pointB->IsDeleted());
                float const bWater = pointB->GetWater();

                if (aWater < 1 && bWater < 1)   // if water content below threshold, no need to force water out
                    continue;

                // Move water from more wet to less wet
                float correction = (bWater - aWater) * 2.5f * dt; // can tune this number; value of 1 means will equalise in 1 second.
                pointA->AdjustWater(correction);
                pointB->AdjustWater(-correction);
            }
        }
    }
}

void Ship::DiffuseLight(
    float dt,
    uint64_t currentStepSequenceNumber,
    GameParameters const & gameParameters)
{
    //
    // Diffuse light from each lamp to the closest adjacent (i.e. spring-connected) points,
    // inversely-proportional to the square of the distance
    //

    // We start at each lamp and do a graph flood from there, stopping at a maximum distance
    for (ElectricalElement * el : mAllElectricalElements)
    {
        if (!el->IsDeleted())
        {
            if (ElectricalElement::Type::Lamp == el->GetType())
            {
                Point * const lampPoint = el->GetPoint();
                assert(nullptr != lampPoint && !lampPoint->IsDeleted());

                // Set light on lamp's point
                // TBD: this needs to be based off the current at this lamp
                lampPoint->SetLight(1.0f);

                std::set<Point *> visitedPoints;
                visitedPoints.insert(lampPoint);

                std::queue<Point *> pointsToVisit;

                Point * currentPoint = lampPoint;
                while (true)
                {
                    // Go through this point's adjacents
                    for (Spring * spring : currentPoint->GetConnectedSprings())
                    {
                        if (!spring->IsDeleted())
                        {
                            Point * connectedPoint = nullptr;
                            if (0 == visitedPoints.count(spring->GetPointA()))
                            {
                                assert(1 == visitedPoints.count(spring->GetPointB()));
                                connectedPoint = spring->GetPointA();
                            }
                            else if (0 == visitedPoints.count(spring->GetPointB()))
                            {
                                connectedPoint = spring->GetPointB();
                            }

                            if (nullptr != connectedPoint)
                            {
                                visitedPoints.insert(connectedPoint);

                                // Calculate light to push into this point, and stop if it's too little
                                float squareDistance = (connectedPoint->GetPosition() - lampPoint->GetPosition()).squareLength();
                                assert(squareDistance != 0.0f);
                                float light = lampPoint->GetLight() / squareDistance;
                                if (light > 0.02)
                                {                              
                                    connectedPoint->SetLight(light);

                                    // Visit this point next
                                    pointsToVisit.push(connectedPoint);
                                }
                            }
                        }
                    }

                    //
                    // Get next point to visit
                    //

                    if (pointsToVisit.empty())
                    {
                        break;
                    }

                    currentPoint = pointsToVisit.front();
                    pointsToVisit.pop();
                }
            }
        }
    }
}

void Ship::Update(
	float dt,
    uint64_t currentStepSequenceNumber,
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


    //
    // Update springs dynamics
    //

	// Iterate the spring relaxation
	DoSprings(dt);

	// Check if any springs exceed their breaking strain
    size_t brokenSprings = 0u; // TBD: remove and replace with IGameSink call(material) 
	for (Spring * spring : mAllSprings)
	{
		if (!spring->IsDeleted())
		{
			if (spring->IsBroken(gameParameters.StrengthAdjustment))
			{
				spring->Destroy();

                ++brokenSprings;
			}
		}
	}


    //
	// Update all electrical and water stuff
    //

    LeakWaterAndZeroLight(dt, gameParameters);

    for (int i = 0; i < 4; i++)
        BalancePressure(dt);

	for (int i = 0; i < 4; i++)
	{
        BalancePressure(dt);
		GravitateWater(dt, gameParameters);
	}

    DiffuseLight(
        dt, 
        currentStepSequenceNumber,
        gameParameters);

    //
    // Clear up pointer containers, in case there have been deletions
    // during or before this update step
    //

    mAllPoints.shrink_to_fit();
    mAllSprings.shrink_to_fit();
    mAllTriangles.shrink_to_fit();

    if (brokenSprings > 0)
    {
        LogDebug("Broken springs: ", brokenSprings);
    }
}


void Ship::Render(
	GameParameters const & gameParameters,
	RenderParameters const & renderParameters) const
{
    if (renderParameters.DrawPointsOnly)
    {
        glBegin(GL_POINTS);

        for (Point const * point : mAllPoints)
        {
            RenderUtils::SetColour(point->GetColour(point->GetMaterial()->Colour));
            glVertex3f(point->GetPosition().x, point->GetPosition().y, -1);
        }

        glEnd();

        return;
    }

	// Draw all the springs
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
    float const dampingamount = (1 - powf(0.0f, static_cast<float>(dt))) * 0.5f;

	if (mAllSprings.empty())
	{
		// Nothing to do!
		return;
	}

	// Calculate number of parallel chunks and size of each chunk
	size_t nParallelChunks = mScheduler.GetNumberOfThreads();
    size_t parallelChunkSize = std::max((mAllSprings.size() / nParallelChunks), (size_t)1);

	assert(parallelChunkSize > 0);

    // Run iterations
	for (int outiter = 0; outiter < 3; outiter++)
	{
		for (int iteration = 0; iteration < 8; iteration++)
		{
            for (size_t i = 0; i < mAllSprings.size(); /* incremented in loop */)
            {
                size_t available = mAllSprings.size() - i;
                size_t thisChunkSize = (available >= parallelChunkSize) ? parallelChunkSize : available;
                assert(thisChunkSize > 0);

                mScheduler.Schedule(
                    new SpringCalculateTask(
                        this,
                        i,
                        i + thisChunkSize));

                i += thisChunkSize;
            }
			
            mScheduler.WaitForAllTasks();
		}
		
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
    for (size_t i = mStartSpringIndex; i < mEndSpringIndex; ++i)
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
            point->AddToPosition(point->GetForce() * mDt);
            point->ZeroForce();
        }
	}
}

}