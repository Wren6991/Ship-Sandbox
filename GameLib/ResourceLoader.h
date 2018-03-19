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
#include <memory>
#include <string>
#include <vector>

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

    ImageData LoadTextureRgba(std::string const & name);

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
