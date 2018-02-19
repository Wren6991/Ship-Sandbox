/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-13
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "RenderContext.h"

#include "GameException.h"

#include <cstring>

RenderContext::RenderContext(std::shared_ptr<ResourceLoader> resourceLoader)
    : mResourceLoader(std::move(resourceLoader))
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
    // Land 
    //

    // Load texture
    mLandTextureData = mResourceLoader->LoadTextureRgb("sand_1.jpg");

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
    glGenTextures(1, &tmpGLuint);
    mLandTexture = tmpGLuint;

    glBindTexture(GL_TEXTURE_2D, *mLandTexture);

    // Set repeat mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mLandTextureData->Width, mLandTextureData->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mLandTextureData->Data);

    glBindTexture(GL_TEXTURE_2D, 0);


    //
    // Water 
    //

    // Load texture
    mWaterTextureData = mResourceLoader->LoadTextureRgb("water_1.jpg");

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
    glGenTextures(1, &tmpGLuint);
    mWaterTexture = tmpGLuint;

    glBindTexture(GL_TEXTURE_2D, *mWaterTexture);

    // Set repeat mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaterTextureData->Width, mWaterTextureData->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, mWaterTextureData->Data);

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

    glUseProgram(*mShipTriangleShaderProgram);

    // Set hardcoded parameters    
        
    glUseProgram(0);

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
}

RenderContext::~RenderContext()
{
    glUseProgram(0u);
}

//////////////////////////////////////////////////////////////////////////////////

void RenderContext::RenderStart()
{
    //
    // Clear canvas 
    //

    static const vec3f ClearColorBase(0.529f, 0.808f, 0.980f); // (cornflower blue)
    vec3f clearColor = ClearColorBase * mAmbientLightIntensity;
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set anti-aliasing for lines and polygons
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH, GL_NICEST);
}

void RenderContext::RenderLandStart(size_t slices)
{
    if (slices + 1 != mLandBufferMaxSize)
    {
        // Realloc
        mLandBuffer.reset();
        mLandBuffer.reset(new LandElement[slices + 1]);
        mLandBufferMaxSize = slices + 1;
    }

    mLandBufferSize = 0u;    
}

void RenderContext::RenderLandEnd()
{
    assert(mLandBufferSize == mLandBufferMaxSize);

    // Use program
    glUseProgram(*mLandShaderProgram);

    // Set parameters
    glUniform1f(mLandShaderAmbientLightIntensityParameter, mAmbientLightIntensity);
    glUniformMatrix4fv(mLandShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, *mLandTexture);

    // Upload land buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mLandVBO);
    glBufferData(GL_ARRAY_BUFFER, mLandBufferSize * sizeof(LandElement), mLandBuffer.get(), GL_DYNAMIC_DRAW);

    // Describe InputPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(2 * mLandBufferSize));

    // Stop using program
    glUseProgram(0);
}

void RenderContext::RenderWaterStart(size_t slices)
{
    if (slices + 1 != mWaterBufferMaxSize)
    {
        // Realloc
        mWaterBuffer.reset();
        mWaterBuffer.reset(new WaterElement[slices + 1]);
        mWaterBufferMaxSize = slices + 1;
    }

    mWaterBufferSize = 0u;
}

void RenderContext::RenderWaterEnd()
{
    assert(mWaterBufferSize == mWaterBufferMaxSize);

    // Use program
    glUseProgram(*mWaterShaderProgram);

    // Set parameters
    glUniform1f(mWaterShaderAmbientLightIntensityParameter, mAmbientLightIntensity);
    glUniform1f(mWaterShaderWaterTransparencyParameter, mWaterTransparency);
    glUniformMatrix4fv(mWaterShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind Texture
    glBindTexture(GL_TEXTURE_2D, *mWaterTexture);

    // Upload water buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mWaterVBO);
    glBufferData(GL_ARRAY_BUFFER, mWaterBufferSize * sizeof(WaterElement), mWaterBuffer.get(), GL_DYNAMIC_DRAW);

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
    assert(mShipPointBufferSize == mShipPointBufferMaxSize);

    // Upload point buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    glBufferData(GL_ARRAY_BUFFER, mShipPointBufferSize * sizeof(ShipPointElement), mShipPointBuffer.get(), GL_DYNAMIC_DRAW);
}

void RenderContext::RenderShipPoints()
{
    assert(mShipPointBufferSize == mShipPointBufferMaxSize);

    // Use program
    glUseProgram(*mShipPointShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mShipPointShaderOrthoMatrixParameter, 1, GL_FALSE, &(mOrthoMatrix[0][0]));

    // Bind ship points
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    DescribeShipPointsVBO();

    // Set point size
    glPointSize(0.15f * 2.0f * mCanvasHeight / mWorldHeight);

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
    assert(mSpringBufferSize == mSpringBufferMaxSize);

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
    glLineWidth(0.1f * 2.0f * mCanvasHeight / mWorldHeight);

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
    glLineWidth(0.1f * 2.0f * mCanvasHeight / mWorldHeight);

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
    assert(mShipTriangleBufferSize == mShipTriangleBufferMaxSize);
    
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

    mOrthoMatrix[0][0] = 2.0f / mWorldWidth;
    mOrthoMatrix[1][1] = 2.0f / mWorldHeight;
    mOrthoMatrix[2][2] = -2.0f / (zFar - zNear);
    mOrthoMatrix[3][0] = -2.0f * mCamX / mWorldWidth; 
    mOrthoMatrix[3][1] = -2.0f * mCamY / mWorldHeight; 
    mOrthoMatrix[3][2] = -(zFar + zNear) / (zFar - zNear);
    mOrthoMatrix[3][3] = 1.0f;
}

void RenderContext::CalculateWorldCoordinates()
{
    mWorldHeight = 2.0f * 70.0f / (mZoom + 0.001f);
    mWorldWidth = static_cast<float>(mCanvasWidth) / static_cast<float>(mCanvasHeight) * mWorldHeight;
}