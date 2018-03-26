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
    ShipDefinition const & shipDefinition,
    std::vector<std::unique_ptr<Material const>> const & allMaterials)
{
    // Prepare materials dictionary
    std::map<std::array<uint8_t, 3u>, Material const *> structuralColourMap;
    for (auto const & material : allMaterials)
    {
        structuralColourMap[material->StructuralColourRgb] = material.get();
    }


    //
    // 1. Process image points and create matrix of point indices and materials
    //    

    struct PointInfo
    {
        Material const * Mtl;
        size_t PointIndex;

        PointInfo(
            Material const * mtl,
            size_t pointIndex)
            : Mtl(mtl)
            , PointIndex(pointIndex)
        {
        }
    };

    int const structureWidth = shipDefinition.StructuralImage.Width;
    float const halfWidth = static_cast<float>(structureWidth) / 2.0f;
    int const structureHeight = shipDefinition.StructuralImage.Height;

    std::unique_ptr<std::unique_ptr<std::optional<PointInfo>[]>[]> pointInfoMatrix(new std::unique_ptr<std::optional<PointInfo>[]>[structureWidth]);
    
    size_t pointCount = 0;
    for (int x = 0; x < structureWidth; ++x)
    {
        pointInfoMatrix[x] = std::unique_ptr<std::optional<PointInfo>[]>(new std::optional<PointInfo>[structureHeight]);

        // From bottom to top
        for (int y = 0; y < structureHeight; ++y)
        {
            // R G B
            std::array<uint8_t, 3u> rgbColour = {
                shipDefinition.StructuralImage.Data[(x + (structureHeight - y - 1) * structureWidth) * 3 + 0],
                shipDefinition.StructuralImage.Data[(x + (structureHeight - y - 1) * structureWidth) * 3 + 1],
                shipDefinition.StructuralImage.Data[(x + (structureHeight - y - 1) * structureWidth) * 3 + 2] };

            auto srchIt = structuralColourMap.find(rgbColour);
            if (srchIt != structuralColourMap.end())
            {
                // This will be a point
                pointInfoMatrix[x][y] = PointInfo(srchIt->second, pointCount);

                ++pointCount;
            }
        }
    }


    //
    // 2. Create:
    //  - Points
    //  - PointColors
    //  - PointTextureCoordinates
    //  - SpringInfo
    //  - TriangleInfo
    //

    Ship *ship = new Ship(parentWorld);

    ElementRepository<Point> allPoints(pointCount);
    ElementRepository<vec3f> allPointColors(pointCount);
    ElementRepository<vec2f> allPointTextureCoordinates(pointCount);

    size_t leakingPointsCount = 0;

    struct SpringInfo
    {
        size_t PointAIndex;
        size_t PointBIndex;

        SpringInfo(
            size_t pointAIndex,
            size_t pointBIndex)
            : PointAIndex(pointAIndex)
            , PointBIndex(pointBIndex)
        {
        }
    };

    std::vector<SpringInfo> springInfos;

    struct TriangleInfo
    {
        size_t PointAIndex;
        size_t PointBIndex;
        size_t PointCIndex;

        TriangleInfo(
            size_t pointAIndex,
            size_t pointBIndex,
            size_t pointCIndex)
            : PointAIndex(pointAIndex)
            , PointBIndex(pointBIndex)
            , PointCIndex(pointCIndex)
        {
        }
    };

    std::vector<TriangleInfo> triangleInfos;


    // Visit point matrix

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

    for (int x = 0; x < structureWidth; ++x)
    {
        for (int y = 0; y < structureHeight; ++y)
        {
            if (!!pointInfoMatrix[x][y])
            {
                Material const * mtl = pointInfoMatrix[x][y]->Mtl;

                //
                // Create point
                //

                Point & point = allPoints.emplace_back(
                    ship,
                    vec2(
                        static_cast<float>(x) - halfWidth, 
                        static_cast<float>(y)) + shipDefinition.Offset,
                    mtl,
                    mtl->IsHull ? 0.0f : 1.0f, // No buoyancy if it's a hull point, as it can't get water
                    static_cast<int>(pointInfoMatrix[x][y]->PointIndex));  

                //
                // Create point color
                //

                allPointColors.emplace_back(mtl->RenderColour);


                //
                // Create point texture coordinates
                //

                if (!!shipDefinition.TextureImage)
                {
                    allPointTextureCoordinates.emplace_back(
                        static_cast<float>(x) / static_cast<float>(structureWidth),
                        static_cast<float>(y) / static_cast<float>(structureHeight));
                }

                // If a non-hull node has empty space on one of its four sides, it is automatically leaking.
                // Check if a is leaking; a is leaking if:
                // - a is not hull, AND
                // - there is at least a hole at E, S, W, N
                if (!point.GetMaterial()->IsHull)
                {
                    if ((x < structureWidth - 1 && !pointInfoMatrix[x + 1][y])
                        || (y < structureHeight - 1 && !pointInfoMatrix[x][y + 1])
                        || (x > 0 && !pointInfoMatrix[x - 1][y])
                        || (y > 0 && !pointInfoMatrix[x][y - 1]))
                    {
                        point.SetLeaking();

                        ++leakingPointsCount;
                    }
                }


                //
                // Check if a spring exists
                //

                // First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y),
                // i.e. E, NE, N, NW - this covers each pair of points in each direction
                for (int i = 0; i < 4; ++i)
                {
                    int adjx1 = x + Directions[i][0];
                    int adjy1 = y + Directions[i][1];                    
                    if (adjx1 >= 0 && adjx1 < structureWidth && adjy1 >= 0) // Valid coordinates?
                    {
                        assert(adjy1 < structureHeight); // The four directions we're checking do not include S

                        if (!!pointInfoMatrix[adjx1][adjy1])
                        {
                            // This point is adjacent to the first point at one of E, NE, N, NW

                            //
                            // Create spring
                            // 

                            springInfos.emplace_back(
                                pointInfoMatrix[x][y]->PointIndex, 
                                pointInfoMatrix[adjx1][adjy1]->PointIndex);


                            //
                            // Check if a triangle exists

                            // Check adjacent point in next CW direction
                            int adjx2 = x + Directions[i + 1][0];
                            int adjy2 = y + Directions[i + 1][1];                            
                            if (adjx2 >= 0 && adjx2 < structureWidth && adjy2 >= 0) // Valid coordinates?
                            {
                                assert(adjy2 < structureHeight); // The five directions we're checking do not include S

                                if (!!pointInfoMatrix[adjx2][adjy2])
                                {
                                    // This point is adjacent to the first point at one of SW, E, NE, N

                                    //
                                    // Create triangle
                                    // 

                                    triangleInfos.emplace_back(
                                        pointInfoMatrix[x][y]->PointIndex,
                                        pointInfoMatrix[adjx1][adjy1]->PointIndex,
                                        pointInfoMatrix[adjx2][adjy2]->PointIndex);
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    //
    // 3. Create Springs
    //

    ElementRepository<Spring> allSprings(springInfos.size());

    for (SpringInfo const & springInfo : springInfos)
    {
        Point * a = &(allPoints[springInfo.PointAIndex]);
        Point * b = &(allPoints[springInfo.PointBIndex]);

        // If one of the two nodes is not a hull node, then the spring is not hull
        Material const * const mtl = b->GetMaterial()->IsHull ? a->GetMaterial() : b->GetMaterial();

        allSprings.emplace_back(
            ship,
            a,
            b,
            mtl);
    }


    //
    // 4. Create Triangles
    //

    ElementRepository<Triangle> allTriangles(triangleInfos.size());

    for (TriangleInfo const & triangleInfo : triangleInfos)
    {
        Point * a = &(allPoints[triangleInfo.PointAIndex]);
        Point * b = &(allPoints[triangleInfo.PointBIndex]);
        Point * c = &(allPoints[triangleInfo.PointCIndex]);

        allTriangles.emplace_back(
            ship,
            a,
            b,
            c);
    }


    //
    // 4. Create Electrical Elements
    //

    std::vector<ElectricalElement*> allElectricalElements;

    for (Point & point : allPoints)
    {
        if (!!point.GetMaterial()->Electrical)
        {
            switch (point.GetMaterial()->Electrical->ElementType)
            {
                case Material::ElectricalProperties::ElectricalElementType::Cable:
                {
                    allElectricalElements.emplace_back(new Cable(ship, &point));
                    break;
                }

                case Material::ElectricalProperties::ElectricalElementType::Generator:
                {
                    allElectricalElements.emplace_back(new Generator(ship, &point));
                    break;
                }

                case Material::ElectricalProperties::ElectricalElementType::Lamp:
                {
                    allElectricalElements.emplace_back(new Lamp(ship, &point));
                    break;
                }
            }
        }
    }


    ship->InitializeRepository(
        std::move(allPoints),   
        std::move(allPointColors),
        std::move(allPointTextureCoordinates),
        std::move(allSprings),
        std::move(allTriangles),
        std::move(allElectricalElements));

    return std::unique_ptr<Ship>(ship);
}

Ship::Ship(World * parentWorld)
    : mId(parentWorld->GetNextShipId())
    , mParentWorld(parentWorld)    
    , mAllPoints(0)
    , mAllPointColors(0)
    , mAllPointTextureCoordinates(0)
    , mAllSprings(0)
    , mAllTriangles(0)
    , mAllElectricalElements()
    , mScheduler()
    , mConnectedComponentSizes()
    , mIsPointCountDirty(true)
    , mAreElementsDirty(true)
    , mIsSinking(false)
    , mTotalWater(0.0)
{
}

Ship::~Ship()
{
    // Delete elements that are not unique_ptr's nor are kept
    // in a PointerContainer
}

void Ship::DestroyAt(
    vec2 const & targetPos, 
    float radius,
    GameParameters const & gameParameters)
{
    // Destroy all points within the radius
    for (Point & point : mAllPoints)
    {
        if (!point.IsDeleted())
        {
            if ((point.GetPosition() - targetPos).length() < radius)
            {
                // Notify
                mParentWorld->GetGameEventHandler()->OnDestroy(
                    point.GetMaterial(), 
                    mParentWorld->IsUnderwater(point, gameParameters),
                    1u);

                // Destroy point
                point.Destroy();
            }
        }
    }

    mAllElectricalElements.shrink_to_fit();
}

void Ship::DrawTo(
    vec2f const & targetPos,
    float strength)
{
    // Attract all points to a single position
    for (Point & point : mAllPoints)
    {
        if (!point.IsDeleted())
        {
            vec2f displacement = (targetPos - point.GetPosition());
            float forceMagnitude = strength / sqrtf(0.1f + displacement.length());
            point.AddForce(displacement.normalise() * forceMagnitude);
        }
    }
}

Point const * Ship::GetNearestPointAt(
    vec2 const & targetPos, 
    float radius) const
{
    Point const * bestPoint = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (Point const & point : mAllPoints)
    {
        if (!point.IsDeleted())
        {
            float distance = (point.GetPosition() - targetPos).length();
            if (distance < radius && distance < bestDistance)
            {
                bestPoint = &point;
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

    for (Point & point : mAllPoints)
    {
        //
        // 1) Leak water: stuff some water into all the leaking nodes that are underwater, 
        //    if the external pressure is larger
        //

        if (point.IsLeaking())
        {
            float waterLevel = mParentWorld->GetWaterHeight(
                point.GetPosition().x,
                gameParameters);

            float const externalWaterPressure = point.GetExternalWaterPressure(
                waterLevel,
                gameParameters);

            if (externalWaterPressure > point.GetWater())
            {
                float newWater = dt * gameParameters.WaterPressureAdjustment * (externalWaterPressure - point.GetWater());
                point.AddWater(newWater);
                mTotalWater += newWater;
            }
        }


        //
        // 2) Detect connected components
        //

        // Don't visit destroyed points, or we run the risk of creating a zillion connected components
        if (!point.IsDeleted())
        {
            // Check if visited
            if (point.GetCurrentConnectedComponentDetectionStepSequenceNumber() != currentStepSequenceNumber)
            {
                // This node has not been visited, hence it's the beginning of a new connected component
                ++currentConnectedComponentId;
                size_t pointsInCurrentConnectedComponent = 0;

                //
                // Propagate the connected component ID to all points reachable from this point
                //

                assert(pointsToVisitForConnectedComponents.empty());
                pointsToVisitForConnectedComponents.push(&point);
                point.SetConnectedComponentDetectionStepSequenceNumber(currentStepSequenceNumber);

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
    for (Spring & spring : mAllSprings)
    {
        // Don't visit destroyed springs, or we run the risk of being affected by destroyed connected points
        if (!spring.IsDeleted())
        {
            if (!spring.GetMaterial()->IsHull)
            {
                Point * const pointA = spring.GetPointA();

                Point * const pointB = spring.GetPointB();

                // cos_theta > 0 => pointA above pointB
                float cos_theta = (pointB->GetPosition() - pointA->GetPosition()).normalise().dot(gameParameters.GravityNormal);

                // The 0.60 can be tuned, it's just to stop all the water being stuffed into the lowest node...
                float correction = 0.60f * cos_theta * dt * (cos_theta > 0.0f ? pointA->GetWater() : pointB->GetWater());
                pointA->AddWater(-correction);
                pointB->AddWater(correction);
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
    for (Spring & spring : mAllSprings)
    {   
        // Don't visit destroyed springs, or we run the risk of being affected by destroyed connected points
        if (!spring.IsDeleted())
        {
            if (!spring.GetMaterial()->IsHull)
            {
                Point * const pointA = spring.GetPointA();
                float const aWater = pointA->GetWater();

                Point * const pointB = spring.GetPointB();
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
}

void Ship::DiffuseLight(
    float dt,
    GameParameters const & gameParameters)
{
    //
    // Diffuse light from each lamp to the closest adjacent (i.e. spring-connected) points,
    // inversely-proportional to the square of the distance
    //

    // Greater adjustment => underrated distance => wider diffusion
    float const adjustmentCoefficient = powf(1.0f - gameParameters.LightDiffusionAdjustment, 2.0f);

    // Visit all points
    for (Point & point : mAllPoints)
    {
        point.ZeroLight();

        vec2f const & pointPosition = point.GetPosition();

        // Go through all lamps in the same connected component
        for (ElectricalElement * el : mAllElectricalElements)
        {
            assert(!el->IsDeleted());

            if (ElectricalElement::Type::Lamp == el->GetType()
                && el->GetPoint()->GetConnectedComponentId() == point.GetConnectedComponentId())
            {
                Point * const lampPoint = el->GetPoint();

                assert(!lampPoint->IsDeleted());

                // TODO: this needs to be replaced with getting the light from the lamp itself
                float const lampLight = 1.0f;

                float squareDistance = std::max(
                    1.0f,
                    (pointPosition - lampPoint->GetPosition()).squareLength() * adjustmentCoefficient);

                assert(squareDistance >= 1.0f);

                point.AdjustLight(lampLight / squareDistance);
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

    float dragCoefficient = (1.0f - powf(0.6f, dt));

    for (Point & point : mAllPoints)
    {
        // We can safely update all points, even deleted ones
        point.Update(
            dt, 
            dragCoefficient, 
            gameParameters);
    }


    //
    // Update springs dynamics
    //

    // Iterate the spring relaxation
    DoSpringsRelaxation(dt, gameParameters);

    // Update tension strain for all springs; might cause springs to break
    for (Spring & spring : mAllSprings)
    {
        // Avoid breaking deleted springs
        if (!spring.IsDeleted())
        {
            spring.UpdateTensionStrain(gameParameters, gameEventHandler);
        }
    }

    //
    // Clear up pointer containers, in case there have been deletions
    // during or before this update step
    //

    mAllElectricalElements.shrink_to_fit();


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
        gameParameters);
}

void Ship::Render(
    GameParameters const & gameParameters,
    RenderContext & renderContext) const
{
    if (mIsPointCountDirty)
    {
        //
        // Upload point colors and texture coordinates
        //

        renderContext.UploadShipPointVisualAttributes(
            mId,
            mAllPointColors.data(), 
            mAllPointTextureCoordinates.data(),
            mAllPointColors.size());

        mIsPointCountDirty = false;
    }


    //
    // Upload points
    //

    renderContext.UploadShipPointsStart(
        mId,
        mAllPoints.size());

    for (Point const & point : mAllPoints)
    {
        renderContext.UploadShipPoint(
            mId,
            point.GetPosition().x,
            point.GetPosition().y,
            point.GetLight(),
            point.GetWater());
    }

    renderContext.UploadShipPointsEnd(mId);


    //
    // Element upload
    //    

    if (!mConnectedComponentSizes.empty())
    {
        //
        // Upload elements (point (elements), springs and triangles), iff dirty
        //

        if (mAreElementsDirty)
        {
            renderContext.UploadShipElementsStart(
                mId,
                mConnectedComponentSizes);

            //
            // Upload all the points
            //

            for (Point const & point : mAllPoints)
            {
                if (!point.IsDeleted())
                {
                    renderContext.UploadShipElementPoint(
                        mId,
                        point.GetElementIndex(),
                        point.GetConnectedComponentId());
                }
            }

            //
            // Upload all the springs
            //

            for (Spring const & spring : mAllSprings)
            {
                if (!spring.IsDeleted())
                {
                    assert(spring.GetPointA()->GetConnectedComponentId() == spring.GetPointB()->GetConnectedComponentId());

                    renderContext.UploadShipElementSpring(
                        mId,
                        spring.GetPointA()->GetElementIndex(),
                        spring.GetPointB()->GetElementIndex(),
                        spring.GetPointA()->GetConnectedComponentId());
                }
            }


            //
            // Upload all the triangles
            //

            for (Triangle const & triangle : mAllTriangles)
            {
                if (!triangle.IsDeleted())
                {
                    assert(triangle.GetPointA()->GetConnectedComponentId() == triangle.GetPointB()->GetConnectedComponentId()
                        && triangle.GetPointA()->GetConnectedComponentId() == triangle.GetPointC()->GetConnectedComponentId());

                    renderContext.UploadShipElementTriangle(
                        mId,
                        triangle.GetPointA()->GetElementIndex(),
                        triangle.GetPointB()->GetElementIndex(),
                        triangle.GetPointC()->GetElementIndex(),
                        triangle.GetPointA()->GetConnectedComponentId());
                }
            }

            renderContext.UploadShipElementsEnd(mId);

            mAreElementsDirty = false;
        }

        //////
        ////// Upload all lamps
        //////

        ////renderContext.UploadLampsStart(
        ////    mId, 
        ////    mConnectedComponentSizes.size());

        ////for (ElectricalElement const * el : mAllElectricalElements)
        ////{
        ////    assert(!el->IsDeleted());

        ////    if (ElectricalElement::Type::Lamp == el->GetType())
        ////    {
        ////        Point const * lampPoint = el->GetPoint();

        ////        assert(!lampPoint->IsDeleted());

        ////        // TODO: this needs to be replaced with getting the light from the lamp itself,
        ////        // which would have been previously set via UpdateLightIntensity
        ////        float const lampLight = 1.0f;

        ////        renderContext.UploadLamp(
        ////            mId,
        ////            lampPoint->GetPosition().x,
        ////            lampPoint->GetPosition().y,
        ////            lampLight,
        ////            lampPoint->GetConnectedComponentId());
        ////    }
        ////}

        ////renderContext.UploadLampsEnd(
        ////    mid);
    }        



    //
    // Upload all stressed springs
    //

    if (renderContext.GetShowStressedSprings())
    {
        renderContext.UploadStressedSpringsStart(
            mId,
            mAllSprings.size());

        for (Spring const & spring : mAllSprings)
        {
            if (!spring.IsDeleted())
            {
                if (spring.IsStressed())
                {
                    renderContext.UploadStressedSpring(
                        mId,
                        spring.GetPointA()->GetElementIndex(),
                        spring.GetPointB()->GetElementIndex());
                }
            }
        }

        renderContext.UploadStressedSpringsEnd(mId);
    }


    //
    // Render ship
    //

    renderContext.RenderShip(mId);
}

///////////////////////////////////////////////////////////////////////////////////
// Private Helpers
///////////////////////////////////////////////////////////////////////////////////

void Ship::DoSpringsRelaxation(
    float dt,
    GameParameters const & gameParameters)
{
    //
    // Relax
    //

    // Calculate number of parallel chunks and size of each chunk
    size_t nParallelChunks = mScheduler.GetNumberOfThreads();
    size_t parallelChunkSize = std::max((mAllSprings.size() / nParallelChunks), (size_t)1);

    assert(parallelChunkSize > 0);

    // Run iterations
    for (int iter = 0; iter < 24; ++iter)
    {
        for (size_t i = 0; i < mAllSprings.size(); /* incremented in loop */)
        {
            size_t available = mAllSprings.size() - i;
            size_t thisChunkSize = (available >= parallelChunkSize) ? parallelChunkSize : available;
            assert(thisChunkSize > 0);

            mScheduler.Schedule(
                new SpringRelaxationCalculateTask(
                    this,
                    i,
                    i + thisChunkSize));

            i += thisChunkSize;
        }
            
        mScheduler.WaitForAllTasks();
    }


    //
    // Damp
    //

    // Run iterations
    for (int iter = 0; iter < 3; ++iter)
    {
        for (Spring & spring : mAllSprings)
        {
            // Don't damp destroyed springs, or we run the risk of being affected by destroyed connected points
            if (!spring.IsDeleted())
            {
                spring.Damp(gameParameters.SpringDampingFactor);
            }
        }
    }
}

void Ship::SpringRelaxationCalculateTask::Process()
{
    for (Spring & spring : mParentShip->mAllSprings.range(mStartSpringIndex, mEndSpringIndex))
    {
        // Don't relax destroyed springs, or we run the risk of being affected by destroyed connected points
        if (!spring.IsDeleted())
        {
            spring.Relax();
        }
    }
}

void Ship::PointIntegrateTask::Process()
{
    for (size_t i = mFirstPointIndex; i <= mLastPointIndex; ++i)
    {
        Point & point = mParentShip->mAllPoints[i];

        point.AddToPosition(point.GetForce() * mDt);
        point.ZeroForce();
    }
}

}