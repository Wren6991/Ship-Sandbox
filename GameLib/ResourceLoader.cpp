/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ResourceLoader.h"

#include "GameException.h"
#include "Log.h"
#include "Utils.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <cstring>
#include <filesystem>

namespace std {
    namespace filesystem = experimental::filesystem;
}

ResourceLoader::ResourceLoader()
{
    // Initialize DevIL
    ilInit();
    iluInit();
}

std::unique_ptr<ResourceLoader::Texture const> ResourceLoader::LoadTexture(std::string const & name)
{
    std::tuple<int, int, unsigned char *> image = LoadImage(
        (std::filesystem::path(std::string("Data")) / "Textures" / name).string());

    Texture * texture = new Texture();
    texture->Width = std::get<0>(image);
    texture->Height = std::get<1>(image);
    texture->Data = std::get<2>(image);

    return std::unique_ptr<Texture>(texture);
}

std::unique_ptr<ResourceLoader::StructureImage const> ResourceLoader::LoadStructureImage(std::string const & filepath)
{
    std::tuple<int, int, unsigned char *> image = LoadImage(filepath);

    StructureImage * structureImage = new StructureImage();
    structureImage->Width = std::get<0>(image);
    structureImage->Height = std::get<1>(image);
    structureImage->Data = std::get<2>(image);

    return std::unique_ptr<StructureImage>(structureImage);
}

std::vector<std::unique_ptr<Material const>> ResourceLoader::LoadMaterials()
{
    std::vector<std::unique_ptr<Material const>> materials;

    picojson::value root = Utils::ParseJSONFile("Data/materials.json");
    if (!root.is<picojson::array>())
    {
        throw GameException("File \"Data/materials.json\" does not contain a JSON array");
    }

    picojson::array rootArray = root.get<picojson::array>();
    for (auto const & rootElem : rootArray)
    {
        if (!rootElem.is<picojson::object>())
        {
            throw GameException("File \"Data/materials.json\" does not contain a JSON array of objects");
        }

        materials.emplace_back(Material::Create(rootElem.get<picojson::object>()));
    }

    return materials;
}

////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int, int, unsigned char *> ResourceLoader::LoadImage(std::string const & filepath)
{
    //
    // Load image
    //

    ILuint imghandle;
    ilGenImages(1, &imghandle);
    ilBindImage(imghandle);

    ILconst_string ilFilename(filepath.c_str());
    if (!ilLoadImage(ilFilename))
    {
        ILint devilError = ilGetError();
        std::string devilErrorMessage(iluErrorString(devilError));
        throw GameException("Could not load image \"" + filepath + "\": " + devilErrorMessage);
    }

    //
    // Check if we need to convert it
    //

    int imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
    int imageType = ilGetInteger(IL_IMAGE_TYPE);
    if (IL_RGB != imageFormat || IL_UNSIGNED_BYTE != imageType)
    {
        if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
        {
            ILint devilError = ilGetError();
            std::string devilErrorMessage(iluErrorString(devilError));
            throw GameException("Could not convert image \"" + filepath + "\": " + devilErrorMessage);
        }
    }


    //
    // Create data
    //

    ILubyte const * imageData = ilGetData();
    int const width = ilGetInteger(IL_IMAGE_WIDTH);
    int const height = ilGetInteger(IL_IMAGE_HEIGHT);

    unsigned char * data = new unsigned char[width * height * 3];
    std::memcpy(data, imageData, width * height * 3);


    //
    // Delete image
    //

    ilDeleteImage(imghandle);

    return std::tuple<int, int, unsigned char *>(width, height, data);
}
