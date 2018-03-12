/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-13
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameOpenGL.h"
#include "ProgressCallback.h"
#include "ResourceLoader.h"
#include "Vectors.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>

class RenderContext
{
public:

    RenderContext(
        ResourceLoader & resourceLoader,
        ProgressCallback const & progressCallback);
    
    ~RenderContext();

public:

    float GetZoom() const
    {
        return mZoom;
    }

    void SetZoom(float zoom)
    {
        mZoom = zoom;

        CalculateVisibleWorldCoordinates();
        CalculateOrthoMatrix();        
    }

    void AdjustZoom(float amount)
    {
        mZoom *= amount;

        CalculateVisibleWorldCoordinates();
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

        CalculateVisibleWorldCoordinates();
        CalculateOrthoMatrix();
    }

    void AdjustCameraWorldPosition(vec2f const & worldOffset)
    {
        mCamX += worldOffset.x;
        mCamY += worldOffset.y;

        CalculateVisibleWorldCoordinates();
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

        CalculateVisibleWorldCoordinates();
        CalculateOrthoMatrix();
    }

    float GetVisibleWorldWidth() const
    {
        return mVisibleWorldWidth;
    }

    float GetVisibleWorldHeight() const
    {
        return mVisibleWorldHeight;
    }

    float GetAmbientLightIntensity() const
    {
        return mAmbientLightIntensity;
    }

    void SetAmbientLightIntensity(float intensity)
    {
        mAmbientLightIntensity = intensity;
    }

    float GetWaterTransparency() const
    {
        return mWaterTransparency;
    }

    void SetWaterTransparency(float transparency)
    {
        mWaterTransparency = transparency;
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

    inline vec2 ScreenToWorld(vec2 const & screenCoordinates)
    {
        return vec2(
            (screenCoordinates.x / static_cast<float>(mCanvasWidth) - 0.5f) * mVisibleWorldWidth + mCamX,
            (screenCoordinates.y / static_cast<float>(mCanvasHeight) - 0.5f) * -mVisibleWorldHeight + mCamY);
    }

    inline vec2 ScreenOffsetToWorldOffset(vec2 const & screenOffset)
    {
        return vec2(
            screenOffset.x / static_cast<float>(mCanvasWidth) * mVisibleWorldWidth,
            screenOffset.y / static_cast<float>(mCanvasHeight) * -mVisibleWorldHeight);
    }

public:

    void RenderStart();


    //
    // Clouds
    //

    void RenderCloudsStart(size_t clouds);

    inline void RenderCloud(
        float virtualX,
        float virtualY,
        float scale)
    {
        //
        // We use Normalized Device Coordinates here
        //

        //
        // Roll coordinates into a 3.0 X 2.0 view,
        // then take the central slice and map it into NDC ((-1,-1) X (1,1))
        //

        float rolledX = fmodf(virtualX, 3.0f);
        if (rolledX < 0.0f)
            rolledX += 3.0f;
        float mappedX = -1.0f + 2.0f * (rolledX - 1.0f);

        float rolledY = fmodf(virtualY, 2.0f);
        if (rolledY < 0.0f)
            rolledY += 2.0f;
        float mappedY = -1.0f + 2.0f * (rolledY - 0.5f);

        assert(mCloudBufferSize + 1u <= mCloudBufferMaxSize);
        CloudElement * cloudElement = &(mCloudBuffer[mCloudBufferSize]);

        // Calculate texture dimensions in NDC, as proportion in 1280 X 1024
        // (totally arbitrary, a texture this size would fill the entire screen)
        float textureTileNdcW = static_cast<float>(mCloudTextureDatas[GetCloudTextureIndex(mCloudBufferSize)]->Width) / 1280.0f;
        float textureTileNdcH = static_cast<float>(mCloudTextureDatas[GetCloudTextureIndex(mCloudBufferSize)]->Height) / 1024.0f;

        float leftX = mappedX - textureTileNdcW * scale / 2.0f;
        float rightX = mappedX + textureTileNdcW * scale / 2.0f;
        float topY = mappedY - textureTileNdcH * scale / 2.0f;
        float bottomY = mappedY + textureTileNdcH * scale / 2.0f;
        
        cloudElement->ndcXTopLeft = leftX;
        cloudElement->ndcYTopLeft = topY;
        cloudElement->ndcTextureXTopLeft = 0.0f;
        cloudElement->ndcTextureYTopLeft = 0.0f;

        cloudElement->ndcXBottomLeft = leftX;
        cloudElement->ndcYBottomLeft = bottomY;
        cloudElement->ndcTextureXBottomLeft = 0.0f;
        cloudElement->ndcTextureYBottomLeft = 1.0f;

        cloudElement->ndcXTopRight = rightX;
        cloudElement->ndcYTopRight = topY;
        cloudElement->ndcTextureXTopRight = 1.0f;
        cloudElement->ndcTextureYTopRight = 0.0f;

        cloudElement->ndcXBottomRight = rightX;
        cloudElement->ndcYBottomRight = bottomY;
        cloudElement->ndcTextureXBottomRight = 1.0f;
        cloudElement->ndcTextureYBottomRight = 1.0f;

        ++mCloudBufferSize;
    }

    void RenderCloudsEnd();


    //
    // Land and Water
    //

    void UploadLandAndWaterStart(size_t slices);

    inline void UploadLandAndWater(
        float x,
        float yLand,
        float yWater,
        float restWaterHeight)
    {
        assert(mLandBufferMaxSize == mWaterBufferMaxSize);
        assert(mLandBufferMaxSize > 0);

        float const worldBottom = mCamY - (mVisibleWorldHeight / 2.0f);

        //
        // Store Land element
        //

        assert(mLandBufferSize + 1u <= mLandBufferMaxSize);
        LandElement * landElement = &(mLandBuffer[mLandBufferSize]);

        landElement->x1 = x;
        landElement->y1 = yLand;
        landElement->x2 = x;
        landElement->y2 = worldBottom;

        ++mLandBufferSize;


        //
        // Store water element
        //

        assert(mWaterBufferSize + 1u <= mWaterBufferMaxSize);
        WaterElement * waterElement = &(mWaterBuffer[mWaterBufferSize]);

        waterElement->x1 = x;
        waterElement->y1 = yWater > yLand ? yWater : yLand; // Make sure that islands are not covered in water!
        waterElement->textureY1 = restWaterHeight;

        waterElement->x2 = x;
        waterElement->y2 = yLand;
        waterElement->textureY2 = 0.0f;

        ++mWaterBufferSize;
    }

    void UploadLandAndWaterEnd();

    void RenderLand();

    void RenderWater();



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

        OpenGLObject(OpenGLObject && other)
        {
            mValue = other.mValue;
            other.mValue = 0;
        }

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

    struct OpenGLTextureDeleter
    {
        static void Delete(GLuint p)
        {
            if (p != 0)
            {
                glDeleteTextures(1, &p);
            }
        }
    };

    using OpenGLShaderProgram = OpenGLObject<GLuint, OpenGLProgramDeleter>;
    using OpenGLVBO = OpenGLObject<GLuint, OpenGLVBODeleter>;
    using OpenGLTexture = OpenGLObject<GLuint, OpenGLTextureDeleter>;

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

    void CalculateVisibleWorldCoordinates();

private:

    //
    // Clouds
    //

    OpenGLShaderProgram mCloudShaderProgram;
    GLint mCloudShaderAmbientLightIntensityParameter;

#pragma pack(push)
    struct CloudElement
    {
        float ndcXTopLeft;
        float ndcYTopLeft;
        float ndcTextureXTopLeft;
        float ndcTextureYTopLeft;

        float ndcXBottomLeft;
        float ndcYBottomLeft;
        float ndcTextureXBottomLeft;
        float ndcTextureYBottomLeft;

        float ndcXTopRight;
        float ndcYTopRight;
        float ndcTextureXTopRight;
        float ndcTextureYTopRight;

        float ndcXBottomRight;
        float ndcYBottomRight;
        float ndcTextureXBottomRight;
        float ndcTextureYBottomRight;
    };
#pragma pack(pop)

    std::unique_ptr<CloudElement[]> mCloudBuffer;
    size_t mCloudBufferSize;
    size_t mCloudBufferMaxSize;
    
    OpenGLVBO mCloudVBO;

    std::vector<std::unique_ptr<ResourceLoader::Texture const>> mCloudTextureDatas;
    std::vector<OpenGLTexture> mCloudTextures;

    inline size_t GetCloudTextureIndex(size_t cloudIndex) const
    {
        assert(mCloudTextureDatas.size() == mCloudTextures.size());

        return cloudIndex % mCloudTextures.size();
    }

    //
    // Land
    //

    OpenGLShaderProgram mLandShaderProgram;
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
    
    std::unique_ptr<ResourceLoader::Texture const> mLandTextureData;
    OpenGLTexture mLandTexture;


    //
    // Water
    //

    OpenGLShaderProgram mWaterShaderProgram;
    GLint mWaterShaderAmbientLightIntensityParameter;
    GLint mWaterShaderWaterTransparencyParameter;
    GLint mWaterShaderOrthoMatrixParameter;

#pragma pack(push)
    struct WaterElement
    {
        float x1;
        float y1;
        float textureY1;
        
        float x2;
        float y2;
        float textureY2;
    };
#pragma pack(pop)

    std::unique_ptr<WaterElement[]> mWaterBuffer;
    size_t mWaterBufferSize;
    size_t mWaterBufferMaxSize;

    OpenGLVBO mWaterVBO;

    std::unique_ptr<ResourceLoader::Texture const> mWaterTextureData;
    OpenGLTexture mWaterTexture;


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


    //
    // Multi-purpose shaders
    //

    OpenGLShaderProgram mMatteNdcShaderProgram;
    GLint mMatteNdcShaderColorParameter;
    OpenGLVBO mMatteNdcVBO;

    OpenGLShaderProgram mMatteWorldShaderProgram;    
    GLint mMatteWorldShaderColorParameter;
    GLint mMatteWorldShaderOrthoMatrixParameter;
    OpenGLVBO mMatteWorldVBO;

private:

    // The Ortho matrix
    float mOrthoMatrix[4][4];

    // The world coordinates of the visible portion
    float mVisibleWorldWidth;
    float mVisibleWorldHeight;


    //
    // The current render parameters
    //

    float mZoom;
    float mCamX;
    float mCamY;
    int mCanvasWidth;
    int mCanvasHeight;

    float mAmbientLightIntensity;
    float mWaterTransparency;

    bool mShowStress;
    bool mUseXRayMode;
    bool mShowShipThroughWater;
    bool mDrawPointsOnly;    
};
