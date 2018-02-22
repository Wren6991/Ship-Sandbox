/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class ResourceLoader
{
public:

    ResourceLoader();

public:

    struct Texture
    {
        int Width;
        int Height;
        unsigned char const * Data;
    };

    std::unique_ptr<Texture const> LoadTextureRgb(std::string const & name);
    std::unique_ptr<Texture const> LoadTextureRgba(std::string const & name);

    struct StructureImage
    {
        int Width;
        int Height;
        unsigned char const * Data;
    };

    std::unique_ptr<StructureImage const> LoadStructureImage(std::string const & filepath);

    std::vector<std::unique_ptr<Material const>> LoadMaterials();

private:

    std::tuple<int, int, unsigned char *> LoadImage(
        std::string const & filepath,
        int format,
        int origin);
};
