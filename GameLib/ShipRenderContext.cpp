/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "ShipRenderContext.h"

#include "GameException.h"

#include <cstring>

ShipRenderContext::ShipRenderContext(
    std::optional<ImageData> texture,
    vec3f const & ropeColour)
    // Points
    : mPointCount(0u)
    , mPointBuffer()
    , mPointBufferSize(0u)
    , mPointBufferMaxSize(0u)   
    , mPointVBO()
    , mPointColorVBO()
    , mPointElementTextureCoordinatesVBO()
    // Elements
    , mElementColorShaderProgram()
    , mElementColorShaderOrthoMatrixParameter(0)
    , mElementColorShaderAmbientLightIntensityParameter(0)
    , mElementRopeShaderProgram()
    , mElementRopeShaderOrthoMatrixParameter(0)
    , mElementRopeShaderAmbientLightIntensityParameter(0)
    , mElementTextureShaderProgram()
    , mElementTextureShaderOrthoMatrixParameter(0)
    , mElementTextureShaderAmbientLightIntensityParameter(0)
    , mElementStressedSpringShaderProgram(0)
    , mElementStressedSpringShaderOrthoMatrixParameter(0)
    , mPointElementBuffers()
    , mSpringElementBuffers()
    , mRopeElementBuffers()
    , mTriangleElementBuffers()
    , mStressedSpringElementBuffers()
    , mElementBufferSizes()
    , mElementBufferMaxSizes()
    , mPointElementVBO()
    , mSpringElementVBO()
    , mRopeElementVBO()
    , mTriangleElementVBO()
    , mStressedSpringElementVBO()
    , mElementTexture()
    , mElementStressedSpringTexture()
    // Lamps
    , mLampBuffers()
{
    GLuint tmpGLuint;
    GLenum glError;

    // Clear errors
    glError = glGetError();


    //
    // Create point VBOs
    //

    GLuint pointVBOs[3];
    glGenBuffers(3, pointVBOs);
    mPointVBO = pointVBOs[0];
    mPointColorVBO = pointVBOs[1];
    mPointElementTextureCoordinatesVBO = pointVBOs[2];
    


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
    glBindAttribLocation(*mElementColorShaderProgram, InputPosPosition, "inputPos");
    glBindAttribLocation(*mElementColorShaderProgram, InputLightPosition, "inputLight");
    glBindAttribLocation(*mElementColorShaderProgram, InputWaterPosition, "inputWater");
    glBindAttribLocation(*mElementColorShaderProgram, InputColorPosition, "inputCol");

    // Link
    GameOpenGL::LinkShaderProgram(mElementColorShaderProgram, "Ship Color Elements");

    // Get uniform locations
    mElementColorShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementColorShaderProgram, "paramOrthoMatrix");
    mElementColorShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mElementColorShaderProgram, "paramAmbientLightIntensity");


    //
    // Create rope elements program
    //

    mElementRopeShaderProgram = glCreateProgram();

    char const * elementRopeVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;        
        attribute float inputLight;

        // Outputs        
        varying float vertexLight;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {            
            vertexLight = inputLight;

            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(elementRopeVertexShaderSource, GL_VERTEX_SHADER, mElementRopeShaderProgram);

    char const * elementRopeFragmentShaderSource = R"(

        // Inputs from previous shader        
        varying float vertexLight;

        // Params
        uniform vec3 paramRopeColour;
        uniform float paramAmbientLightIntensity;

        // Constants
        vec3 lightColour = vec3(1.0, 1.0, 0.25);

        void main()
        {
            vec3 colour1 = paramRopeColour * paramAmbientLightIntensity;
            colour1 = colour1 * (1.0 - vertexLight) + lightColour * vertexLight;
            
            gl_FragColor = vec4(colour1.xyz, 1.0);
        } 
    )";

    GameOpenGL::CompileShader(elementRopeFragmentShaderSource, GL_FRAGMENT_SHADER, mElementRopeShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mElementRopeShaderProgram, InputPosPosition, "inputPos");
    glBindAttribLocation(*mElementRopeShaderProgram, InputLightPosition, "inputLight");

    // Link
    GameOpenGL::LinkShaderProgram(mElementRopeShaderProgram, "Ship Rope Elements");

    // Get uniform locations
    mElementRopeShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementRopeShaderProgram, "paramOrthoMatrix");
    GLint ropeShaderRopeColourParameter = GameOpenGL::GetParameterLocation(mElementRopeShaderProgram, "paramRopeColour");
    mElementRopeShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mElementRopeShaderProgram, "paramAmbientLightIntensity");

    // Set hardcoded parameters
    glUseProgram(*mElementRopeShaderProgram);
    glUniform3f(
        ropeShaderRopeColourParameter,
        ropeColour.x,
        ropeColour.y,
        ropeColour.z);
    glUseProgram(0);



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
            
            gl_FragColor = vec4(fragColour.xyz, vertexCol.w);
        } 
    )";

    GameOpenGL::CompileShader(elementTextureFragmentShaderSource, GL_FRAGMENT_SHADER, mElementTextureShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mElementTextureShaderProgram, InputPosPosition, "inputPos");
    glBindAttribLocation(*mElementTextureShaderProgram, InputLightPosition, "inputLight");
    glBindAttribLocation(*mElementTextureShaderProgram, InputWaterPosition, "inputWater");
    glBindAttribLocation(*mElementTextureShaderProgram, InputTextureCoordinatesPosition, "inputTextureCoords");

    // Link
    GameOpenGL::LinkShaderProgram(mElementTextureShaderProgram, "Ship Texture Elements");

    // Get uniform locations
    mElementTextureShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementTextureShaderProgram, "paramOrthoMatrix");
    mElementTextureShaderAmbientLightIntensityParameter = GameOpenGL::GetParameterLocation(mElementTextureShaderProgram, "paramAmbientLightIntensity");


    //
    // Create stressed spring program
    //

    mElementStressedSpringShaderProgram = glCreateProgram();

    char const * elementStressedSpringVertexShaderSource = R"(

        // Inputs
        attribute vec2 inputPos;
        
        // Outputs        
        varying vec2 vertexTextureCoords;

        // Params
        uniform mat4 paramOrthoMatrix;

        void main()
        {
            vertexTextureCoords = inputPos; 
            gl_Position = paramOrthoMatrix * vec4(inputPos.xy, -1.0, 1.0);
        }
    )";

    GameOpenGL::CompileShader(elementStressedSpringVertexShaderSource, GL_VERTEX_SHADER, mElementStressedSpringShaderProgram);

    char const * elementStressedSpringFragmentShaderSource = R"(
    
        // Inputs
        varying vec2 vertexTextureCoords;

        // Input texture
        uniform sampler2D inputTexture;

        // Params
        uniform float paramAmbientLightIntensity;

        void main()
        {
            gl_FragColor = texture2D(inputTexture, vertexTextureCoords);
        } 
    )";

    GameOpenGL::CompileShader(elementStressedSpringFragmentShaderSource, GL_FRAGMENT_SHADER, mElementStressedSpringShaderProgram);

    // Bind attribute locations
    glBindAttribLocation(*mElementStressedSpringShaderProgram, 0, "inputPos");

    // Link
    GameOpenGL::LinkShaderProgram(mElementStressedSpringShaderProgram, "Stressed Spring");

    // Get uniform locations
    mElementStressedSpringShaderOrthoMatrixParameter = GameOpenGL::GetParameterLocation(mElementStressedSpringShaderProgram, "paramOrthoMatrix");


    //
    // Create element VBOs
    //

    GLuint elementVBOs[5];
    glGenBuffers(5, elementVBOs);
    mPointElementVBO = elementVBOs[0];
    mSpringElementVBO = elementVBOs[1];
    mRopeElementVBO = elementVBOs[2];
    mTriangleElementVBO = elementVBOs[3];
    mStressedSpringElementVBO = elementVBOs[4];


    //
    // Create and upload ship texture, if present
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

        // Upload texture
        GameOpenGL::UploadMipmappedTexture(std::move(*texture));

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    //
    // Create stressed spring texture
    //

    // Create texture name
    glGenTextures(1, &tmpGLuint);
    mElementStressedSpringTexture = tmpGLuint;

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, *mElementStressedSpringTexture);

    // Set repeat mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Make texture data
    unsigned char buf[] = {
        239, 16, 39, 255,       255, 253, 181,  255,    239, 16, 39, 255,
        255, 253, 181, 255,     239, 16, 39, 255,       255, 253, 181,  255,
        239, 16, 39, 255,       255, 253, 181,  255,    239, 16, 39, 255
    };

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    if (GL_NO_ERROR != glGetError())
    {
        throw GameException("Error uploading stressed spring texture onto GPU");
    }

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
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

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, *mPointColorVBO);

    // Upload data
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(vec3f), colors, GL_STATIC_DRAW);

    // Describe color
    glVertexAttribPointer(InputColorPosition, 3, GL_FLOAT, GL_FALSE, sizeof(vec3f), (void*)(0));
    glEnableVertexAttribArray(InputColorPosition);

    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0u);


    if (!!mElementTexture)
    {
        // Bind VBO
        glBindBuffer(GL_ARRAY_BUFFER, *mPointElementTextureCoordinatesVBO);

        // Upload data
        glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(vec2f), textureCoordinates, GL_STATIC_DRAW);

        // Describe texture coordinates
        glVertexAttribPointer(InputTextureCoordinatesPosition, 2, GL_FLOAT, GL_FALSE, sizeof(vec2f), (void*)(0));
        glEnableVertexAttribArray(InputTextureCoordinatesPosition);

        // Unbind VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0u);
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
        mPointBuffer.reset(new ShipRenderContext::Point[maxPoints]);
        mPointBufferMaxSize = maxPoints;
    }

    mPointBufferSize = 0u;
}

void ShipRenderContext::UploadPointsEnd()
{
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, *mPointVBO);

    // Upload point buffer 
    glBufferData(GL_ARRAY_BUFFER, mPointBufferSize * sizeof(ShipRenderContext::Point), mPointBuffer.get(), GL_STREAM_DRAW);

    // Describe position    
    glVertexAttribPointer(InputPosPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ShipRenderContext::Point), (void*)(0));
    glEnableVertexAttribArray(InputPosPosition);

    // Describe light
    glVertexAttribPointer(InputLightPosition, 1, GL_FLOAT, GL_FALSE, sizeof(ShipRenderContext::Point), (void*)((2) * sizeof(float)));
    glEnableVertexAttribArray(InputLightPosition);

    // Describe water
    glVertexAttribPointer(InputWaterPosition, 1, GL_FLOAT, GL_FALSE, sizeof(ShipRenderContext::Point), (void*)((2 + 1) * sizeof(float)));
    glEnableVertexAttribArray(InputWaterPosition);

    // Unbind VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
}

void ShipRenderContext::UploadElementsStart(std::vector<std::size_t> const & connectedComponentsMaxSizes)
{
    if (connectedComponentsMaxSizes.size() != mElementBufferMaxSizes.size())
    {
        // A change in the number of connected components

        mPointElementBuffers.clear();
        mPointElementBuffers.resize(connectedComponentsMaxSizes.size());

        mSpringElementBuffers.clear();
        mSpringElementBuffers.resize(connectedComponentsMaxSizes.size());

        mRopeElementBuffers.clear();
        mRopeElementBuffers.resize(connectedComponentsMaxSizes.size());

        mTriangleElementBuffers.clear();
        mTriangleElementBuffers.resize(connectedComponentsMaxSizes.size());

        mStressedSpringElementBuffers.clear();
        mStressedSpringElementBuffers.resize(connectedComponentsMaxSizes.size());

        mElementBufferMaxSizes.clear();
        mElementBufferMaxSizes.resize(connectedComponentsMaxSizes.size());
    }

    for (size_t c = 0; c < connectedComponentsMaxSizes.size(); ++c)
    {
        size_t maxConnectedComponentPoints = connectedComponentsMaxSizes[c];
        if (mElementBufferMaxSizes[c].pointCount != maxConnectedComponentPoints)
        {
            // A change in the max size of this connected component
            mPointElementBuffers[c].reset();
            mPointElementBuffers[c].reset(new PointElement[maxConnectedComponentPoints]);
            mElementBufferMaxSizes[c].pointCount = maxConnectedComponentPoints;
        }

        size_t maxConnectedComponentSprings = connectedComponentsMaxSizes[c] * 9;        
        if (mElementBufferMaxSizes[c].springCount != maxConnectedComponentSprings)
        {
            // A change in the max size of this connected component
            mSpringElementBuffers[c].reset();
            mSpringElementBuffers[c].reset(new SpringElement[maxConnectedComponentSprings]);
            mElementBufferMaxSizes[c].springCount = maxConnectedComponentSprings;
        }

        size_t maxConnectedComponentRopes = connectedComponentsMaxSizes[c];
        if (mElementBufferMaxSizes[c].ropeCount != maxConnectedComponentRopes)
        {
            // A change in the max size of this connected component
            mRopeElementBuffers[c].reset();
            mRopeElementBuffers[c].reset(new RopeElement[maxConnectedComponentRopes]);
            mElementBufferMaxSizes[c].ropeCount = maxConnectedComponentRopes;
        }

        size_t maxConnectedComponentTriangles = connectedComponentsMaxSizes[c] * 12;
        if (mElementBufferMaxSizes[c].triangleCount != maxConnectedComponentTriangles)
        {
            // A change in the max size of this connected component
            mTriangleElementBuffers[c].reset();
            mTriangleElementBuffers[c].reset(new TriangleElement[maxConnectedComponentTriangles]);
            mElementBufferMaxSizes[c].triangleCount = maxConnectedComponentTriangles;
        }

        size_t maxConnectedComponentStressedSprings = connectedComponentsMaxSizes[c] * 9;
        if (mElementBufferMaxSizes[c].stressedSpringCount != maxConnectedComponentStressedSprings)
        {
            // A change in the max size of this connected component
            mStressedSpringElementBuffers[c].reset();
            mStressedSpringElementBuffers[c].reset(new StressedSpringElement[maxConnectedComponentStressedSprings]);
            mElementBufferMaxSizes[c].stressedSpringCount = maxConnectedComponentStressedSprings;
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

void ShipRenderContext::Render(
    ShipRenderMode renderMode,
    bool showStressedSprings,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    //
    // Process all connected components, from first to last, and draw all elements
    //

    for (size_t c = 0; c < mElementBufferSizes.size(); ++c)
    {
        //
        // Draw points
        //

        if (renderMode == ShipRenderMode::Points)
        {
            RenderPoints(
                c,
                ambientLightIntensity,
                canvasToVisibleWorldHeightRatio,
                orthoMatrix);
        }


        //
        // Draw springs
        //
        // We draw springs when:
        // - RenderMode is springs ("X-Ray Mode"), in which case we use colors - so to show structural springs -, or
        // - RenderMode is structure (so to draw 1D chains), in which case we use colors, or
        // - RenderMode is texture (so to draw 1D chains), in which case we use texture iff it is present
        //

        if (renderMode == ShipRenderMode::Springs
            || renderMode == ShipRenderMode::Structure
            || renderMode == ShipRenderMode::Texture)
        {
            RenderSprings(
                c,
                renderMode == ShipRenderMode::Texture,
                ambientLightIntensity,
                canvasToVisibleWorldHeightRatio,
                orthoMatrix);
        }


        //
        // Draw ropes now if RenderMode is:
        // - Springs
        // - Texture (so rope endpoints are hidden behind texture, looks better)
        //

        if (renderMode == ShipRenderMode::Springs
            || renderMode == ShipRenderMode::Texture)
        {
            RenderRopes(
                c,
                ambientLightIntensity,
                canvasToVisibleWorldHeightRatio,
                orthoMatrix);
        }


        //
        // Draw triangles
        //

        if (renderMode == ShipRenderMode::Structure
            || renderMode == ShipRenderMode::Texture)
        {
            RenderTriangles(
                c,
                renderMode == ShipRenderMode::Texture,
                ambientLightIntensity,
                orthoMatrix);
        }


        //
        // Draw ropes now if RenderMode is Structure (so rope endpoints on the structure are visible)
        //

        if (renderMode == ShipRenderMode::Structure)
        {
            RenderRopes(
                c,
                ambientLightIntensity,
                canvasToVisibleWorldHeightRatio,
                orthoMatrix);
        }


        //
        // Draw stressed springs
        //

        if (showStressedSprings)
        {
            RenderStressedSprings(
                c,
                canvasToVisibleWorldHeightRatio,
                orthoMatrix);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ShipRenderContext::RenderPoints(
    size_t connectedComponentId,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    // Use color program
    glUseProgram(*mElementColorShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mElementColorShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
    glUniform1f(mElementColorShaderAmbientLightIntensityParameter, ambientLightIntensity);

    // Set point size
    glPointSize(0.2f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Upload points
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mPointElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[connectedComponentId].pointCount * sizeof(PointElement), mPointElementBuffers[connectedComponentId].get(), GL_DYNAMIC_DRAW);

    // Draw
    glDrawElements(GL_POINTS, static_cast<GLsizei>(1 * mElementBufferSizes[connectedComponentId].pointCount), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::RenderSprings(
    size_t connectedComponentId,
    bool withTexture,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    if (withTexture && !!mElementTexture)
    {
        // Use texture program
        glUseProgram(*mElementTextureShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementTextureShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementTextureShaderAmbientLightIntensityParameter, ambientLightIntensity);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, *mElementTexture);
    }
    else
    {
        // Use color program
        glUseProgram(*mElementColorShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementColorShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementColorShaderAmbientLightIntensityParameter, ambientLightIntensity);
    }

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Upload springs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mSpringElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[connectedComponentId].springCount * sizeof(SpringElement), mSpringElementBuffers[connectedComponentId].get(), GL_DYNAMIC_DRAW);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mElementBufferSizes[connectedComponentId].springCount), GL_UNSIGNED_INT, 0);

    // Unbind texture (if any)
    glBindTexture(GL_TEXTURE_2D, 0);

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::RenderRopes(
    size_t connectedComponentId,
    float ambientLightIntensity,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    // Use rope program
    glUseProgram(*mElementRopeShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mElementRopeShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
    glUniform1f(mElementRopeShaderAmbientLightIntensityParameter, ambientLightIntensity);

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Upload ropes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mRopeElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[connectedComponentId].ropeCount * sizeof(RopeElement), mRopeElementBuffers[connectedComponentId].get(), GL_DYNAMIC_DRAW);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mElementBufferSizes[connectedComponentId].ropeCount), GL_UNSIGNED_INT, 0);

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::RenderTriangles(
    size_t connectedComponentId,
    bool withTexture,
    float ambientLightIntensity,
    float(&orthoMatrix)[4][4])
{
    if (withTexture && !!mElementTexture)
    {
        // Use texture program
        glUseProgram(*mElementTextureShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementTextureShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementTextureShaderAmbientLightIntensityParameter, ambientLightIntensity);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, *mElementTexture);
    }
    else
    {
        // Use color program
        glUseProgram(*mElementColorShaderProgram);

        // Set parameters
        glUniformMatrix4fv(mElementColorShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));
        glUniform1f(mElementColorShaderAmbientLightIntensityParameter, ambientLightIntensity);
    }

    // Upload elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mTriangleElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[connectedComponentId].triangleCount * sizeof(TriangleElement), mTriangleElementBuffers[connectedComponentId].get(), GL_DYNAMIC_DRAW);

    // Draw
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(3 * mElementBufferSizes[connectedComponentId].triangleCount), GL_UNSIGNED_INT, 0);

    // Unbind texture (if any)
    glBindTexture(GL_TEXTURE_2D, 0);

    // Stop using program
    glUseProgram(0);
}

void ShipRenderContext::RenderStressedSprings(
    size_t connectedComponentId,
    float canvasToVisibleWorldHeightRatio,
    float(&orthoMatrix)[4][4])
{
    // Use program
    glUseProgram(*mElementStressedSpringShaderProgram);

    // Set parameters
    glUniformMatrix4fv(mElementStressedSpringShaderOrthoMatrixParameter, 1, GL_FALSE, &(orthoMatrix[0][0]));

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, *mElementStressedSpringTexture);

    // Upload elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mStressedSpringElementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mElementBufferSizes[connectedComponentId].stressedSpringCount * sizeof(StressedSpringElement), mStressedSpringElementBuffers[connectedComponentId].get(), GL_DYNAMIC_DRAW);

    // Set line size
    glLineWidth(0.1f * 2.0f * canvasToVisibleWorldHeightRatio);

    // Draw
    glDrawElements(GL_LINES, static_cast<GLsizei>(2 * mElementBufferSizes[connectedComponentId].stressedSpringCount), GL_UNSIGNED_INT, 0);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Stop using program
    glUseProgram(0);
}
