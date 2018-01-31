/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Vectors.h"

#include <picojson/picojson.h>
#include <string>

class Material
{
public:

    std::string Name;
    float Strength;
    float Mass;
    vec3f Colour;
    bool IsHull;

public:

    Material(picojson::object const & materialJson);
};
