/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-22
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameOpenGL.h"

#include "GameException.h"

void GameOpenGL::CompileShader(
    char const * shaderSource,
    GLenum shaderType,
    GameOpenGLShaderProgram const & shaderProgram)
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
        throw GameException("Error Compiling vertex shader: " + std::string(infoLog));
    }

    // Attach to program
    glAttachShader(*shaderProgram, shader);

    // Delete shader
    glDeleteShader(shader);
}

void GameOpenGL::LinkShaderProgram(
    GameOpenGLShaderProgram const & shaderProgram,
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
        throw GameException("Error linking " + programName + " shader program: " + std::string(infoLog));
    }
}

GLint GameOpenGL::GetParameterLocation(
    GameOpenGLShaderProgram const & shaderProgram,
    std::string const & parameterName)
{
    GLint parameterLocation = glGetUniformLocation(*shaderProgram, parameterName.c_str());
    if (parameterLocation == -1)
    {
        throw GameException("ERROR retrieving location of parameter \"" + parameterName + "\"");
    }

    return parameterLocation;
}

