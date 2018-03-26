/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ElementRepository.h"
#include "GameParameters.h"
#include "Material.h"
#include "Physics.h"
#include "PointerContainer.h"
#include "RenderContext.h"
#include "Scheduler.h"
#include "ShipDefinition.h"
#include "Vectors.h"

#include <set>

namespace Physics
{

class Ship
{
public:

    static std::unique_ptr<Ship> Create(
        int shipId,
        World * parentWorld,
        ShipDefinition const & shipDefinition,
        std::vector<std::unique_ptr<Material const>> const & allMaterials);

    ~Ship();

    unsigned int GetId() const { return mId; }

    void Destroy();

    World const * GetParentWorld() const { return mParentWorld; }
    World * GetParentWorld() { return mParentWorld; }

    auto const & GetElectricalElements() const { return mAllElectricalElements; }
    auto & GetElectricalElements() { return mAllElectricalElements; }

    auto const & GetPoints() const { return mAllPoints; }
    auto & GetPoints() { return mAllPoints; }

    auto const & GetSprings() const { return mAllSprings; }
    auto & GetSprings() { return mAllSprings; }

    auto const & GetTriangles() const { return mAllTriangles; }
    auto & GetTriangles() { return mAllTriangles; }

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

    void PreparePointsForFinalStep(
        float dt,
        uint64_t currentStepSequenceNumber,
        GameParameters const & gameParameters);

    void GravitateWater(
        float dt,
        GameParameters const & gameParameters);

    void BalancePressure(float dt);

    void DiffuseLight(
        float dt,
        GameParameters const & gameParameters);

    void Update(
        float dt,
        uint64_t currentStepSequenceNumber,
        GameParameters const & gameParameters);

    void Render(
        GameParameters const & gameParameters,
        RenderContext & renderContext) const;

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

    Ship(
        int id,
        World * parentWorld);

    void InitializeRepository(
        ElementRepository<Point> && allPoints,
        ElementRepository<vec3f> && allPointColors,
        ElementRepository<vec2f> && allPointTextureCoordinates,
        ElementRepository<Spring> && allSprings,
        ElementRepository<Triangle> && allTriangles,
        std::vector<ElectricalElement *> && allElectricalElements)
    {
        mAllPoints = std::move(allPoints);
        mAllPointColors = std::move(allPointColors);
        mAllPointTextureCoordinates = std::move(allPointTextureCoordinates);
        mAllSprings = std::move(allSprings);
        mAllTriangles = std::move(allTriangles);
        mAllElectricalElements.initialize(std::move(allElectricalElements));

        mIsPointCountDirty = true;
        mAreElementsDirty = true;
    }

    void DoSpringsRelaxation(
        float dt,
        GameParameters const & gameParameters);

    struct SpringRelaxationCalculateTask : Scheduler::ITask
    {
    public:

        SpringRelaxationCalculateTask(
            Ship * parentShip,
            size_t startSpringIndex,
            size_t endSpringIndex)
            : mParentShip(parentShip)
            , mStartSpringIndex(startSpringIndex)
            , mEndSpringIndex(endSpringIndex)
        {
        }

        virtual ~SpringRelaxationCalculateTask()
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

    unsigned int const mId;
    World * const mParentWorld;

    // All the ship elements - never removed, the repositories maintain their own size forever
    ElementRepository<Point> mAllPoints;
    ElementRepository<vec3f> mAllPointColors;
    ElementRepository<vec2f> mAllPointTextureCoordinates;
    ElementRepository<Spring> mAllSprings;
    ElementRepository<Triangle> mAllTriangles;

    // Parts repository
    PointerContainer<ElectricalElement> mAllElectricalElements;

    // The scheduler we use for parallelizing updates
    Scheduler mScheduler;

    // Connected components metadata
    std::vector<std::size_t> mConnectedComponentSizes;

    // Flag remembering whether the number of points has changed
    // since the last time we delivered them to the rendering context
    mutable bool mIsPointCountDirty;

    // Flag remembering whether points (elements) and/or springs and/or triangles have changed
    // since the last time we delivered them to the rendering context
    mutable bool mAreElementsDirty;

    // Sinking detection
    bool mIsSinking;
    float mTotalWater;
};

template<>
inline void Ship::RegisterDestruction(Point * /* element */)
{
    // Remember that we need to re-upload ship elements
    mAreElementsDirty = true;
}

template<>
inline void Ship::RegisterDestruction(Spring * /* element */)
{
    // Remember that we need to re-upload ship elements
    mAreElementsDirty = true;
}

template<>
inline void Ship::RegisterDestruction(Triangle * /* element */)
{
    // Remember that we need to re-upload ship elements
    mAreElementsDirty = true;
}

template<>
inline void Ship::RegisterDestruction(ElectricalElement * /* element */)
{
    // Also tell the pointer container, he'll take care of removing the element later
    mAllElectricalElements.register_deletion();
}

}