/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

//TBD: my laptop does not support OpenGL 3, hence no need for me to move to glad at this moment.
//#include <glad/glad.h>
#include <GL/gl.h>

#include <stdexcept>

inline void InitOpenGL()
{
    //if (!gladLoadGLLoader((GLADloadproc)GetProcAddress))
    //{
    //    throw std::runtime_error("Failed to initialize GLAD");
    //}
}