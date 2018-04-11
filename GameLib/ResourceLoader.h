/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ImageData.h"
#include "MaterialDatabase.h"
#include "ProgressCallback.h"
#include "ShipDefinition.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

// At the time of writing, VS 2017 shipped with std::filesystem being still experimental
#ifdef _MSC_VER
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif

class ResourceLoader
{
public:

    ResourceLoader();

public:

    //
    // Ships
    //

    ShipDefinition LoadShipDefinition(std::filesystem::path const & filepath);

    std::filesystem::path GetDefaultShipDefinitionFilePath() const;


    //
    // Textures
    //

    ImageData LoadTextureRgb(std::string const & name);

    ImageData LoadTextureRgb(std::filesystem::path const & filePath);

    ImageData LoadTextureRgba(std::string const & name);

    ImageData LoadTextureRgba(std::filesystem::path const & filePath);

    std::vector<ImageData> LoadTexturesRgba(
        std::string const & prefix, 
        ProgressCallback progressCallback);


    //
    // Materials
    //

    MaterialDatabase LoadMaterials();


    //
    // Music
    //

    std::filesystem::path GetMusicFilepath(std::string const & musicName) const;


    //
    // Sounds
    //

    std::vector<std::string> GetSoundNames() const;

    std::filesystem::path GetSoundFilepath(std::string const & soundName) const;


    //
    // Resources
    //

    std::filesystem::path GetCursorFilepath(std::string const & cursorName) const;

    std::filesystem::path GetArtFilepath(std::string const & artName) const;

private:

    ImageData LoadImage(
        std::filesystem::path const & filepath,
        int format,
        int origin);
};
