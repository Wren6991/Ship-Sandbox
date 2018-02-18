/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-21
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <glad/glad.h>
//#include <GL/gl.h>

#include <stdexcept>

inline void InitOpenGL()
{
    int status = gladLoadGL();
    if (!status)
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}