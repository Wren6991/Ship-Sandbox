/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"
#include "ProgressCallback.h"

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
    // Textures
    //

    struct Texture
    {
        int Width;
        int Height;
        std::unique_ptr<unsigned char const []> Data;
    };

    std::unique_ptr<Texture const> LoadTextureRgb(std::string const & name);

    std::unique_ptr<Texture const> LoadTextureRgba(std::string const & name);

    std::vector<std::unique_ptr<Texture const>> LoadTexturesRgba(
        std::string const & prefix, 
        ProgressCallback progressCallback);

    //
    // Structure images
    //

    struct StructureImage
    {
        int Width;
        int Height;
        std::unique_ptr<unsigned char const[]> Data;
    };

    std::unique_ptr<StructureImage const> LoadStructureImage(std::string const & filepath);


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

private:

    std::tuple<int, int, unsigned char *> LoadImage(
        std::string const & filepath,
        int format,
        int origin);
};
