/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Material.h"

#include "GameException.h"
#include "Utils.h"

#include <sstream>

std::unique_ptr<Material> Material::Create(picojson::object const & materialJson)
{
	std::string name = Utils::GetOptionalJsonMember<std::string>(materialJson, "name", "Unspecified");
	float mass = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "mass", 1.0));
	float strength = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "strength", 1.0) / mass * 1000.0);
    std::array<uint8_t, 3u> rgbColour = Hex2RgbColour(Utils::GetMandatoryJsonMember<std::string>(materialJson, "colour"));
	bool isHull = Utils::GetOptionalJsonMember<bool>(materialJson, "isHull", false);

	std::optional<ElectricalProperties> electricalProperties;
	std::optional<picojson::object> electricalPropertiesJson = Utils::GetOptionalJsonObject(materialJson, "electrical_properties");
	if (!!electricalPropertiesJson)
	{
		ElectricalProperties::ElectricalElementType elementType;
		std::string elementTypeJson = Utils::GetMandatoryJsonMember<std::string>(*electricalPropertiesJson, "element_type");
		if (elementTypeJson == "Lamp")
			elementType = ElectricalProperties::ElectricalElementType::Lamp;
		else if (elementTypeJson == "Cable")
			elementType = ElectricalProperties::ElectricalElementType::Cable;
		else if (elementTypeJson == "Generator")
			elementType = ElectricalProperties::ElectricalElementType::Generator;
		else
			throw GameException("Electrical element_type \"" + elementTypeJson + "\" is not recognized");

		float resistance = static_cast<float>(Utils::GetOptionalJsonMember<double>(*electricalPropertiesJson, "resistance", 0.0));
		float generatedVoltage = static_cast<float>(Utils::GetOptionalJsonMember<double>(*electricalPropertiesJson, "generated_voltage", 0.0));

		electricalProperties.emplace(
			elementType,
			resistance,
			generatedVoltage);
	}

    std::optional<SoundProperties> soundProperties;
    std::optional<picojson::object> soundPropertiesJson = Utils::GetOptionalJsonObject(materialJson, "sound_properties");
    if (!!soundPropertiesJson)
    {
        SoundProperties::SoundElementType elementType;
        std::string elementTypeJson = Utils::GetMandatoryJsonMember<std::string>(*soundPropertiesJson, "element_type");
        if (elementTypeJson == "Glass")
            elementType = SoundProperties::SoundElementType::Glass;
        else if (elementTypeJson == "Metal")
            elementType = SoundProperties::SoundElementType::Metal;
        else if (elementTypeJson == "Wood")
            elementType = SoundProperties::SoundElementType::Wood;
        else
            throw GameException("Sound element_type \"" + elementTypeJson + "\" is not recognized");

        soundProperties.emplace(
            elementType);
    }

	return std::unique_ptr<Material>(
		new Material(
			name,
			strength,
			mass,
            rgbColour,
			isHull,
			electricalProperties,
            soundProperties));
}

uint8_t Material::Hex2Byte(std::string const & str)
{
    std::stringstream ss;
    ss << std::hex << str;
    int x;
    ss >> x;

    return static_cast<uint8_t>(x);
}

std::array<uint8_t, 3u> Material::Hex2RgbColour(std::string str)    //  e.g. "#00FF00";
{
    if (str[0] == '#')
        str = str.substr(1);

    if (str.length() != 6)
        throw GameException("Error: badly formed hex colour value \"" + str + "\"");

    std::array<uint8_t, 3u> rgbColour;
    rgbColour[0] = Hex2Byte(str.substr(0, 2));
    rgbColour[1] = Hex2Byte(str.substr(2, 2));
    rgbColour[2] = Hex2Byte(str.substr(4, 2));

    return rgbColour;
}

vec3f Material::RgbToVec(std::array<uint8_t, 3u> const & rgbColour)    
{
    return vec3f(
        rgbColour[0] / 255.f,
        rgbColour[1] / 255.f,
        rgbColour[2] / 255.f);
}
