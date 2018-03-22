/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "ShipRenderContext.h"

#include <cstring>

ShipRenderContext::ShipRenderContext()
    // Ship points
    : mShipPointShaderProgram()
    , mShipPointShaderOrthoMatrixParameter(0)
    , mShipPointShaderAmbientLightIntensityParameter(0)
    , mShipElementCount(0u)
    , mShipPointBuffer()
    , mShipPointBufferSize(0u)
    , mShipPointBufferMaxSize(0u)   
    , mShipPointColorVBO()
    , mShipPointTextureCoordinatesVBO()
    , mShipPointVBO()
    // Ship 
    , mShipShaderProgram()
    , mShipShaderOrthoMatrixParameter(0)
    , mShipShaderAmbientLightIntensityParameter(0)
    , mShipSpringBuffers()
    , mShipTriangleBuffers()
    , mShipBufferSizes()
    , mShipBufferMaxSizes()
    , mShipSpringVBO()
    , mShipTriangleVBO()
    // Lamps
    , mShipLampBuffers()
    // Stressed springs
    , mStressedSpringShaderProgram()
    , mStressedSpringShaderAmbientLightIntensityParameter(0)
    , mStressedSpringShaderOrthoMatrixParameter(0)
    , mStressedSpringBuffer()
    , mStressedSpringBufferSize(0u)
    , mStressedSpringBufferMaxSize(0u)
    , mStressedSpringVBO()
{
    // TODO: see if needed
    GLuint tmpGLuint;


    //
    // Ship points
    //

    GLuint shipPointVBOs[2];
    glGenBuffers(2, shipPointVBOs);
    mShipPointColorVBO = shipPointVBOs[0];
    mShipPointVBO = shipPointVBOs[1];


    //
    // Create ship points program
    //

    mShipPointShaderProgram = glCreateProgram();

    char const * shipPointVertexShaderSource = R"(

        // Inputs
        attribute vec4 inputPos;        
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

    GameOpenGL::CompileShader(shipPointVertexShaderSource, GL_VERTEX_SHADER, mShipPointShaderProgram);

    char const * shipPointFragmentShaderSource = R"(

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

    GameOpenGL::CompileShader(shipPointFragmentShaderSource, GL_FRAGMENT_SHADER, mShipPointShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mShipPointShaderProgram, 0, "inputPos");
    glBindAttribLocation(*mShipPointShaderProgram, 1, "inputLight");
    glBindAttribLocation(*mShipPointShaderProgram, 2, "inputWater");
    glBindAttribLocation(*mShipPointShaderProgram, 3, "inputCol");

    // Link
    GameOpenGL::LinkShaderProgram(mShipPointShaderProgram, "Ship Point");

    // Get uniform locations
    mShipPointShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mShipPointShaderProgram, "paramOrthoMatrix");
    mShipPointShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mShipPointShaderProgram, "paramAmbientLightIntensity");

    // Set hardcoded parameters    
    glUseProgram(*mShipPointShaderProgram);
    glUseProgram(0);


    //
    // Create ship program
    //

    mShipShaderProgram = glCreateProgram();

    char const * shipVertexShaderSource = R"(

        // Inputs
        attribute vec4 inputPos;        
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

    GameOpenGL::CompileShader(shipVertexShaderSource, GL_VERTEX_SHADER, mShipShaderProgram);

    char const * shipFragmentShaderSource = R"(

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

    GameOpenGL::CompileShader(shipFragmentShaderSource, GL_FRAGMENT_SHADER, mShipShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mShipShaderProgram, 0, "inputPos");    
    glBindAttribLocation(*mShipShaderProgram, 1, "inputLight");
    glBindAttribLocation(*mShipShaderProgram, 2, "inputWater");
    glBindAttribLocation(*mShipShaderProgram, 3, "inputCol");

    // Link
    GameOpenGL::LinkShaderProgram(mShipShaderProgram, "Ship");

    // Get uniform locations
    mShipShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mShipShaderProgram, "paramOrthoMatrix");
    mShipShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mShipShaderProgram, "paramAmbientLightIntensity");

    // Create VBOs
    GLuint shipVBOs[2];
    glGenBuffers(2, shipVBOs);
    mShipSpringVBO = shipVBOs[0];
    mShipTriangleVBO = shipVBOs[1];

    // Set hardcoded parameters    
    glUseProgram(*mShipShaderProgram);
    glUseProgram(0);


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

    // Create VBOs
    glGenBuffers(1, &tmpGLuint);
    mStressedSpringVBO = tmpGLuint;

    // Set hardcoded parameters    
    glUseProgram(*mStressedSpringShaderProgram);
    glUseProgram(0);
}

ShipRenderContext::~ShipRenderContext()
{
}

//////////////////////////////////////////////////////////////////////////////////

void ShipRenderContext::UploadShipPointVisualAttributes(
    vec3f const * colors,
    vec2f const * textureCoordinates,
    size_t elementCount)
{
    //
    // Upload to GPU right away
    //

    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointColorVBO);
    glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(vec3f), colors, GL_STATIC_DRAW);

    if (!!mShipPointTextureCoordinatesVBO)
    {
        glBindBuffer(GL_ARRAY_BUFFER, *mShipPointTextureCoordinatesVBO);
        glBufferData(GL_ARRAY_BUFFER, elementCount * sizeof(vec2f), textureCoordinates, GL_STATIC_DRAW);
    }    

    // Store size (for later assert)
    mShipElementCount = elementCount;
}

void ShipRenderContext::UploadShipPointsStart(size_t maxPoints)
{
    if (maxPoints > mShipPointBufferMaxSize)
    {
        // Realloc
        mShipPointBuffer.reset();
        mShipPointBuffer.reset(new ShipPointElement[maxPoints]);
        mShipPointBufferMaxSize = maxPoints;
    }

    mShipPointBufferSize = 0u;
}

void ShipRenderContext::UploadShipPointsEnd()
{
    // Upload point buffer 
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);
    glBufferData(GL_ARRAY_BUFFER, mShipPointBufferSize * sizeof(ShipPointElement), mShipPointBuffer.get(), GL_STREAM_DRAW);
}

void ShipRenderContext::RenderShipPoints(
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    assert(mShipPointBufferSize <= mShipPointBufferMaxSize);

    // Use program
    glUseProgram(*mShipPointShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mShipPointShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
    glUniform1f(mShipPointShaderAmbientLightIntensityParameter, ambientLightIntensity);

    // Bind and describe ship points
    DescribeShipPointVBO();

    // Set point size
    glPointSize(0.15f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Draw
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mShipPointBufferSize));

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::UploadShipStart(std::vector<std::size_t> const & connectedComponentsMaxSizes)
{
    if (connectedComponentsMaxSizes.size() != mShipBufferMaxSizes.size())
    {
        // A change in the number of connected components

        mShipSpringBuffers.clear();
        mShipSpringBuffers.resize(connectedComponentsMaxSizes.size());

        mShipTriangleBuffers.clear();
        mShipTriangleBuffers.resize(connectedComponentsMaxSizes.size());

        mShipBufferMaxSizes.clear();
        mShipBufferMaxSizes.resize(connectedComponentsMaxSizes.size());
    }

    for (size_t c = 0; c < connectedComponentsMaxSizes.size(); ++c)
    {
        size_t maxConnectedComponentSprings = connectedComponentsMaxSizes[c] * 8;        
        if (mShipBufferMaxSizes[c].springCount != maxConnectedComponentSprings)
        {
            // A change in the max size of this connected component
            mShipSpringBuffers[c].reset();
            mShipSpringBuffers[c].reset(new ShipSpringElement[maxConnectedComponentSprings]);
            mShipBufferMaxSizes[c].springCount = maxConnectedComponentSprings;
        }

        size_t maxConnectedComponentTriangles = connectedComponentsMaxSizes[c] * 12;
        if (mShipBufferMaxSizes[c].triangleCount != maxConnectedComponentTriangles)
        {
            // A change in the max size of this connected component
            mShipTriangleBuffers[c].reset();
            mShipTriangleBuffers[c].reset(new ShipTriangleElement[maxConnectedComponentTriangles]);
            mShipBufferMaxSizes[c].triangleCount = maxConnectedComponentTriangles;
        }
    }

    mShipBufferSizes.clear();
    mShipBufferSizes.resize(connectedComponentsMaxSizes.size());
}

void ShipRenderContext::UploadShipEnd()
{
    // Nop
}

void ShipRenderContext::UploadLampsStart(size_t connectedComponents)
{
    mShipLampBuffers.clear();
    mShipLampBuffers.resize(connectedComponents);
}

void ShipRenderContext::UploadLampsEnd()
{
    // Nop
}

void ShipRenderContext::RenderShip(
    bool useXRayMode,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    // Use program
    glUseProgram(*mShipShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mShipShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
    glUniform1f(mShipShaderAmbientLightIntensityParameter, ambientLightIntensity);

    // Bind and describe ship points    
    DescribeShipPointVBO();

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    //
    // Process all connected components, from first to last, and draw springs and triangles
    //

    for (size_t c = 0; c < mShipBufferSizes.size(); ++c)
    {
        //
        // Springs
        //

        // Upload elements
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mShipSpringVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mShipBufferSizes[c].springCount * sizeof(ShipSpringElement), mShipSpringBuffers[c].get(), GL_DYNAMIC_DRAW);

        // Draw
        glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mShipBufferSizes[c].springCount), GL_UNSIGNED_INT, 0);


        //
        // Triangles
        //

        if (!useXRayMode)
        {
            // Upload elements
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mShipTriangleVBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mShipBufferSizes[c].triangleCount * sizeof(ShipTriangleElement), mShipTriangleBuffers[c].get(), GL_DYNAMIC_DRAW);

            // Draw
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(3 * mShipBufferSizes[c].triangleCount), GL_UNSIGNED_INT, 0);
        }
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
        mStressedSpringBuffer.reset(new ShipSpringElement[maxSprings]);
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

    // Bind and describe ship points
    DescribeShipPointVBO();

    // Upload stressed springs buffer 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mStressedSpringVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mStressedSpringBufferSize * sizeof(ShipSpringElement), mStressedSpringBuffer.get(), GL_DYNAMIC_DRAW);

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mStressedSpringBufferSize), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////////

void ShipRenderContext::DescribeShipPointVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointVBO);

    // Position    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShipPointElement), (void*)(0));
    glEnableVertexAttribArray(0);
    // Light
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ShipPointElement), (void*)((2) * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Water
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ShipPointElement), (void*)((2 + 1) * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, *mShipPointColorVBO);

    // Color    
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), (void*)(0));
    glEnableVertexAttribArray(3);
}
