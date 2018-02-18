/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-13
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameOpenGL.h"
#include "ResourceLoader.h"
#include "Vectors.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

class RenderContext
{
public:

    RenderContext(std::shared_ptr<ResourceLoader> resourceLoader);
    
    ~RenderContext();

public:

    float GetZoom() const
    {
        return mZoom;
    }

    void SetZoom(float zoom)
    {
        mZoom = zoom;

        CalculateWorldCoordinates();
        CalculateOrthoMatrix();        
    }

    void AdjustZoom(float amount)
    {
        mZoom *= amount;

        CalculateWorldCoordinates();
        CalculateOrthoMatrix();
    }

    vec2f GetCameraWorldPosition() const
    {
        return vec2f(mCamX, mCamY);
    }

    void SetCameraWorldPosition(vec2f const & pos)
    {
        mCamX = pos.x;
        mCamY = pos.y;

        CalculateWorldCoordinates();
        CalculateOrthoMatrix();
    }

    void AdjustCameraWorldPosition(vec2f const & worldOffset)
    {
        mCamX -= worldOffset.x;
        mCamY -= worldOffset.y;

        CalculateWorldCoordinates();
        CalculateOrthoMatrix();
    }

    int GetCanvasSizeWidth() const
    {
        return mCanvasWidth;
    }

    int GetCanvasSizeHeight() const
    {
        return mCanvasHeight;
    }

    void SetCanvasSize(int width, int height)
    {
        mCanvasWidth = width;
        mCanvasHeight = height;

        glViewport(0, 0, mCanvasWidth, mCanvasHeight);

        CalculateWorldCoordinates();
        CalculateOrthoMatrix();
    }

    vec2f GetWorldSize() const
    {
        return vec2f(mWorldWidth, mWorldHeight);
    }

    float GetAmbientLightIntensity() const
    {
        return mAmbientLightIntensity;
    }

    void SetAmbientLightIntensity(float intensity)
    {
        mAmbientLightIntensity = intensity;
    }

    bool GetShowStress() const
    {
        return mShowStress;
    }

    void SetShowStress(bool showStress)
    {
        mShowStress = showStress;
    }

    bool GetUseXRayMode() const
    {
        return mUseXRayMode;
    }

    void SetUseXRayMode(bool useXRayMode)
    {
        mUseXRayMode = useXRayMode;
    }

    bool GetShowShipThroughWater() const
    {
        return mShowShipThroughWater;
    }

    void SetShowShipThroughWater(bool showShipThroughWater)
    {
        mShowShipThroughWater = showShipThroughWater;
    }

    bool GetDrawPointsOnly() const
    {
        return mDrawPointsOnly;
    }

    void SetDrawPointsOnly(bool drawPointsOnly)
    {
        mDrawPointsOnly = drawPointsOnly;
    }

    inline vec2 Screen2World(vec2 const & screenCoordinates)
    {
        return vec2(
            (screenCoordinates.x / static_cast<float>(mCanvasWidth) - 0.5f) * mWorldWidth + mCamX,
            (screenCoordinates.y / static_cast<float>(mCanvasHeight) - 0.5f) * -mWorldHeight + mCamY);
    }

    inline vec2 ScreenOffset2WorldOffset(vec2 const & screenOffset)
    {
        return vec2(
            screenOffset.x / static_cast<float>(mCanvasWidth) * mWorldWidth,
            screenOffset.y / static_cast<float>(mCanvasHeight) * -mWorldHeight);
    }

public:

    void RenderStart();


    //
    // Land
    //

    void RenderLandStart(size_t slices);

    inline void RenderLand(
        float x,
        float bottom,
        float top)
    {
        assert(mLandBufferSize + 1u <= mLandBufferMaxSize);

        LandElement * landElement = &(mLandBuffer[mLandBufferSize]);

        landElement->x1 = x;
        landElement->y1 = top;
        landElement->x2 = x;
        landElement->y2 = bottom;

        ++mLandBufferSize;
    }

    void RenderLandEnd();


    //
    // Water
    //

    void RenderWaterStart(size_t slices);

    inline void RenderWater(
        float x,
        float bottom,
        float top)
    {
        assert(mWaterBufferSize + 1u <= mWaterBufferMaxSize);

        WaterElement * waterElement = &(mWaterBuffer[mWaterBufferSize]);

        waterElement->x1 = x;
        waterElement->y1 = top;
        waterElement->x2 = x;
        waterElement->y2 = bottom;

        ++mWaterBufferSize;
    }

    void RenderWaterEnd();


    //
    // Ship Points
    //

    void UploadShipPointStart(size_t points);

    inline void UploadShipPoint(
        float x,
        float y,
        float r,
        float g,
        float b)
    {
        assert(mShipPointBufferSize + 1u <= mShipPointBufferMaxSize);

        ShipPointElement * shipPointElement = &(mShipPointBuffer[mShipPointBufferSize]);

        shipPointElement->x = x;
        shipPointElement->y = y;
        shipPointElement->r = r;
        shipPointElement->g = g;
        shipPointElement->b = b;

        ++mShipPointBufferSize;
    }

    void UploadShipPointEnd();

    void RenderShipPoints();


    //
    // Springs
    //

    void RenderSpringsStart(size_t springs);

    inline void RenderSpring(
        int shipPointIndex1,
        int shipPointIndex2)
    {
        assert(mSpringBufferSize + 1u <= mSpringBufferMaxSize);

        SpringElement * springElement = &(mSpringBuffer[mSpringBufferSize]);

        springElement->shipPointIndex1 = shipPointIndex1;
        springElement->shipPointIndex2 = shipPointIndex2;

        ++mSpringBufferSize;
    }

    void RenderSpringsEnd();


    void RenderStressedSpringsStart(size_t maxSprings);

    inline void RenderStressedSpring(
        int shipPointIndex1,
        int shipPointIndex2)
    {
        assert(mStressedSpringBufferSize + 1u <= mStressedSpringBufferMaxSize);

        SpringElement * springElement = &(mStressedSpringBuffer[mStressedSpringBufferSize]);

        springElement->shipPointIndex1 = shipPointIndex1;
        springElement->shipPointIndex2 = shipPointIndex2;

        ++mStressedSpringBufferSize;
    }

    void RenderStressedSpringsEnd();


    //
    // Ship triangles
    //

    void RenderShipTrianglesStart(size_t triangles);

    inline void RenderShipTriangle(
        int shipPointIndex1,
        int shipPointIndex2,
        int shipPointIndex3)
    {
        assert(mShipTriangleBufferSize + 1u <= mShipTriangleBufferMaxSize);

        ShipTriangleElement * shipTriangleElement = &(mShipTriangleBuffer[mShipTriangleBufferSize]);

        shipTriangleElement->shipPointIndex1 = shipPointIndex1;
        shipTriangleElement->shipPointIndex2 = shipPointIndex2;
        shipTriangleElement->shipPointIndex3 = shipPointIndex3;

        ++mShipTriangleBufferSize;
    }

    void RenderShipTrianglesEnd();

    void RenderEnd();

private:

    template<typename T, typename TDeleter>
    class OpenGLObject
    {
    public:
        OpenGLObject(T value)
            : mValue(value)
        {}

        ~OpenGLObject()
        {
            TDeleter::Delete(mValue);
        }

        OpenGLObject(OpenGLObject const & other) = delete;

        OpenGLObject & operator=(OpenGLObject const & other) = delete;

        OpenGLObject & operator=(OpenGLObject && other)
        {
            TDeleter::Delete(mValue);
            mValue = other.mValue;
            other.mValue = 0;

            return *this;
        }

        T operator*() const
        {
            return mValue;
        }

    private:
        T mValue;
    };

    struct OpenGLProgramDeleter
    {
        static void Delete(GLuint p)
        {
            if (p != 0)
            {
                glDeleteProgram(p);
            }
        }
    };

    struct OpenGLVBODeleter
    {
        static void Delete(GLuint p)
        {
            if (p != 0)
            {
                glDeleteBuffers(1, &p);
            }
        }
    };

    using OpenGLShaderProgram = OpenGLObject<GLuint, OpenGLProgramDeleter>;
    using OpenGLVBO = OpenGLObject<GLuint, OpenGLVBODeleter>;

private:
    
    void CompileShader(
        char const * shaderSource,
        GLenum shaderType,
        OpenGLShaderProgram const & shaderProgram);

    void LinkProgram(
        OpenGLShaderProgram const & shaderProgram,
        std::string const & programName);

    GLint GetParameterLocation(
        OpenGLShaderProgram const & shaderProgram,
        std::string const & parameterName);

    void DescribeShipPointsVBO();

    void CalculateOrthoMatrix();

    void CalculateWorldCoordinates();

private:

    std::shared_ptr<ResourceLoader> mResourceLoader;

    //
    // Land
    //

    OpenGLShaderProgram mLandShaderProgram;
    GLint mLandShaderLandColorParameter;
    GLint mLandShaderAmbientLightIntensityParameter;
    GLint mLandShaderOrthoMatrixParameter;

#pragma pack(push)
    struct LandElement
    {
        float x1;
        float y1;
        float x2;
        float y2;
    };
#pragma pack(pop)

    std::unique_ptr<LandElement[]> mLandBuffer;
    size_t mLandBufferSize;
    size_t mLandBufferMaxSize;

    OpenGLVBO mLandVBO;


    //
    // Water
    //

    OpenGLShaderProgram mWaterShaderProgram;
    GLint mWaterShaderWaterColorParameter;
    GLint mWaterShaderAmbientLightIntensityParameter;
    GLint mWaterShaderOrthoMatrixParameter;

#pragma pack(push)
    struct WaterElement
    {
        float x1;
        float y1;
        float x2;
        float y2;
    };
#pragma pack(pop)

    std::unique_ptr<WaterElement[]> mWaterBuffer;
    size_t mWaterBufferSize;
    size_t mWaterBufferMaxSize;

    OpenGLVBO mWaterVBO;


    //
    // Ship points
    //

    OpenGLShaderProgram mShipPointShaderProgram;
    GLint mShipPointShaderOrthoMatrixParameter;

#pragma pack(push)
    struct ShipPointElement
    {
        float x;
        float y;
        float r;
        float g;
        float b;
    };
#pragma pack(pop)

    std::unique_ptr<ShipPointElement[]> mShipPointBuffer;
    size_t mShipPointBufferSize;
    size_t mShipPointBufferMaxSize;

    OpenGLVBO mShipPointVBO;


    //
    // Springs
    //

    OpenGLShaderProgram mSpringShaderProgram;
    GLint mSpringShaderOrthoMatrixParameter;

#pragma pack(push)
    struct SpringElement
    {
        int shipPointIndex1;
        int shipPointIndex2;
    };
#pragma pack(pop)

    std::unique_ptr<SpringElement[]> mSpringBuffer;
    size_t mSpringBufferSize;
    size_t mSpringBufferMaxSize;

    OpenGLVBO mSpringVBO;


    //
    // Stressed springs
    //

    OpenGLShaderProgram mStressedSpringShaderProgram;
    GLint mStressedSpringShaderAmbientLightIntensityParameter;
    GLint mStressedSpringShaderOrthoMatrixParameter;

    std::unique_ptr<SpringElement[]> mStressedSpringBuffer;
    size_t mStressedSpringBufferSize;
    size_t mStressedSpringBufferMaxSize;

    OpenGLVBO mStressedSpringVBO;


    //
    // Ship triangles
    //

    OpenGLShaderProgram mShipTriangleShaderProgram;
    GLint mShipTriangleShaderOrthoMatrixParameter;

#pragma pack(push)
    struct ShipTriangleElement
    {
        int shipPointIndex1;
        int shipPointIndex2;
        int shipPointIndex3;
    };
#pragma pack(pop)

    std::unique_ptr<ShipTriangleElement[]> mShipTriangleBuffer;
    size_t mShipTriangleBufferSize;
    size_t mShipTriangleBufferMaxSize;

    OpenGLVBO mShipTriangleVBO;

private:

    // The Ortho matrix
    float mOrthoMatrix[4][4];

    // The world coordinates
    float mWorldWidth;
    float mWorldHeight;


    //
    // The current render parameters
    //

    float mZoom;
    float mCamX;
    float mCamY;
    int mCanvasWidth;
    int mCanvasHeight;
    float mAmbientLightIntensity;

    bool mShowStress;
    bool mUseXRayMode;
    bool mShowShipThroughWater;
    bool mDrawPointsOnly;    
};
