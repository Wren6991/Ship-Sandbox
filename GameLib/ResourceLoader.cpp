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

// At the time of writing, VS 2017 shipped with std::filesystem being still experimental
#ifdef _MSC_VER
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif

ResourceLoader::ResourceLoader()
{

    // Initialize DevIL
    ilInit();
    iluInit();
}

std::unique_ptr<ResourceLoader::Texture const> ResourceLoader::LoadTextureRgb(std::string const & name)
{
    std::tuple<int, int, unsigned char *> image = LoadImage(
        (std::filesystem::path(std::string("Data")) / "Textures" / name).string(),
        IL_RGB,
        IL_ORIGIN_LOWER_LEFT);

    Texture * texture = new Texture();
    texture->Width = std::get<0>(image);
    texture->Height = std::get<1>(image);
    texture->Data = std::unique_ptr<unsigned char const []>(std::get<2>(image));

    return std::unique_ptr<Texture>(texture);
}

std::unique_ptr<ResourceLoader::Texture const> ResourceLoader::LoadTextureRgba(std::string const & name)
{
    std::tuple<int, int, unsigned char *> image = LoadImage(
        (std::filesystem::path("Data") / "Textures" / name).string(),
        IL_RGBA,
        IL_ORIGIN_LOWER_LEFT);

    Texture * texture = new Texture();
    texture->Width = std::get<0>(image);
    texture->Height = std::get<1>(image);
    texture->Data = std::unique_ptr<unsigned char const[]>(std::get<2>(image));

    return std::unique_ptr<Texture>(texture);
}

std::vector<std::unique_ptr<ResourceLoader::Texture const>> ResourceLoader::LoadTexturesRgba(
    std::string const & prefix,
    ProgressCallback progressCallback)
{
    std::vector<std::string> matchingFilepaths;
    for (auto const & entryIt : std::filesystem::directory_iterator(std::filesystem::path("Data") / "Textures"))
    {
        if (std::filesystem::is_regular_file(entryIt.path())
            && entryIt.path().extension().string() == ".png"
            && 0 == entryIt.path().filename().string().compare(0, prefix.length(), prefix))
        {
            matchingFilepaths.push_back(entryIt.path().string());
        }
    }

    std::vector<std::unique_ptr<ResourceLoader::Texture const>> textures;
    for (size_t i = 0; i < matchingFilepaths.size(); ++i)
    {
        if (progressCallback)
            progressCallback(static_cast<float>(i + 1) / static_cast<float>(matchingFilepaths.size()), "Loading texture...");

        std::tuple<int, int, unsigned char *> image = LoadImage(
            matchingFilepaths[i],
            IL_RGBA,
            IL_ORIGIN_LOWER_LEFT);

        Texture * texture = new Texture();
        texture->Width = std::get<0>(image);
        texture->Height = std::get<1>(image);
        texture->Data = std::unique_ptr<unsigned char const[]>(std::get<2>(image));

        textures.emplace_back(texture);
    }

    return textures;
}

std::unique_ptr<ResourceLoader::StructureImage const> ResourceLoader::LoadStructureImage(std::string const & filepath)
{
    std::tuple<int, int, unsigned char *> image = LoadImage(filepath, IL_RGB, IL_ORIGIN_UPPER_LEFT);

    StructureImage * structureImage = new StructureImage();
    structureImage->Width = std::get<0>(image);
    structureImage->Height = std::get<1>(image);
    structureImage->Data = std::unique_ptr<unsigned char const[]>(std::get<2>(image));

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

std::tuple<int, int, unsigned char *> ResourceLoader::LoadImage(
    std::string const & filepath,
    int format,
    int origin)
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
    if (format != imageFormat || IL_UNSIGNED_BYTE != imageType)
    {
        if (!ilConvertImage(format, IL_UNSIGNED_BYTE))
        {
            ILint devilError = ilGetError();
            std::string devilErrorMessage(iluErrorString(devilError));
            throw GameException("Could not convert image \"" + filepath + "\": " + devilErrorMessage);
        }
    }

    int imageOrigin = ilGetInteger(IL_IMAGE_ORIGIN);
    if (imageOrigin != origin)
    {
        iluFlipImage();
    }


    //
    // Create data
    //

    ILubyte const * imageData = ilGetData();
    int const width = ilGetInteger(IL_IMAGE_WIDTH);
    int const height = ilGetInteger(IL_IMAGE_HEIGHT);
    int const bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

    unsigned char * data = new unsigned char[width * height * bpp];
    std::memcpy(data, imageData, width * height * bpp);


    //
    // Delete image
    //

    ilDeleteImage(imghandle);

    return std::tuple<int, int, unsigned char *>(width, height, data);
}
