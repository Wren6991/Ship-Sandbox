/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Utils.h"

#include <picojson/picojson.h>

#include <filesystem>
#include <string>

// At the time of writing, VS 2017 shipped with std::filesystem being still experimental
#ifdef _MSC_VER
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif

/*
 * The content of a ship definition file (.shp)
 */
struct ShipDefinitionFile
{
public:

    // Absolute or relative path
    std::string const StructuralImageFilePath;

    // Absolute or relative path
    std::string const TextureImageFilePath;

    // The name of the ship
    std::string const ShipName;

    static ShipDefinitionFile Create(picojson::object const & definitionJson);

    static bool IsShipDefinitionFile(std::string const & filepath)
    {
        return Utils::ToLower(std::filesystem::path(filepath).extension().string()) == ".shp";
    }

    ShipDefinitionFile(
        std::string structuralImageFilePath,
        std::string textureImageFilePath,
        std::string shipName)
        : StructuralImageFilePath(structuralImageFilePath)
        , TextureImageFilePath(textureImageFilePath)
        , ShipName(shipName)
    {
    }
};
