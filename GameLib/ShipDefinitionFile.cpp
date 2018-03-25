/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/

#include "ShipDefinitionFile.h"

#include "Utils.h"

ShipDefinitionFile ShipDefinitionFile::Create(picojson::object const & definitionJson)
{
    std::string structuralImageFilePath = Utils::GetMandatoryJsonMember<std::string>(
        definitionJson, 
        "structure_image");

    std::string textureImageFilePath = Utils::GetMandatoryJsonMember<std::string>(
        definitionJson,
        "texture_image");

    std::string shipName = Utils::GetOptionalJsonMember<std::string>(
        definitionJson,
        "ship_name",
        "");

    vec2f offset(0.0f, 0.0f);
    std::optional<picojson::object> offsetObject = Utils::GetOptionalJsonObject(definitionJson, "offset");
    if (!!offsetObject)
    {
        offset.x = Utils::GetMandatoryJsonMember<double>(*offsetObject, "x");
        offset.y = Utils::GetMandatoryJsonMember<double>(*offsetObject, "y");
    }

    return ShipDefinitionFile(
        structuralImageFilePath,
        textureImageFilePath,
        shipName,
        offset);
}
