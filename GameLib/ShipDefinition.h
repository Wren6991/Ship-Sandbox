/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ImageData.h"

#include <memory>
#include <optional>
#include <string>

/*
* The complete definition of a ship.
*/
struct ShipDefinition
{
public:

    ImageData StructuralImage;

    std::optional<ImageData> TextureImage;

    std::string const ShipName;

    ShipDefinition(
        ImageData structuralImage,
        std::optional<ImageData> textureImage,
        std::string shipName)
        : StructuralImage(std::move(structuralImage))
        , TextureImage(std::move(textureImage))
        , ShipName(shipName)
    {
    }
};
