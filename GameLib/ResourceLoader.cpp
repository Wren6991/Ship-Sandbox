/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-18
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "ResourceLoader.h"

#include "GameException.h"
#include "Log.h"
#include "ShipDefinitionFile.h"
#include "Utils.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <cstring>

ResourceLoader::ResourceLoader()
{
    // Initialize DevIL
    ilInit();
    iluInit();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Ships
////////////////////////////////////////////////////////////////////////////////////////////

ShipDefinition ResourceLoader::LoadShipDefinition(std::string const & filepath)
{    
    if (ShipDefinitionFile::IsShipDefinitionFile(filepath))
    {
        // 
        // Load full definition
        //

        picojson::value root = Utils::ParseJSONFile(filepath);
        if (!root.is<picojson::object>())
        {
            throw GameException("File \"" + filepath + "\" does not contain a JSON object");
        }

        ShipDefinitionFile sdf = ShipDefinitionFile::Create(root.get<picojson::object>());

        //
        // Make paths absolute
        //

        std::filesystem::path basePath = std::filesystem::path(filepath).parent_path();

        std::filesystem::path absoluteStructuralImageFilePath = std::filesystem::absolute(
            sdf.StructuralImageFilePath,
            basePath);

        std::filesystem::path absoluteTextureImageFilePath = std::filesystem::absolute(
            sdf.TextureImageFilePath,
            basePath);


        //
        // Load
        //

        return ShipDefinition(
            LoadImage(absoluteStructuralImageFilePath.string(), IL_RGB, IL_ORIGIN_UPPER_LEFT),
            LoadTextureRgba(absoluteTextureImageFilePath),
            sdf.ShipName.empty() 
                ? std ::filesystem::path(filepath).stem().string() 
                : sdf.ShipName,
            sdf.Offset);
    }
    else
    {
        //
        // Assume it's just a structural image
        //
        
        auto imageData = LoadImage(filepath, IL_RGB, IL_ORIGIN_UPPER_LEFT);

        return ShipDefinition(
            std::move(imageData),
            std::nullopt,
            std::filesystem::path(filepath).stem().string(),
            vec2f(0.0f, 0.0f));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Textures
////////////////////////////////////////////////////////////////////////////////////////////

ImageData ResourceLoader::LoadTextureRgb(std::string const & name)
{
    return LoadImage(
        (std::filesystem::path(std::string("Data")) / "Textures" / name).string(),
        IL_RGB,
        IL_ORIGIN_LOWER_LEFT);
}

ImageData ResourceLoader::LoadTextureRgb(std::filesystem::path const & filePath)
{
    return LoadImage(
        filePath.string(),
        IL_RGB,
        IL_ORIGIN_LOWER_LEFT);
}

ImageData ResourceLoader::LoadTextureRgba(std::string const & name)
{
    return LoadImage(
        (std::filesystem::path("Data") / "Textures" / name).string(),
        IL_RGBA,
        IL_ORIGIN_LOWER_LEFT);
}

ImageData ResourceLoader::LoadTextureRgba(std::filesystem::path const & filePath)
{
    return LoadImage(
        filePath.string(),
        IL_RGBA,
        IL_ORIGIN_LOWER_LEFT);
}

std::vector<ImageData> ResourceLoader::LoadTexturesRgba(
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

    std::vector<ImageData> textures;
    for (size_t i = 0; i < matchingFilepaths.size(); ++i)
    {
        if (progressCallback)
            progressCallback(static_cast<float>(i + 1) / static_cast<float>(matchingFilepaths.size()), "Loading texture...");

        auto imageData = LoadImage(
            matchingFilepaths[i],
            IL_RGBA,
            IL_ORIGIN_LOWER_LEFT);

        textures.emplace_back(std::move(imageData));
    }

    return textures;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Materials
////////////////////////////////////////////////////////////////////////////////////////////

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
// Music
////////////////////////////////////////////////////////////////////////////////////////////

std::string ResourceLoader::GetMusicFilepath(std::string const & filename) const
{
    std::filesystem::path localPath = std::filesystem::path("Data") / "Music" / (filename + ".flac");
    return std::filesystem::absolute(localPath).string();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Sounds
////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> ResourceLoader::GetSoundFilenames() const
{
    std::vector<std::string> filenames;
    for (auto const & entryIt : std::filesystem::directory_iterator(std::filesystem::path("Data") / "Sounds"))
    {
        if (std::filesystem::is_regular_file(entryIt.path())
            && entryIt.path().extension().string() == ".flac")
        {
            filenames.push_back(entryIt.path().stem().string());
        }
    }

    return filenames;
}

std::string ResourceLoader::GetSoundFilepath(std::string const & filename) const
{
    std::filesystem::path localPath = std::filesystem::path("Data") / "Sounds" / (filename + ".flac");
    return std::filesystem::absolute(localPath).string();
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

ImageData ResourceLoader::LoadImage(
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

    auto data = std::make_unique<unsigned char []>(width * height * bpp);
    std::memcpy(data.get(), imageData, width * height * bpp);


    //
    // Delete image
    //

    ilDeleteImage(imghandle);

    return ImageData(
        width, 
        height, 
        std::move(data));
}
