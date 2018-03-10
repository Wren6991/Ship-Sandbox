/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "Log.h"

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
	std::map<std::array<uint8_t, 3u>, Material const *> structuralColourMap;
    for (auto const & material : allMaterials)
    {
        structuralColourMap[material->StructuralColourRgb] = material.get();
    }

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

            auto srchIt = structuralColourMap.find(rgbColour);
			if (srchIt != structuralColourMap.end())
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
    : mId(parentWorld->GenerateNewShipId())
    , mParentWorld(parentWorld)    
    , mAllPoints()
    , mAllSprings()
    , mAllTriangles()
    , mScheduler()
    , mConnectedComponentSizes()
    , mIsSinking(false)
    , mTotalWater(0.0)
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
                // Notify
                mParentWorld->GetGameEventHandler()->OnDestroy(point->GetMaterial(), 1u);

                // Destroy point
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

Point const * Ship::GetNearestPointAt(
    vec2 const & targetPos, 
    float radius) const
{
    Point const * bestPoint = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (Point const * point : mAllPoints)
    {
        if (!point->IsDeleted())
        {
            float distance = (point->GetPosition() - targetPos).length();
            if (distance < radius && distance < bestDistance)
            {
                bestPoint = point;
                bestDistance = distance;
            }
        }
    }

    return bestPoint;
}

void Ship::PreparePointsForFinalStep(
	float dt,
    uint64_t currentStepSequenceNumber,
	GameParameters const & gameParameters)
{
    //
	// We use this point loop for everything that needs to be done on all points
    //

    uint64_t currentConnectedComponentId = 0;
    std::queue<Point *> pointsToVisitForConnectedComponents;
    mConnectedComponentSizes.clear();

	for (Point * point : mAllPoints)
	{
        assert(!point->IsDeleted());
        
        //
        // 1) Leak water: stuff some water into all the leaking nodes that are underwater, 
        //    if the external pressure is larger
        //

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
                float newWater = dt * gameParameters.WaterPressureAdjustment * (externalWaterPressure - point->GetWater());
                point->AddWater(newWater);
                mTotalWater += newWater;
            }
        }

        //
        // 2) Detect connected components
        //

        // Check if visited
        if (point->GetCurrentConnectedComponentDetectionStepSequenceNumber() != currentStepSequenceNumber)
        {
            // This node has not been visited, hence it's the beginning of a new connected component
            ++currentConnectedComponentId;
            size_t pointsInCurrentConnectedComponent = 0;

            //
            // Propagate the connected component ID to all points reachable from this point
            //

            assert(pointsToVisitForConnectedComponents.empty());
            pointsToVisitForConnectedComponents.push(point);
            point->SetConnectedComponentDetectionStepSequenceNumber(currentStepSequenceNumber);

            while (!pointsToVisitForConnectedComponents.empty())
            {
                Point * currentPoint = pointsToVisitForConnectedComponents.front();
                pointsToVisitForConnectedComponents.pop();

                // Assign the connected component ID
                currentPoint->SetConnectedComponentId(currentConnectedComponentId);
                ++pointsInCurrentConnectedComponent;

                // Go through this point's adjacents
                for (Spring * spring : currentPoint->GetConnectedSprings())
                {
                    assert(!spring->IsDeleted());

                    Point * const pointA = spring->GetPointA();
                    assert(!pointA->IsDeleted());
                    if (pointA->GetCurrentConnectedComponentDetectionStepSequenceNumber() != currentStepSequenceNumber)
                    {
                        pointA->SetConnectedComponentDetectionStepSequenceNumber(currentStepSequenceNumber);
                        pointsToVisitForConnectedComponents.push(pointA);
                    }

                    Point * const pointB = spring->GetPointB();
                    assert(!pointB->IsDeleted());
                    if (pointB->GetCurrentConnectedComponentDetectionStepSequenceNumber() != currentStepSequenceNumber)
                    {
                        pointB->SetConnectedComponentDetectionStepSequenceNumber(currentStepSequenceNumber);
                        pointsToVisitForConnectedComponents.push(pointB);
                    }
                }
            }

            // Store number of connected components
            mConnectedComponentSizes.push_back(pointsInCurrentConnectedComponent);
        }
	}

    //
    // Check whether we've started sinking
    //

    if (!mIsSinking
        && mTotalWater > static_cast<float>(mAllPoints.size()) / 3.0f)
    {
        // Started sinking
        mParentWorld->GetGameEventHandler()->OnSinkingBegin(mId);
        mIsSinking = true;
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
        assert(!spring->IsDeleted());
        
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
            pointA->AddWater(-correction);
            pointB->AddWater(correction);
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
        assert(!spring->IsDeleted());
        
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
            pointA->AddWater(correction);
            pointB->AddWater(-correction);
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

    // Greater adjustment => underrated distance => wider diffusion
    float const adjustmentCoefficient = powf(1.0f - gameParameters.LightDiffusionAdjustment, 2.0f);

    // Visit all points
    for (Point * point : mAllPoints)
    {
        assert(!point->IsDeleted());

        point->ZeroLight();

        vec2f const & pointPosition = point->GetPosition();

        // Go through all lamps in the same connected component
        for (ElectricalElement * el : mAllElectricalElements)
        {
            assert(!el->IsDeleted());

            if (ElectricalElement::Type::Lamp == el->GetType()
                && el->GetPoint()->GetConnectedComponentId() == point->GetConnectedComponentId())
            {
                Point * const lampPoint = el->GetPoint();

                assert(!lampPoint->IsDeleted());

                // TODO: this needs to be replaced with getting the light from the lamp itself
                float const lampLight = 1.0f;

                float squareDistance = std::max(
                    1.0f, 
                    (pointPosition - lampPoint->GetPosition()).squareLength() * adjustmentCoefficient);

                assert(squareDistance >= 1.0f);

                point->AdjustLight(lampLight / squareDistance);
            }
        }
    }
}

void Ship::Update(
	float dt,
    uint64_t currentStepSequenceNumber,
	GameParameters const & gameParameters)
{
    IGameEventHandler * const gameEventHandler = mParentWorld->GetGameEventHandler();

    //
	// Advance simulation for points (velocity and forces)
    //

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

	// Update tension strain for all springs; might cause springs to break
	for (Spring * spring : mAllSprings)
	{
		if (!spring->IsDeleted())
		{
            spring->UpdateTensionStrain(gameParameters.StrengthAdjustment, gameEventHandler);
		}
	}

    //
    // Clear up pointer containers, in case there have been deletions
    // during or before this update step
    //

    mAllPoints.shrink_to_fit();
    mAllSprings.shrink_to_fit();
    mAllTriangles.shrink_to_fit();


    //
	// Update all electrical and water stuff
    //

    PreparePointsForFinalStep(dt, currentStepSequenceNumber, gameParameters);

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
}

void Ship::Render(
	GameParameters const & gameParameters,
	RenderContext & renderContext) const
{
    //
    // Upload points
    //

    renderContext.UploadShipPointStart(mAllPoints.size());

    int todoElementIndex = 0; // Temporary - we need to assign an index now as points get deleted over time
    for (Point const * point : mAllPoints)
    {
        assert(!point->IsDeleted());

        auto pointColour = point->GetColour(
            point->GetMaterial()->RenderColour,
            renderContext.GetAmbientLightIntensity());

        renderContext.UploadShipPoint(
            point->GetPosition().x,
            point->GetPosition().y,
            pointColour.x,
            pointColour.y,
            pointColour.z);

        Point * todoPoint = const_cast<Point *>(point);
        todoPoint->TodoElementIndex = todoElementIndex++;
    }

    renderContext.UploadShipPointEnd();

    if (renderContext.GetDrawPointsOnly())
    {
        // Draw just the points
        renderContext.RenderShipPoints();
    }
    else
    {
        //
        // Render all the springs
        //

        renderContext.RenderSpringsStart(mAllSprings.size());

        for (Spring const * spring : mAllSprings)
        {
            assert(!spring->IsDeleted());

            renderContext.RenderSpring(
                spring->GetPointA()->TodoElementIndex,
                spring->GetPointB()->TodoElementIndex);
        }

        renderContext.RenderSpringsEnd();


        //
        // Render all triangles
        //

        if (!renderContext.GetUseXRayMode())
        {
            renderContext.RenderShipTrianglesStart(mAllTriangles.size());
            
            for (Triangle const * triangle : mAllTriangles)
            {
                assert(!triangle->IsDeleted());

                renderContext.RenderShipTriangle(
                    triangle->GetPointA()->TodoElementIndex,
                    triangle->GetPointB()->TodoElementIndex,
                    triangle->GetPointC()->TodoElementIndex);
            }

            renderContext.RenderShipTrianglesEnd();
        }

        if (renderContext.GetShowStress())
        {
            renderContext.RenderStressedSpringsStart(mAllSprings.size());

            for (Spring const * spring : mAllSprings)
            {
                assert(!spring->IsDeleted());

                if (spring->IsStressed())
                {
                    renderContext.RenderStressedSpring(
                        spring->GetPointA()->TodoElementIndex,
                        spring->GetPointB()->TodoElementIndex);
                }
            }

            renderContext.RenderStressedSpringsEnd();
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