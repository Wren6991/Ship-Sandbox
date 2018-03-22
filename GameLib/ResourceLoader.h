/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ImageData.h"
#include "Material.h"
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

    ShipDefinition LoadShipDefinition(std::string const & filepath);


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

    std::vector<std::unique_ptr<Material const>> LoadMaterials();


    //
    // Music
    //

    std::string GetMusicFilepath(std::string const & filename) const;


    //
    // Sounds
    //

    std::vector<std::string> GetSoundFilenames() const;

    std::string GetSoundFilepath(std::string const & filename) const;

private:

    ImageData LoadImage(
        std::string const & filepath,
        int format,
        int origin);
};
