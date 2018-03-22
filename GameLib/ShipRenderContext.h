/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameOpenGL.h"
#include "ImageData.h"
#include "Vectors.h"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ShipRenderContext
{
public:

    ShipRenderContext();
    
    ~ShipRenderContext();

public:

    //
    // Ship Points
    //

    void UploadShipPointVisualAttributes(
        vec3f const * colors, 
        vec2f const * textureCoordinates,
        size_t elementCount);

    void UploadShipPointsStart(size_t maxPoints);

    inline void UploadShipPoint(
        float x,
        float y,
        float light,
        float water)
    {
        assert(mShipPointBufferSize + 1u <= mShipPointBufferMaxSize);
        ShipPointElement * shipPointElement = &(mShipPointBuffer[mShipPointBufferSize]);

        shipPointElement->x = x;
        shipPointElement->y = y;
        shipPointElement->light = light;
        shipPointElement->water = water;

        ++mShipPointBufferSize;
    }

    void UploadShipPointsEnd();

    void RenderShipPoints(
        float ambientLightIntensity,
        float canvasToVisibleWorldHeightRatio,
        float (&orthoMatrix)[4][4]);


    //
    // Ship springs and triangles
    //

    void UploadShipStart(std::vector<std::size_t> const & connectedComponentsMaxSizes);

    inline void UploadShipSpring(
        int shipPointIndex1,
        int shipPointIndex2,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mShipBufferSizes.size());
        assert(connectedComponentIndex < mShipBufferMaxSizes.size());
        assert(mShipBufferSizes[connectedComponentIndex].springCount + 1u <= mShipBufferMaxSizes[connectedComponentIndex].springCount);

        ShipSpringElement * const shipSpringElement = &(mShipSpringBuffers[connectedComponentIndex][mShipBufferSizes[connectedComponentIndex].springCount]);

        shipSpringElement->shipPointIndex1 = shipPointIndex1;
        shipSpringElement->shipPointIndex2 = shipPointIndex2;

        ++(mShipBufferSizes[connectedComponentIndex].springCount);
    }

    inline void UploadShipTriangle(
        int shipPointIndex1,
        int shipPointIndex2,
        int shipPointIndex3,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mShipBufferSizes.size());
        assert(connectedComponentIndex < mShipBufferMaxSizes.size());
        assert(mShipBufferSizes[connectedComponentIndex].triangleCount + 1u <= mShipBufferMaxSizes[connectedComponentIndex].triangleCount);

        ShipTriangleElement * const shipTriangleElement = &(mShipTriangleBuffers[connectedComponentIndex][mShipBufferSizes[connectedComponentIndex].triangleCount]);

        shipTriangleElement->shipPointIndex1 = shipPointIndex1;
        shipTriangleElement->shipPointIndex2 = shipPointIndex2;
        shipTriangleElement->shipPointIndex3 = shipPointIndex3;

        ++(mShipBufferSizes[connectedComponentIndex].triangleCount);
    }

    void UploadShipEnd();


    //
    // Lamps
    //

    void UploadLampsStart(size_t connectedComponents);

    void UploadLamp(
        float x,
        float y,
        float lightIntensity,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mShipLampBuffers.size());

        ShipLampElement & shipLampElement = mShipLampBuffers[connectedComponentIndex].emplace_back();

        shipLampElement.x = x;
        shipLampElement.y = y;
        shipLampElement.lightIntensity = lightIntensity;
    }

    void UploadLampsEnd();


    //
    // Ship
    //

    void RenderShip(
        bool useXRayMode,
        float ambientLightIntensity,
        float canvasToVisibleWorldHeightRatio,
        float(&orthoMatrix)[4][4]);


    //
    // Stressed springs
    //

    void RenderStressedSpringsStart(size_t maxSprings);

    inline void RenderStressedSpring(
        int shipPointIndex1,
        int shipPointIndex2)
    {
        assert(mStressedSpringBufferSize + 1u <= mStressedSpringBufferMaxSize);
        ShipSpringElement * const springElement = &(mStressedSpringBuffer[mStressedSpringBufferSize]);

        springElement->shipPointIndex1 = shipPointIndex1;
        springElement->shipPointIndex2 = shipPointIndex2;

        ++mStressedSpringBufferSize;
    }

    void RenderStressedSpringsEnd(
        float ambientLightIntensity,
        float canvasToVisibleWorldHeightRatio,
        float(&orthoMatrix)[4][4]);

private:

    void DescribeShipPointVBO();

private:

    //
    // Ship points
    //

    GameOpenGLShaderProgram mShipPointShaderProgram;
    GLint mShipPointShaderOrthoMatrixParameter;
    GLint mShipPointShaderAmbientLightIntensityParameter;

#pragma pack(push)
    struct ShipPointElement
    {
        float x;
        float y;
        float light;
        float water;
    };
#pragma pack(pop)

    size_t mShipElementCount;
    std::unique_ptr<ShipPointElement[]> mShipPointBuffer;
    size_t mShipPointBufferSize;
    size_t mShipPointBufferMaxSize;

    GameOpenGLVBO mShipPointColorVBO;
    GameOpenGLVBO mShipPointTextureCoordinatesVBO;
    GameOpenGLVBO mShipPointVBO;


    //
    // Ship springs and triangles
    //

    GameOpenGLShaderProgram mShipShaderProgram;
    GLint mShipShaderOrthoMatrixParameter;
    GLint mShipShaderAmbientLightIntensityParameter;

#pragma pack(push)
    struct ShipSpringElement
    {
        int shipPointIndex1;
        int shipPointIndex2;
    };
#pragma pack(pop)

#pragma pack(push)
    struct ShipTriangleElement
    {
        int shipPointIndex1;
        int shipPointIndex2;
        int shipPointIndex3;
    };
#pragma pack(pop)

    struct ShipElementCounts
    {
        size_t springCount;
        size_t triangleCount;

        ShipElementCounts()
            : springCount(0)
            , triangleCount(0)
        {}
    };

    std::vector<std::unique_ptr<ShipSpringElement[]>> mShipSpringBuffers;
    std::vector<std::unique_ptr<ShipTriangleElement[]>> mShipTriangleBuffers;
    std::vector<ShipElementCounts> mShipBufferSizes;
    std::vector<ShipElementCounts> mShipBufferMaxSizes;

    GameOpenGLVBO mShipSpringVBO;
    GameOpenGLVBO mShipTriangleVBO;


    //
    // Ship lamps
    //

#pragma pack(push)
    struct ShipLampElement
    {
        float x;
        float y;
        float lightIntensity;
    };
#pragma pack(pop)

    std::vector<std::vector<ShipLampElement>> mShipLampBuffers;


    //
    // Stressed springs
    //

    GameOpenGLShaderProgram mStressedSpringShaderProgram;
    GLint mStressedSpringShaderAmbientLightIntensityParameter;
    GLint mStressedSpringShaderOrthoMatrixParameter;

    std::unique_ptr<ShipSpringElement[]> mStressedSpringBuffer;
    size_t mStressedSpringBufferSize;
    size_t mStressedSpringBufferMaxSize;

    GameOpenGLVBO mStressedSpringVBO;
};
