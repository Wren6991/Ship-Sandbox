/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameOpenGL.h"
#include "ImageData.h"
#include "RenderTypes.h"
#include "Vectors.h"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ShipRenderContext
{
public:

    ShipRenderContext(std::optional<ImageData> const & texture);
    
    ~ShipRenderContext();

public:

    //
    // Points
    //

    void UploadPointVisualAttributes(
        vec3f const * colors, 
        vec2f const * textureCoordinates,
        size_t pointCount);

    void UploadPointsStart(size_t maxPoints);

    inline void UploadPoint(
        float x,
        float y,
        float light,
        float water)
    {
        assert(mPointBufferSize + 1u <= mPointBufferMaxSize);
        ShipRenderContext::Point * point = &(mPointBuffer[mPointBufferSize]);

        point->x = x;
        point->y = y;
        point->light = light;
        point->water = water;

        ++mPointBufferSize;
    }

    void UploadPointsEnd();


    //
    // Springs and triangles
    //

    void UploadElementsStart(std::vector<std::size_t> const & connectedComponentsMaxSizes);

    inline void UploadElementPoint(
        int pointIndex,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mElementBufferSizes.size());
        assert(connectedComponentIndex < mElementBufferMaxSizes.size());
        assert(mElementBufferSizes[connectedComponentIndex].pointCount + 1u <= mElementBufferMaxSizes[connectedComponentIndex].pointCount);

        PointElement * const pointElement = &(mPointElementBuffers[connectedComponentIndex][mElementBufferSizes[connectedComponentIndex].pointCount]);

        pointElement->pointIndex = pointIndex;

        ++(mElementBufferSizes[connectedComponentIndex].pointCount);
    }

    inline void UploadElementSpring(
        int pointIndex1,
        int pointIndex2,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mElementBufferSizes.size());
        assert(connectedComponentIndex < mElementBufferMaxSizes.size());
        assert(mElementBufferSizes[connectedComponentIndex].springCount + 1u <= mElementBufferMaxSizes[connectedComponentIndex].springCount);

        SpringElement * const springElement = &(mSpringElementBuffers[connectedComponentIndex][mElementBufferSizes[connectedComponentIndex].springCount]);

        springElement->pointIndex1 = pointIndex1;
        springElement->pointIndex2 = pointIndex2;

        ++(mElementBufferSizes[connectedComponentIndex].springCount);
    }

    inline void UploadElementTriangle(
        int pointIndex1,
        int pointIndex2,
        int pointIndex3,
        size_t connectedComponentId)
    {
        size_t const connectedComponentIndex = connectedComponentId - 1;

        assert(connectedComponentIndex < mElementBufferSizes.size());
        assert(connectedComponentIndex < mElementBufferMaxSizes.size());
        assert(mElementBufferSizes[connectedComponentIndex].triangleCount + 1u <= mElementBufferMaxSizes[connectedComponentIndex].triangleCount);

        TriangleElement * const triangleElement = &(mTriangleElementBuffers[connectedComponentIndex][mElementBufferSizes[connectedComponentIndex].triangleCount]);

        triangleElement->pointIndex1 = pointIndex1;
        triangleElement->pointIndex2 = pointIndex2;
        triangleElement->pointIndex3 = pointIndex3;

        ++(mElementBufferSizes[connectedComponentIndex].triangleCount);
    }

    void UploadElementsEnd();


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

        assert(connectedComponentIndex < mLampBuffers.size());

        LampElement & lampElement = mLampBuffers[connectedComponentIndex].emplace_back();

        lampElement.x = x;
        lampElement.y = y;
        lampElement.lightIntensity = lightIntensity;
    }

    void UploadLampsEnd();


    //
    // Stressed springs
    //

    void UploadStressedSpringsStart(size_t maxSprings);

    inline void UploadStressedSpring(
        int pointIndex1,
        int pointIndex2)
    {
        assert(mStressedSpringBufferSize + 1u <= mStressedSpringBufferMaxSize);
        SpringElement * const springElement = &(mStressedSpringBuffer[mStressedSpringBufferSize]);

        springElement->pointIndex1 = pointIndex1;
        springElement->pointIndex2 = pointIndex2;

        ++mStressedSpringBufferSize;
    }

    void UploadStressedSpringsEnd();

    /////////////////////////////////////////////////////////////

    void Render(
        ShipRenderMode renderMode,
        bool showStressedSprings,
        float ambientLightIntensity,
        float canvasToVisibleWorldHeightRatio,
        float(&orthoMatrix)[4][4]);


private:

    void DescribePointVBO();
    void DescribePointColorVBO();
    void DescribePointTextureVBO();

private:

    //
    // Points
    //

#pragma pack(push)
    struct Point
    {
        float x;
        float y;
        float light;
        float water;
    };
#pragma pack(pop)

    size_t mPointCount;
    std::unique_ptr<ShipRenderContext::Point[]> mPointBuffer;
    size_t mPointBufferSize;
    size_t mPointBufferMaxSize;

    GameOpenGLVBO mPointColorVBO;
    GameOpenGLVBO mPointElementTextureCoordinatesVBO;
    GameOpenGLVBO mPointVBO;


    //
    // Elements (points, springs, and triangles)
    //

    GameOpenGLShaderProgram mElementColorShaderProgram;
    GLint mElementColorShaderOrthoMatrixParameter;
    GLint mElementColorShaderAmbientLightIntensityParameter;

    GameOpenGLShaderProgram mElementTextureShaderProgram;
    GLint mElementTextureShaderOrthoMatrixParameter;
    GLint mElementTextureShaderAmbientLightIntensityParameter;

#pragma pack(push)
    struct PointElement
    {
        int pointIndex;
    };
#pragma pack(pop)

#pragma pack(push)
    struct SpringElement
    {
        int pointIndex1;
        int pointIndex2;
    };
#pragma pack(pop)

#pragma pack(push)
    struct TriangleElement
    {
        int pointIndex1;
        int pointIndex2;
        int pointIndex3;
    };
#pragma pack(pop)

    struct ElementCounts
    {
        size_t pointCount;
        size_t springCount;
        size_t triangleCount;

        ElementCounts()
            : pointCount(0)
            , springCount(0)
            , triangleCount(0)
        {}
    };

    std::vector<std::unique_ptr<PointElement[]>> mPointElementBuffers;
    std::vector<std::unique_ptr<SpringElement[]>> mSpringElementBuffers;
    std::vector<std::unique_ptr<TriangleElement[]>> mTriangleElementBuffers;
    std::vector<ElementCounts> mElementBufferSizes;
    std::vector<ElementCounts> mElementBufferMaxSizes;

    GameOpenGLVBO mPointElementVBO;
    GameOpenGLVBO mSpringElementVBO;
    GameOpenGLVBO mTriangleElementVBO;
    
    GameOpenGLTexture mElementTexture;

    //
    // Lamps
    //

#pragma pack(push)
    struct LampElement
    {
        float x;
        float y;
        float lightIntensity;
    };
#pragma pack(pop)

    std::vector<std::vector<LampElement>> mLampBuffers;


    //
    // Stressed springs
    //

    GameOpenGLShaderProgram mStressedSpringShaderProgram;
    GLint mStressedSpringShaderOrthoMatrixParameter;

    std::unique_ptr<SpringElement[]> mStressedSpringBuffer;
    size_t mStressedSpringBufferSize;
    size_t mStressedSpringBufferMaxSize;

    GameOpenGLVBO mStressedSpringVBO;

    GameOpenGLTexture mStressedSpringTexture;
};
