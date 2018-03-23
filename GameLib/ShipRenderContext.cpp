/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "ShipRenderContext.h"

#include "GameException.h"

#include <cstring>

ShipRenderContext::ShipRenderContext(std::optional<ImageData> const & texture)
    // Points
    : mPointShaderProgram()
    , mPointShaderOrthoMatrixParameter(0)
    , mPointShaderAmbientLightIntensityParameter(0)
    , mPointCount(0u)
    , mPointBuffer()
    , mPointBufferSize(0u)
    , mPointBufferMaxSize(0u)   
    , mPointColorVBO()
    , mPointElementTextureCoordinatesVBO()
    , mPointVBO()
    // Elements
    , mElementColorShaderProgram()
    , mElementColorShaderOrthoMatrixParameter(0)
    , mElementColorShaderAmbientLightIntensityParameter(0)
    , mElementTextureShaderProgram()
    , mElementTextureShaderOrthoMatrixParameter(0)
    , mElementTextureShaderAmbientLightIntensityParameter(0)
    , mSpringElementBuffers()
    , mTriangleElementBuffers()
    , mElementBufferSizes()
    , mElementBufferMaxSizes()
    , mSpringElementVBO()
    , mTriangleElementVBO()
    , mElementTexture()
    // Lamps
    , mLampBuffers()
    // Stressed springs
    , mStressedSpringShaderProgram()
    , mStressedSpringShaderAmbientLightIntensityParameter(0)
    , mStressedSpringShaderOrthoMatrixParameter(0)
    , mStressedSpringBuffer()
    , mStressedSpringBufferSize(0u)
    , mStressedSpringBufferMaxSize(0u)
    , mStressedSpringVBO()
{
    GLuint tmpGLuint;
    GLenum glError;

    // Clear errors
    glError = glGetError();


    //
    // Create points program
    //

    mPointShaderProgram = glCreateProgram();

    char const * pointVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;        
        attribute float inputLight;
        attribute float inputWater;
        attribute vec3 inputCol;

        // Outputs        
        varying float vertexLight;
        varying float vertexWater;
        varying vec3 vertexCol;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {            
            vertexLight = inputLight;
            vertexWater = inputWater;
            vertexCol = inputCol;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(pointVertexShaderSource, GL_VERTEX_SHADER, mPointShaderProgram);

    char const * pointFragmentShaderSource = R"(

        // Inputs from previous shader        
        varying float vertexLight;
        varying float vertexWater;
        varying vec3 vertexCol;

        // Params
        uniform float paramAmbientLightIntensity;

        // Constants
        vec3 lightColour = vec3(1.0, 1.0, 0.25);
        vec3 wetColour = vec3(0.0, 0.0, 0.8);

        void main()
        {
            float colorWetness = min(vertexWater, 1.0) * 0.7;
            vec3 colour1 = vertexCol * (1.0 - colorWetness) + wetColour * colorWetness;
            colour1 *= paramAmbientLightIntensity;
            colour1 = colour1 * (1.0 - vertexLight) + lightColour * vertexLight;
            
            gl_FragColor = vec4(colour1.xyz, 1.0);
        } 
    )";

    GameOpenGL::CompileShader(pointFragmentShaderSource, GL_FRAGMENT_SHADER, mPointShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mPointShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mPointShaderProgram, 1, "inputLight");
    glBindAttribLocation(*mPointShaderProgram, 2, "inputWater");
    glBindAttribLocation(*mPointShaderProgram, 3, "inputCol");

    // Link
    GameOpenGL::LinkShaderProgram(mPointShaderProgram, "Ship Point");

    // Get uniform locations
    mPointShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mPointShaderProgram, "paramOrthoMatrix");
    mPointShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mPointShaderProgram, "paramAmbientLightIntensity");


    //
    // Create point VBOs
    //

    GLuint pointVBOs[3];
    glGenBuffers(3, pointVBOs);
    mPointColorVBO = pointVBOs[0];
    mPointElementTextureCoordinatesVBO = pointVBOs[1];
    mPointVBO = pointVBOs[2];



    //
    // Create color elements program
    //

    mElementColorShaderProgram = glCreateProgram();

    char const * elementColorVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;        
        attribute float inputLight;
        attribute float inputWater;
        attribute vec3 inputCol;

        // Outputs        
        varying float vertexLight;
        varying float vertexWater;
        varying vec3 vertexCol;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {            
            vertexLight = inputLight;
            vertexWater = inputWater;
            vertexCol = inputCol;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(elementColorVertexShaderSource, GL_VERTEX_SHADER, mElementColorShaderProgram);

    char const * elementColorFragmentShaderSource = R"(

        // Inputs from previous shader        
        varying float vertexLight;
        varying float vertexWater;
        varying vec3 vertexCol;

        // Params
        uniform float paramAmbientLightIntensity;

        // Constants
        vec3 lightColour = vec3(1.0, 1.0, 0.25);
        vec3 wetColour = vec3(0.0, 0.0, 0.8);

        void main()
        {
            float colorWetness = min(vertexWater, 1.0) * 0.7;
            vec3 colour1 = vertexCol * (1.0 - colorWetness) + wetColour * colorWetness;
            colour1 *= paramAmbientLightIntensity;
            colour1 = colour1 * (1.0 - vertexLight) + lightColour * vertexLight;
            
            gl_FragColor = vec4(colour1.xyz, 1.0);
        } 
    )";

    GameOpenGL::CompileShader(elementColorFragmentShaderSource, GL_FRAGMENT_SHADER, mElementColorShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mElementColorShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mElementColorShaderProgram, 1, "inputLight");
    glBindAttribLocation(*mElementColorShaderProgram, 2, "inputWater");
    glBindAttribLocation(*mElementColorShaderProgram, 3, "inputCol");

    // Link
    GameOpenGL::LinkShaderProgram(mElementColorShaderProgram, "Ship Color Elements");

    // Get uniform locations
    mElementColorShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementColorShaderProgram, "paramOrthoMatrix");
    mElementColorShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mElementColorShaderProgram, "paramAmbientLightIntensity");

    
    //
    // Create texture elements program
    //

    mElementTextureShaderProgram = glCreateProgram();

    char const * elementTextureVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;        
        attribute float inputLight;
        attribute float inputWater;
        attribute vec2 inputTextureCoords;

        // Outputs        
        varying float vertexLight;
        varying float vertexWater;
        varying vec2 vertexTextureCoords;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {            
            vertexLight = inputLight;
            vertexWater = inputWater;
            vertexTextureCoords = inputTextureCoords;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(elementTextureVertexShaderSource, GL_VERTEX_SHADER, mElementTextureShaderProgram);

    char const * elementTextureFragmentShaderSource = R"(

        // Inputs from previous shader        
        varying float vertexLight;
        varying float vertexWater;
        varying vec2 vertexTextureCoords;

        // Input texture
        uniform sampler2D inputTexture;

        // Params
        uniform float paramAmbientLightIntensity;

        // Constants
        vec4 lightColour = vec4(1.0, 1.0, 0.25, 1.0);
        vec4 wetColour = vec4(0.0, 0.0, 0.8, 1.0);

        void main()
        {
            vec4 vertexCol = texture2D(inputTexture, vertexTextureCoords);

            // Apply point water
            float colorWetness = min(vertexWater, 1.0) * 0.7;
            vec4 fragColour = vertexCol * (1.0 - colorWetness) + wetColour * colorWetness;

            // Apply ambient light
            fragColour *= paramAmbientLightIntensity;

            // Apply point light
            fragColour = fragColour * (1.0 - vertexLight) + lightColour * vertexLight;
            
            gl_FragColor = fragColour;
        } 
    )";

    GameOpenGL::CompileShader(elementTextureFragmentShaderSource, GL_FRAGMENT_SHADER, mElementTextureShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mElementTextureShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mElementTextureShaderProgram, 1, "inputLight");
    glBindAttribLocation(*mElementTextureShaderProgram, 2, "inputWater");
    glBindAttribLocation(*mElementTextureShaderProgram, 3, "inputTextureCoords");

    // Link
    GameOpenGL::LinkShaderProgram(mElementTextureShaderProgram, "Ship Texture Elements");

    // Get uniform locations
    mElementTextureShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementTextureShaderProgram, "paramOrthoMatrix");
    mElementTextureShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mElementTextureShaderProgram, "paramAmbientLightIntensity");


    //
    // Create element VBOs
    //

    GLuint elementVBOs[2];
    glGenBuffers(2, elementVBOs);
    mSpringElementVBO = elementVBOs[0];
    mTriangleElementVBO = elementVBOs[1];


    //
    // Create and upload texture, if present
    //

    if (!!texture)
    {
        glGenTextures(1, &tmpGLuint);
        mElementTexture = tmpGLuint;

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, *mElementTexture);
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error binding ship texture: " + std::to_string(glError));
        }

        //
        // Configure texture
        //

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error setting wrapping of S coordinate of ship texture: " + std::to_string(glError));
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error setting wrapping of T coordinate of ship texture: " + std::to_string(glError));
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error setting minification filter of ship texture: " + std::to_string(glError));
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error setting magnification filter of ship texture: " + std::to_string(glError));
        }


        //
        // Upload texture
        //

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->Width, texture->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->Data.get());
        glError = glGetError();
        if (GL_NO_ERROR != glError)
        {
            throw GameException("Error uploading ship texture onto GPU: " + std::to_string(glError));
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    //
    // Create stressed spring program
    //

    mStressedSpringShaderProgram = glCreateProgram();

    char const * stressedSpringVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(stressedSpringVertexShaderSource, GL_VERTEX_SHADER, mStressedSpringShaderProgram);

    char const * stressedSpringFragmentShaderSource = R"(

        // Params
        uniform float paramAmbientLightIntensity;

        void main()
        {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * paramAmbientLightIntensity;
        } 
    )";

    GameOpenGL::CompileShader(stressedSpringFragmentShaderSource, GL_FRAGMENT_SHADER, mStressedSpringShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mStressedSpringShaderProgram, 0, "inputPos");

    // Link
    GameOpenGL::LinkShaderProgram(mStressedSpringShaderProgram, "Stressed Spring");

    // Get uniform locations
    mStressedSpringShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mStressedSpringShaderProgram, "paramAmbientLightIntensity");
    mStressedSpringShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mStressedSpringShaderProgram, "paramOrthoMatrix");

    //
    // Create stressed springs VBOs
    //

    glGenBuffers(1, &tmpGLuint);
    mStressedSpringVBO = tmpGLuint;
}

ShipRenderContext::~ShipRenderContext()
{
}

//////////////////////////////////////////////////////////////////////////////////

void ShipRenderContext::UploadPointVisualAttributes(
    vec3f const * colors,
    vec2f const * textureCoordinates,
    size_t pointCount)
{
    //
    // Upload to GPU right away
    //

    glBindBuffer(GL_ARRAY_BUFFER, *mPointColorVBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(vec3f), colors, GL_STATIC_DRAW);

    if (!!mElementTexture)
    {
        glBindBuffer(GL_ARRAY_BUFFER, *mPointElementTextureCoordinatesVBO);
        glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(vec2f), textureCoordinates, GL_STATIC_DRAW);
    }    

    // Store size (for later assert)
    mPointCount = pointCount;
}

void ShipRenderContext::UploadPointsStart(size_t maxPoints)
{
    if (maxPoints > mPointBufferMaxSize)
    {
        // Realloc
        mPointBuffer.reset();
        mPointBuffer.reset(new PointElement[maxPoints]);
        mPointBufferMaxSize = maxPoints;
    }

    mPointBufferSize = 0u;
}

void ShipRenderContext::UploadPointsEnd()
{
    // Upload point buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mPointVBO);
    glBufferData(GL_ARRAY_BUFFER, mPointBufferSize * sizeof(PointElement), mPointBuffer.get(), GL_STREAM_DRAW);
}

void ShipRenderContext::RenderPoints(
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    assert(mPointBufferSize <= mPointBufferMaxSize);

    // Use program
    glUseProgram(*mPointShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mPointShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
    glUniform1f(mPointShaderAmbientLightIntensityParameter, ambientLightIntensity);

    // Bind and describe points
    BindAndDescribePointVBO();

    // Set point size
    glPointSize(0.15f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Draw
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mPointBufferSize));

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::UploadElementsStart(std::vector<std::size_t> const & connectedComponentsMaxSizes)
{
    if (connectedComponentsMaxSizes.size() != mElementBufferMaxSizes.size())
    {
        // A change in the number of connected components

        mSpringElementBuffers.clear();
        mSpringElementBuffers.resize(connectedComponentsMaxSizes.size());

        mTriangleElementBuffers.clear();
        mTriangleElementBuffers.resize(connectedComponentsMaxSizes.size());

        mElementBufferMaxSizes.clear();
        mElementBufferMaxSizes.resize(connectedComponentsMaxSizes.size());
    }

    for (size_t c = 0; c < connectedComponentsMaxSizes.size(); ++c)
    {
        size_t maxConnectedComponentSprings = connectedComponentsMaxSizes[c] * 8;        
        if (mElementBufferMaxSizes[c].springCount != maxConnectedComponentSprings)
        {
            // A change in the max size of this connected component
            mSpringElementBuffers[c].reset();
            mSpringElementBuffers[c].reset(new SpringElement[maxConnectedComponentSprings]);
            mElementBufferMaxSizes[c].springCount = maxConnectedComponentSprings;
        }

        size_t maxConnectedComponentTriangles = connectedComponentsMaxSizes[c] * 12;
        if (mElementBufferMaxSizes[c].triangleCount != maxConnectedComponentTriangles)
        {
            // A change in the max size of this connected component
            mTriangleElementBuffers[c].reset();
            mTriangleElementBuffers[c].reset(new TriangleElement[maxConnectedComponentTriangles]);
            mElementBufferMaxSizes[c].triangleCount = maxConnectedComponentTriangles;
        }
    }

    mElementBufferSizes.clear();
    mElementBufferSizes.resize(connectedComponentsMaxSizes.size());
}

void ShipRenderContext::UploadElementsEnd()
{
    // Nop
}

void ShipRenderContext::UploadLampsStart(size_t connectedComponents)
{
    mLampBuffers.clear();
    mLampBuffers.resize(connectedComponents);
}

void ShipRenderContext::UploadLampsEnd()
{
    // Nop
}

void ShipRenderContext::RenderElements(
    bool useXRayMode,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    if (!mElementTexture)
    {
        // Use color program
        glUseProgram(*mElementColorShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementColorShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementColorShaderAmbientLightIntensityParameter, ambientLightIntensity);
    }
    else
    { 
        // Use texture program
        glUseProgram(*mElementTextureShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementTextureShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementTextureShaderAmbientLightIntensityParameter, ambientLightIntensity);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, *mElementTexture);
    }

    // Bind and describe points    
    BindAndDescribePointVBO();

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    //
    // Process all connected components, from first to last, and draw springs and triangles
    //

    for (size_t c = 0; c < mElementBufferSizes.size(); ++c)
    {
        //
        // Springs
        //

        // Upload elements
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mSpringElementVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[c].springCount * sizeof(SpringElement), mSpringElementBuffers[c].get(), GL_DYNAMIC_DRAW);

        // Draw
        glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mElementBufferSizes[c].springCount), GL_UNSIGNED_INT, 0);


        //
        // Triangles
        //

        if (!useXRayMode)
        {
            // Upload elements
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mTriangleElementVBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[c].triangleCount * sizeof(TriangleElement), mTriangleElementBuffers[c].get(), GL_DYNAMIC_DRAW);

            // Draw
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(3 * mElementBufferSizes[c].triangleCount), GL_UNSIGNED_INT, 0);
        }
    }

    if (!!mElementTexture)
    {
        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::RenderStressedSpringsStart(size_t maxSprings)
{
    if (maxSprings > mStressedSpringBufferMaxSize)
    {
        // Realloc
        mStressedSpringBuffer.reset();
        mStressedSpringBuffer.reset(new SpringElement[maxSprings]);
        mStressedSpringBufferMaxSize = maxSprings;
    }

    mStressedSpringBufferSize = 0u;
}

void ShipRenderContext::RenderStressedSpringsEnd(
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    // Use program
    glUseProgram(*mStressedSpringShaderProgram);

    // Set parameters
    glUniform1f(mStressedSpringShaderAmbientLightIntensityParameter, ambientLightIntensity);
    glUniformMatrix4fv(mStressedSpringShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));

    // Bind and describe points
    BindAndDescribePointVBO();

    // Upload stressed springs buffer 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mStressedSpringVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mStressedSpringBufferSize * sizeof(SpringElement), mStressedSpringBuffer.get(), GL_DYNAMIC_DRAW);

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mStressedSpringBufferSize), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////////

void ShipRenderContext::BindAndDescribePointVBO()
{
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, *mPointVBO);

    // Position    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PointElement), (void*)(0));
    glEnableVertexAttribArray(0);
    // Light
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(PointElement), (void*)((2) * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Water
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(PointElement), (void*)((2 + 1) * sizeof(float)));
    glEnableVertexAttribArray(2);


    if (!mElementTexture)
    {
        //
        // Color
        //

        glBindBuffer(GL_ARRAY_BUFFER, *mPointColorVBO);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), (void*)(0));
    }
    else
    {
        //
        // Texture
        //

        glBindBuffer(GL_ARRAY_BUFFER, *mPointElementTextureCoordinatesVBO);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2f), (void*)(0));        
    }

    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0u);
}
