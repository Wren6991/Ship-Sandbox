/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-13
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "RenderContext.h"

#include "GameException.h"

#include <cstring>

RenderContext::RenderContext(
    ResourceLoader & resourceLoader,
    ProgressCallback const & progressCallback)
    : // Clouds
      mCloudShaderProgram(0u)
    , mCloudShaderAmbientLightIntensityParameter(0)
    , mCloudBuffer()
    , mCloudBufferSize(0u)
    , mCloudBufferMaxSize(0u)    
    , mCloudVBO(0u)
    , mCloudTextureDatas()
    , mCloudTextures()
    // Land
    , mLandShaderProgram(0u)
    , mLandShaderAmbientLightIntensityParameter(0)
    , mLandShaderOrthoMatrixParameter(0)
    , mLandBuffer()
    , mLandBufferSize(0u)
    , mLandBufferMaxSize(0u)
    , mLandVBO(0u)
    , mLandTexture(0u)
    , mLandTextureData()
    // Water
    , mWaterShaderProgram(0u)
    , mWaterShaderAmbientLightIntensityParameter(0)
    , mWaterShaderWaterTransparencyParameter(0)
    , mWaterShaderOrthoMatrixParameter(0)
    , mWaterBuffer()
    , mWaterBufferSize(0u)
    , mWaterBufferMaxSize(0u)
    , mWaterVBO(0u)
    , mWaterTexture(0u)
    , mWaterTextureData()
    // Ship points
    , mShipPointShaderProgram(0u)
    , mShipPointShaderOrthoMatrixParameter(0)
    , mShipPointBuffer()
    , mShipPointBufferSize(0u)
    , mShipPointBufferMaxSize(0u)   
    , mShipPointVBO(0u)
    // Springs
    , mSpringShaderProgram(0u)
    , mSpringShaderOrthoMatrixParameter(0)
    , mSpringBuffer()
    , mSpringBufferSize(0u)
    , mSpringBufferMaxSize(0u)
    , mSpringVBO(0u)
    // Stressed springs
    , mStressedSpringShaderProgram(0u)
    , mStressedSpringShaderAmbientLightIntensityParameter(0)
    , mStressedSpringShaderOrthoMatrixParameter(0)
    , mStressedSpringBuffer()
    , mStressedSpringBufferSize(0u)
    , mStressedSpringBufferMaxSize(0u)
    , mStressedSpringVBO(0u)
    // Ship triangles
    , mShipTriangleShaderProgram(0u)
    , mShipTriangleShaderOrthoMatrixParameter(0)
    , mShipTriangleBuffer()
    , mShipTriangleBufferSize(0u)
    , mShipTriangleBufferMaxSize(0u)
    , mShipTriangleVBO(0u)
    // Multi-purpose shaders
    , mMatteNdcShaderProgram(0u)
    , mMatteNdcShaderColorParameter(0)
    , mMatteNdcVBO(0u)
    , mMatteWorldShaderProgram(0u)
    , mMatteWorldShaderColorParameter(0)
    , mMatteWorldShaderOrthoMatrixParameter(0)
    , mMatteWorldVBO(0u)
    // Render parameters
    , mZoom(1.0f)
    , mCamX(0.0f)
    , mCamY(0.0f)
    , mCanvasWidth(100)
    , mCanvasHeight(100)
    , mAmbientLightIntensity(1.0f)
    , mWaterTransparency(0.66f)
    , mShowStress(false)
    , mUseXRayMode(false)
    , mShowShipThroughWater(false)
    , mDrawPointsOnly(false)
{
    GLuint tmpGLuint;

    //
    // Init OpenGL
    //

    InitOpenGL();


    //
    // Check OpenGL version
    //

    int versionMaj = 0;
    int versionMin = 0;
    char const * glVersion = (char*)glGetString(GL_VERSION);
    if (nullptr == glVersion)
    {
        throw GameException("OpenGL completely not supported");
    }

    sscanf(glVersion, "%d.%d", &versionMaj, &versionMin);
    if (versionMaj < 2)
    {
        throw GameException("This game requires at least OpenGL 2.0 support; the version currently supported by your computer is " + std::string(glVersion));
    }


    //
    // Load textures
    //

    mCloudTextureDatas = resourceLoader.LoadTexturesRgba(
        "cloud",
        [&progressCallback](float progress, std::string const &)
        {
            if (progressCallback)
                progressCallback(progress / 3.0f, "Loading textures...");
        });

    if (progressCallback)
        progressCallback(0.666f, "Loading textures...");

    mLandTextureData = resourceLoader.LoadTextureRgb("sand_1.jpg");

    if (progressCallback)
        progressCallback(1.0f, "Loading textures...");

    mWaterTextureData = resourceLoader.LoadTextureRgb("water_1.jpg");

    // Generate all texture names
    size_t const numberOfTextures = mCloudTextureDatas.size() + 2U;
    std::vector<GLuint> textureNames(numberOfTextures, 0U);
    glGenTextures(static_cast<GLsizei>(numberOfTextures), textureNames.data());



    //
    // Clouds 
    //

    // Create program

    mCloudShaderProgram = glCreateProgram();

    char const * cloudVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        attribute vec2 inputTexturePos;
        
        // Outputs
        varying vec2 texturePos;

        void main()
        {
            gl_Position = vec4(inputPos.xy, -1.0, 1.0);
            texturePos = inputTexturePos;
        }
    )";

    CompileShader(cloudVertexShaderSource, GL_VERTEX_SHADER, mCloudShaderProgram);

    char const * cloudFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec2 texturePos;

        // The texture
        uniform sampler2D inputTexture;

        // Parameters        
        uniform float paramAmbientLightIntensity;

        void main()
        {
            gl_FragColor = texture2D(inputTexture, texturePos) * paramAmbientLightIntensity;
        } 
    )";

    CompileShader(cloudFragmentShaderSource, GL_FRAGMENT_SHADER, mCloudShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mCloudShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mCloudShaderProgram, 1, "inputTexturePos");

    // Link
    LinkProgram(mCloudShaderProgram, "Cloud");

    // Get uniform locations
    mCloudShaderAmbientLightIntensityParameter = GetParameterLocation(mCloudShaderProgram, "paramAmbientLightIntensity");

    // Create VBO    
    glGenBuffers(1, &tmpGLuint);
    mCloudVBO = tmpGLuint;

    // Set hardcoded parameters
    glUseProgram(*mCloudShaderProgram);
    glUseProgram(0);

    // Create textures
    for (size_t i = 0; i < mCloudTextureDatas.size(); ++i)
    {
        // Create texture
        mCloudTextures.emplace_back(textureNames.back());
        textureNames.pop_back();

        glBindTexture(GL_TEXTURE_2D, *mCloudTextures.back());

        // Set repeat mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mCloudTextureDatas[i]->Width, mCloudTextureDatas[i]->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mCloudTextureDatas[i]->Data.get());
        if (GL_NO_ERROR != glGetError())
        {
            throw GameException("Error uploading cloud texture onto GPU");
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    assert(mCloudTextureDatas.size() == mCloudTextures.size());

    //
    // Land 
    //

    // Create program

    mLandShaderProgram = glCreateProgram();

    char const * landVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        
        // Parameters
        uniform mat4 paramOrthoMatrix;

        // Outputs
        varying vec2 texturePos;

        void main()
        {
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
            texturePos = inputPos.xy;
        }
    )";

    CompileShader(landVertexShaderSource, GL_VERTEX_SHADER, mLandShaderProgram);

    char const * landFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec2 texturePos;

        // The texture
        uniform sampler2D inputTexture;

        // Parameters        
        uniform float paramAmbientLightIntensity;
        uniform vec2 paramTextureScaling;

        void main()
        {
            gl_FragColor = texture2D(inputTexture, texturePos * paramTextureScaling) * paramAmbientLightIntensity;
        } 
    )";

    CompileShader(landFragmentShaderSource, GL_FRAGMENT_SHADER, mLandShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mLandShaderProgram, 0, "inputPos");

    // Link
    LinkProgram(mLandShaderProgram, "Land");

    // Get uniform locations
    mLandShaderAmbientLightIntensityParameter = GetParameterLocation(mLandShaderProgram, "paramAmbientLightIntensity");
    GLint landShaderTextureScalingParameter = GetParameterLocation(mLandShaderProgram, "paramTextureScaling");
    mLandShaderOrthoMatrixParameter = GetParameterLocation(mLandShaderProgram, "paramOrthoMatrix");

    // Create VBO    
    glGenBuffers(1, &tmpGLuint);
    mLandVBO = tmpGLuint;

    // Calculate scaling: we want a tile to be repeated every these many world units
    static constexpr float LandTileWorldSize = 128.0f;
    
    // Set hardcoded parameters
    glUseProgram(*mLandShaderProgram);
    glUniform2f(
        landShaderTextureScalingParameter, 
        1.0f / LandTileWorldSize,
        1.0f / LandTileWorldSize);
    glUseProgram(0);

    // Create texture
    mLandTexture = textureNames.back();
    textureNames.pop_back(); 

    glBindTexture(GL_TEXTURE_2D, *mLandTexture);

    // Set repeat mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mLandTextureData->Width, mLandTextureData->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mLandTextureData->Data.get());
    if (GL_NO_ERROR != glGetError())
    {
        throw GameException("Error uploading land texture onto GPU");
    }

    glBindTexture(GL_TEXTURE_2D, 0);


    //
    // Water 
    //

    // Create program

    mWaterShaderProgram = glCreateProgram();

    char const * waterVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        attribute float inputTextureY;

        // Parameters
        uniform mat4 paramOrthoMatrix;

        // Outputs
        varying vec2 texturePos;

        void main()
        {
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
            texturePos = vec2(inputPos.x, inputTextureY);
        }
    )";

    CompileShader(waterVertexShaderSource, GL_VERTEX_SHADER, mWaterShaderProgram);

    char const * waterFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec2 texturePos;

        // The texture
        uniform sampler2D inputTexture;

        // Parameters        
        uniform float paramAmbientLightIntensity;
        uniform float paramWaterTransparency;
        uniform vec2 paramTextureScaling;

        void main()
        {
            vec4 textureColor = texture2D(inputTexture, texturePos * paramTextureScaling);
            gl_FragColor = vec4(textureColor.xyz, 1.0 - paramWaterTransparency) * paramAmbientLightIntensity;
        } 
    )";

    CompileShader(waterFragmentShaderSource, GL_FRAGMENT_SHADER, mWaterShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mWaterShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mWaterShaderProgram, 1, "inputTextureY");

    // Link
    LinkProgram(mWaterShaderProgram, "Water");

    // Get uniform locations    
    mWaterShaderAmbientLightIntensityParameter = GetParameterLocation(mWaterShaderProgram, "paramAmbientLightIntensity");
    mWaterShaderWaterTransparencyParameter = GetParameterLocation(mWaterShaderProgram, "paramWaterTransparency");    
    GLint waterShaderTextureScalingParameter = GetParameterLocation(mWaterShaderProgram, "paramTextureScaling");
    mWaterShaderOrthoMatrixParameter = GetParameterLocation(mWaterShaderProgram, "paramOrthoMatrix");

    // Create VBO
    glGenBuffers(1, &tmpGLuint);
    mWaterVBO = tmpGLuint;

    // Calculate scaling: we want a tile to be repeated every these many world units
    static constexpr float WaterTileWorldSize = 128.0f;

    // Set hardcoded parameters
    glUseProgram(*mWaterShaderProgram);
    glUniform2f(
        waterShaderTextureScalingParameter,
        1.0f / WaterTileWorldSize,
        1.0f / WaterTileWorldSize);
    glUseProgram(0);

    // Create texture
    mWaterTexture = textureNames.back();
    textureNames.pop_back();

    glBindTexture(GL_TEXTURE_2D, *mWaterTexture);

    // Set repeat mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaterTextureData->Width, mWaterTextureData->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mWaterTextureData->Data.get());
    if (GL_NO_ERROR != glGetError())
    {
        throw GameException("Error uploading water texture onto GPU");
    }

    glBindTexture(GL_TEXTURE_2D, 0);


    //
    // Ship points
    //

    glGenBuffers(1, &tmpGLuint);
    mShipPointVBO = tmpGLuint;


    //
    // Create ship points program
    //

    mShipPointShaderProgram = glCreateProgram();

    char const * shipPointShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        attribute vec3 inputCol;

        // Outputs
        varying vec3 vertexCol;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            vertexCol = inputCol;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(shipPointShaderSource, GL_VERTEX_SHADER, mShipPointShaderProgram);

    char const * shipPointFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec3 vertexCol;

        void main()
        {
            gl_FragColor = vec4(vertexCol.xyz, 1.0);
        } 
    )";

    CompileShader(shipPointFragmentShaderSource, GL_FRAGMENT_SHADER, mShipPointShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mShipPointShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mShipPointShaderProgram, 1, "inputCol");

    // Link
    LinkProgram(mShipPointShaderProgram, "Ship Point");

    // Get uniform locations
    mShipPointShaderOrthoMatrixParameter = GetParameterLocation(mShipPointShaderProgram, "paramOrthoMatrix");

    // Set hardcoded parameters    
    glUseProgram(*mShipPointShaderProgram);
    glUseProgram(0);


    //
    // Create spring program
    //

    mSpringShaderProgram = glCreateProgram();

    char const * springShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        attribute vec3 inputCol;

        // Outputs
        varying vec3 vertexCol;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            vertexCol = inputCol;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(springShaderSource, GL_VERTEX_SHADER, mSpringShaderProgram);

    char const * springFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec3 vertexCol;

        void main()
        {
            gl_FragColor = vec4(vertexCol.xyz, 1.0);
        } 
    )";

    CompileShader(springFragmentShaderSource, GL_FRAGMENT_SHADER, mSpringShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mSpringShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mSpringShaderProgram, 1, "inputCol");

    // Link
    LinkProgram(mSpringShaderProgram, "Spring");

    // Get uniform locations
    mSpringShaderOrthoMatrixParameter = GetParameterLocation(mSpringShaderProgram, "paramOrthoMatrix");

    // Create VBOs
    glGenBuffers(1, &tmpGLuint);
    mSpringVBO = tmpGLuint;

    // Set hardcoded parameters    
    glUseProgram(*mSpringShaderProgram);
    glUseProgram(0);


    //
    // Create stressed spring program
    //

    mStressedSpringShaderProgram = glCreateProgram();

    char const * stressedSpringShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(stressedSpringShaderSource, GL_VERTEX_SHADER, mStressedSpringShaderProgram);

    char const * stressedSpringFragmentShaderSource = R"(

        // Params
        uniform float paramAmbientLightIntensity;

        void main()
        {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * paramAmbientLightIntensity;
        } 
    )";

    CompileShader(stressedSpringFragmentShaderSource, GL_FRAGMENT_SHADER, mStressedSpringShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mStressedSpringShaderProgram, 0, "inputPos");

    // Link
    LinkProgram(mStressedSpringShaderProgram, "Stressed Spring");

    // Get uniform locations
    mStressedSpringShaderAmbientLightIntensityParameter = GetParameterLocation(mStressedSpringShaderProgram, "paramAmbientLightIntensity");
    mStressedSpringShaderOrthoMatrixParameter = GetParameterLocation(mStressedSpringShaderProgram, "paramOrthoMatrix");

    // Create VBOs
    glGenBuffers(1, &tmpGLuint);
    mStressedSpringVBO = tmpGLuint;

    // Set hardcoded parameters    
    glUseProgram(*mStressedSpringShaderProgram);
    glUseProgram(0);


    //
    // Create ship triangle program
    //

    mShipTriangleShaderProgram = glCreateProgram();

    char const * shipTriangleShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        attribute vec3 inputCol;

        // Outputs
        varying vec3 vertexCol;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            vertexCol = inputCol;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(shipTriangleShaderSource, GL_VERTEX_SHADER, mShipTriangleShaderProgram);

    char const * shipTriangleFragmentShaderSource = R"(

        // Inputs from previous shader
        varying vec3 vertexCol;

        void main()
        {
            gl_FragColor = vec4(vertexCol.xyz, 1.0);
        } 
    )";

    CompileShader(shipTriangleFragmentShaderSource, GL_FRAGMENT_SHADER, mShipTriangleShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mShipTriangleShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mShipTriangleShaderProgram, 1, "inputCol");

    // Link
    LinkProgram(mShipTriangleShaderProgram, "ShipTriangle");

    // Get uniform locations
    mShipTriangleShaderOrthoMatrixParameter = GetParameterLocation(mShipTriangleShaderProgram, "paramOrthoMatrix");

    // Create VBO
    glGenBuffers(1, &tmpGLuint);
    mShipTriangleVBO = tmpGLuint;
    
    // Set hardcoded parameters    
    glUseProgram(*mShipTriangleShaderProgram);
    glUseProgram(0);


    //
    // Multi-purpose Matte NDC shader
    //

    mMatteNdcShaderProgram = glCreateProgram();

    char const * matteNdcShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;

        void main()
        {
            gl_Position = vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(matteNdcShaderSource, GL_VERTEX_SHADER, mMatteNdcShaderProgram);

    char const * matteNdcFragmentShaderSource = R"(

        // Params
        uniform vec4 paramCol;

        void main()
        {
            gl_FragColor = paramCol;
        } 
    )";

    CompileShader(matteNdcFragmentShaderSource, GL_FRAGMENT_SHADER, mMatteNdcShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mMatteNdcShaderProgram, 0, "inputPos");

    // Link
    LinkProgram(mMatteNdcShaderProgram, "Matte NDC");

    // Get uniform locations
    mMatteNdcShaderColorParameter = GetParameterLocation(mMatteNdcShaderProgram, "paramCol");

    // Create VBO
    glGenBuffers(1, &tmpGLuint);
    mMatteNdcVBO = tmpGLuint;


    //
    // Multi-purpose Matte World shader
    //

    mMatteWorldShaderProgram = glCreateProgram();

    char const * matteWorldShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    CompileShader(matteWorldShaderSource, GL_VERTEX_SHADER, mMatteWorldShaderProgram);

    char const * matteWorldFragmentShaderSource = R"(

        // Params
        uniform vec4 paramCol;

        void main()
        {
            gl_FragColor = paramCol;
        } 
    )";

    CompileShader(matteWorldFragmentShaderSource, GL_FRAGMENT_SHADER, mMatteWorldShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mMatteWorldShaderProgram, 0, "inputPos");

    // Link
    LinkProgram(mMatteWorldShaderProgram, "Matte World");

    // Get uniform locations
    mMatteWorldShaderColorParameter = GetParameterLocation(mMatteWorldShaderProgram, "paramCol");
    mMatteWorldShaderOrthoMatrixParameter = GetParameterLocation(mMatteWorldShaderProgram, "paramOrthoMatrix");

    // Create VBO
    glGenBuffers(1, &tmpGLuint);
    mMatteWorldVBO = tmpGLuint;


    //
    // Initialize ortho matrix
    //

    for (size_t r = 0; r < 4; ++r)
    {
        for (size_t c = 0; c < 4; ++c)
        {
            mOrthoMatrix[r][c] = 0.0f;
        }
    }

    assert(textureNames.empty());
}

RenderContext::~RenderContext()
{
    glUseProgram(0u);
}

//////////////////////////////////////////////////////////////////////////////////

void RenderContext::RenderStart()
{
    // Set anti-aliasing for lines and polygons
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH, GL_NICEST);

    // Set nearest interpolation for textures
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Enable stencil test    
    glEnable(GL_STENCIL_TEST);    

    // Clear canvas 
    static const vec3f ClearColorBase(0.529f, 0.808f, 0.980f); // (cornflower blue)
    vec3f clearColor = ClearColorBase * mAmbientLightIntensity;
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);
}

void RenderContext::RenderCloudsStart(size_t clouds)
{
    if (clouds != mCloudBufferMaxSize)
    {
        // Realloc
        mCloudBuffer.reset();
        mCloudBuffer.reset(new CloudElement[clouds]);
        mCloudBufferMaxSize = clouds;
    }

    mCloudBufferSize = 0u;
}

void RenderContext::RenderCloudsEnd()
{
    //
    // Draw stencil
    //

    // Use matte world program
    glUseProgram(*mMatteWorldShaderProgram);

    // Set parameters
    glUniform4f(mMatteWorldShaderColorParameter, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(mMatteWorldShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind water buffer
    glBindBuffer(GL_ARRAY_BUFFER, *mWaterVBO);

    // Describe InputPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 1) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Disable writing to the color buffer
    glColorMask(false, false, false, false);

    // Write all one's to stencil buffer
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(2 * mWaterBufferSize));

    // Don't write anything to stencil buffer now
    glStencilMask(0x00);

    // Re-enable writing to the color buffer
    glColorMask(true, true, true, true);
    
    // Stop using program
    glUseProgram(0);


    //
    // Draw clouds
    //

    assert(mCloudBufferSize == mCloudBufferMaxSize);

    // Use program
    glUseProgram(*mCloudShaderProgram);

    // Set parameters
    glUniform1f(mCloudShaderAmbientLightIntensityParameter, mAmbientLightIntensity);

    // Upload cloud buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mCloudVBO);
    glBufferData(GL_ARRAY_BUFFER, mCloudBufferSize * sizeof(CloudElement), mCloudBuffer.get(), GL_DYNAMIC_DRAW);

    // Describe InputPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Describe InputTexturePos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Enable stenciling - only draw where there are no 1's
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

    // Draw
    for (size_t c = 0; c < mCloudBufferSize; ++c)
    {
        // Bind Texture
        glBindTexture(GL_TEXTURE_2D, *(mCloudTextures[GetCloudTextureIndex(c)]));

        // Draw
        glDrawArrays(GL_TRIANGLE_STRIP, static_cast<GLint>(4 * c), 4);
    }

    // Disable stenciling - draw always
    glStencilFunc(GL_ALWAYS, 0, 0x00);

    // Stop using program
    glUseProgram(0);
}

void RenderContext::UploadLandAndWaterStart(size_t slices)
{
    //
    // Prepare land buffer
    //

    if (slices + 1 != mLandBufferMaxSize)
    {
        // Realloc
        mLandBuffer.reset();
        mLandBuffer.reset(new LandElement[slices + 1]);
        mLandBufferMaxSize = slices + 1;
    }

    mLandBufferSize = 0u;

    //
    // Prepare water buffer
    //

    if (slices + 1 != mWaterBufferMaxSize)
    {
        // Realloc
        mWaterBuffer.reset();
        mWaterBuffer.reset(new WaterElement[slices + 1]);
        mWaterBufferMaxSize = slices + 1;
    }

    mWaterBufferSize = 0u;
}

void RenderContext::UploadLandAndWaterEnd()
{
    //
    // Upload land buffer
    //

    assert(mLandBufferSize == mLandBufferMaxSize);

    glBindBuffer(GL_ARRAY_BUFFER, *mLandVBO);
    glBufferData(GL_ARRAY_BUFFER, mLandBufferSize * sizeof(LandElement), mLandBuffer.get(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    //
    // Upload water buffer
    //

    assert(mWaterBufferSize == mWaterBufferMaxSize);

    glBindBuffer(GL_ARRAY_BUFFER, *mWaterVBO);
    glBufferData(GL_ARRAY_BUFFER, mWaterBufferSize * sizeof(WaterElement), mWaterBuffer.get(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
}

void RenderContext::RenderLand()
{
    // Use program
    glUseProgram(*mLandShaderProgram);

    // Set parameters
    glUniform1f(mLandShaderAmbientLightIntensityParameter, mAmbientLightIntensity);
    glUniformMatrix4fv(mLandShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, *mLandTexture);

    // Bind land buffer
    glBindBuffer(GL_ARRAY_BUFFER, *mLandVBO);

    // Describe InputPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(2 * mLandBufferSize));

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderWater()
{
    // Use program
    glUseProgram(*mWaterShaderProgram);

    // Set parameters
    glUniform1f(mWaterShaderAmbientLightIntensityParameter, mAmbientLightIntensity);
    glUniform1f(mWaterShaderWaterTransparencyParameter, mWaterTransparency);
    glUniformMatrix4fv(mWaterShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, *mWaterTexture);

    // Bind water buffer
    glBindBuffer(GL_ARRAY_BUFFER, *mWaterVBO);

    // Describe InputPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 1) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Describe InputTextureY
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, (2 + 1) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Enable blend (to make water transparent)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(2 * mWaterBufferSize));

    // Stop using program
    glUseProgram(0);
}

void RenderContext::UploadShipPointStart(size_t points)
{
    if (points != mShipPointBufferMaxSize)
    {
        // Realloc
        mShipPointBuffer.reset();
        mShipPointBuffer.reset(new ShipPointElement[points]);
        mShipPointBufferMaxSize = points;
    }

    mShipPointBufferSize = 0u;
}

void RenderContext::UploadShipPointEnd()
{
    assert(mShipPointBufferSize <= mShipPointBufferMaxSize);

    // Upload point buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    glBufferData(GL_ARRAY_BUFFER, mShipPointBufferSize * sizeof(ShipPointElement), mShipPointBuffer.get(), GL_DYNAMIC_DRAW);
}

void RenderContext::RenderShipPoints()
{
    assert(mShipPointBufferSize <= mShipPointBufferMaxSize);

    // Use program
    glUseProgram(*mShipPointShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mShipPointShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind ship points
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    DescribeShipPointsVBO();

    // Set point size
    glPointSize(0.15f * 2.0f * mCanvasHeight / mVisibleWorldHeight);

    // Draw
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mShipPointBufferSize));

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderSpringsStart(size_t springs)
{
    if (springs != mSpringBufferMaxSize)
    {
        // Realloc
        mSpringBuffer.reset();
        mSpringBuffer.reset(new SpringElement[springs]);
        mSpringBufferMaxSize = springs;
    }

    mSpringBufferSize = 0u;
}

void RenderContext::RenderSpringsEnd()
{
    assert(mSpringBufferSize <= mSpringBufferMaxSize);

    // Use program
    glUseProgram(*mSpringShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mSpringShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind ship points
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    DescribeShipPointsVBO();

    // Upload springs buffer 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mSpringVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSpringBufferSize * sizeof(SpringElement), mSpringBuffer.get(), GL_DYNAMIC_DRAW);

    // Set line size
    glLineWidth(0.1f * 2.0f * mCanvasHeight / mVisibleWorldHeight);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mSpringBufferSize), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderStressedSpringsStart(size_t maxSprings)
{
    if (maxSprings != mStressedSpringBufferMaxSize)
    {
        // Realloc
        mStressedSpringBuffer.reset();
        mStressedSpringBuffer.reset(new SpringElement[maxSprings]);
        mStressedSpringBufferMaxSize = maxSprings;
    }

    mStressedSpringBufferSize = 0u;
}

void RenderContext::RenderStressedSpringsEnd()
{
    assert(mStressedSpringBufferSize <= mStressedSpringBufferMaxSize);

    // Use program
    glUseProgram(*mStressedSpringShaderProgram);

    // Set parameters
    glUniform1f(mStressedSpringShaderAmbientLightIntensityParameter, mAmbientLightIntensity);
    glUniformMatrix4fv(mStressedSpringShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind ship points
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    DescribeShipPointsVBO();

    // Upload stressed springs buffer 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mStressedSpringVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mStressedSpringBufferSize * sizeof(SpringElement), mStressedSpringBuffer.get(), GL_DYNAMIC_DRAW);

    // Set line size
    glLineWidth(0.1f * 2.0f * mCanvasHeight / mVisibleWorldHeight);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mStressedSpringBufferSize), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderShipTrianglesStart(size_t triangles)
{
    if (triangles != mShipTriangleBufferMaxSize)
    {
        // Realloc
        mShipTriangleBuffer.reset();
        mShipTriangleBuffer.reset(new ShipTriangleElement[triangles]);
        mShipTriangleBufferMaxSize = triangles;
    }

    mShipTriangleBufferSize = 0u;
}

void RenderContext::RenderShipTrianglesEnd()
{
    assert(mShipTriangleBufferSize <= mShipTriangleBufferMaxSize);
    
    // Use program
    glUseProgram(*mShipTriangleShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mShipTriangleShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind ship points
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    DescribeShipPointsVBO();
    
    // Upload ship triangles buffer 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mShipTriangleVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mShipTriangleBufferSize * sizeof(ShipTriangleElement), mShipTriangleBuffer.get(), GL_DYNAMIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(3 * mShipTriangleBufferSize), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderEnd()
{
    glFlush();
}

////////////////////////////////////////////////////////////////////////////////////

void RenderContext::CompileShader(
    char const * shaderSource,
    GLenum shaderType,
    OpenGLShaderProgram const & shaderProgram)
{
    // Compile
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        throw GameException("ERROR Compiling vertex shader: " + std::string(infoLog));
    }

    // Attach to program
    glAttachShader(*shaderProgram, shader);

    // Delete shader
    glDeleteShader(shader);
}

void RenderContext::LinkProgram(
    OpenGLShaderProgram const & shaderProgram,
    std::string const & programName)
{
    glLinkProgram(*shaderProgram);

    // Check
    int success;
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(*shaderProgram, sizeof(infoLog), NULL, infoLog);
        throw GameException("ERROR linking " + programName + " shader program: " + std::string(infoLog));
    }
}

GLint RenderContext::GetParameterLocation(
    OpenGLShaderProgram const & shaderProgram,
    std::string const & parameterName)
{
    GLint parameterLocation = glGetUniformLocation(*shaderProgram, parameterName.c_str());
    if (parameterLocation == -1)
    { 
        throw GameException("ERROR retrieving location of parameter \"" + parameterName + "\"");
    }

    return parameterLocation;
}

void RenderContext::DescribeShipPointsVBO()
{
    // Position    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShipPointElement), (void*)(0));
    glEnableVertexAttribArray(0);
    // Color    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShipPointElement), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void RenderContext::CalculateOrthoMatrix()
{
    static constexpr float zFar = 1000.0f;
    static constexpr float zNear = 1.0f;

    mOrthoMatrix[0][0] = 2.0f / mVisibleWorldWidth;
    mOrthoMatrix[1][1] = 2.0f / mVisibleWorldHeight;
    mOrthoMatrix[2][2] = -2.0f / (zFar - zNear);
    mOrthoMatrix[3][0] = -2.0f * mCamX / mVisibleWorldWidth;
    mOrthoMatrix[3][1] = -2.0f * mCamY / mVisibleWorldHeight;
    mOrthoMatrix[3][2] = -(zFar + zNear) / (zFar - zNear);
    mOrthoMatrix[3][3] = 1.0f;
}

void RenderContext::CalculateVisibleWorldCoordinates()
{
    mVisibleWorldHeight = 2.0f * 70.0f / (mZoom + 0.001f);
    mVisibleWorldWidth = static_cast<float>(mCanvasWidth) / static_cast<float>(mCanvasHeight) * mVisibleWorldHeight;
}