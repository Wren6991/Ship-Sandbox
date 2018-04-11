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
#include "MaterialDatabase.h"
#include "Physics.h"
#include "PointerContainer.h"
#include "RenderContext.h"
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
        MaterialDatabase const & materials,
        uint64_t currentStepSequenceNumber);

    ~Ship();

    unsigned int GetId() const { return mId; }

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

    void Initialize(
        ElementRepository<Point> && allPoints,
        ElementRepository<vec3f> && allPointColors,
        ElementRepository<vec2f> && allPointTextureCoordinates,
        ElementRepository<Spring> && allSprings,
        ElementRepository<Triangle> && allTriangles,
        std::vector<ElectricalElement *> && allElectricalElements,
        uint64_t currentStepSequenceNumber)
    {
        mAllPoints = std::move(allPoints);
        mAllPointColors = std::move(allPointColors);
        mAllPointTextureCoordinates = std::move(allPointTextureCoordinates);
        mAllSprings = std::move(allSprings);
        mAllTriangles = std::move(allTriangles);
        mAllElectricalElements.initialize(std::move(allElectricalElements));

        mIsPointCountDirty = true;
        mAreElementsDirty = true;

        DetectConnectedComponents(currentStepSequenceNumber);
    }

    void DoSpringsRelaxation(
        float dt,
        GameParameters const & gameParameters);

    void DetectConnectedComponents(uint64_t currentStepSequenceNumber);

    void LeakWater(
        float dt,
        GameParameters const & gameParameters);

    void GravitateWater(
        float dt,
        GameParameters const & gameParameters);

    void BalancePressure(float dt);

    void DiffuseLight(
        float dt,
        GameParameters const & gameParameters);

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

    // Connected components metadata
    std::vector<std::size_t> mConnectedComponentSizes;

    // Flag remembering whether the number of points has changed
    // since the last time we delivered them to the rendering context.
    // Does not count deleted points at this moment - deleted points remain
    // in the buffer that we deliver to the rendering context
    mutable bool mIsPointCountDirty;

    // Flag remembering whether points (elements) and/or springs (incl. ropes) and/or triangles have changed
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

    // We don't mark the point count as dirty, as at this moment we keep 
    // uploading all points, including deleted ones, to the rendering engine.
    // We are content with only updating the point *elements*
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