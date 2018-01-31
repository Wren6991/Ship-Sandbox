/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "_Material.h"

#include "GameException.h"
#include "Utils.h"

#include <sstream>

namespace /* anonymous */ {

	int Hex2Int(std::string const & str)
	{
		std::stringstream ss;
		ss << std::hex << str;
		int x;
		ss >> x;
		return x;
	}

	vec3f Hex2Colour(std::string str)    //  e.g. "#00FF00";
	{
		if (str[0] == '#')
			str = str.substr(1);

		if (str.length() != 6)
			throw GameException(L"Error: badly formed hex colour value \"" + Utils::ConvertAsciiString(str) + L"\"");
			
		return vec3f(
			Hex2Int(str.substr(0, 2)) / 255.f,
			Hex2Int(str.substr(2, 2)) / 255.f,
			Hex2Int(str.substr(4, 2)) / 255.f);
	}
}

std::unique_ptr<Material> Material::Create(picojson::object const & materialJson)
{
	std::string name = Utils::GetOptionalJsonMember<std::string>(materialJson, "name", "Unspecified");
	float mass = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "mass", 1.0));
	float strength = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "strength", 1.0) / mass * 1000.0);
	vec3f colour = Hex2Colour(Utils::GetMandatoryJsonMember<std::string>(materialJson, "colour"));
	bool isHull = Utils::GetOptionalJsonMember<bool>(materialJson, "isHull", false);

	std::optional<_ElectricalProperties> electricalProperties;
	std::optional<picojson::object> electricalPropertiesJson = Utils::GetOptionalJsonObject(materialJson, "electrical_properties");
	if (!!electricalPropertiesJson)
	{
		_ElectricalProperties::ElectricalElementType elementType;
		std::string elementTypeJson = Utils::GetMandatoryJsonMember<std::string>(*electricalPropertiesJson, "element_type");
		if (elementTypeJson == "Lamp")
			elementType = _ElectricalProperties::ElectricalElementType::Lamp;
		else if (elementTypeJson == "Cable")
			elementType = _ElectricalProperties::ElectricalElementType::Cable;
		else if (elementTypeJson == "Generator")
			elementType = _ElectricalProperties::ElectricalElementType::Generator;
		else
			throw GameException("Electrical element_type \"" + elementTypeJson + "\" is not recognized");

		float resistance = static_cast<float>(Utils::GetOptionalJsonMember<double>(*electricalPropertiesJson, "resistance", 0.0));
		float generatedVoltage = static_cast<float>(Utils::GetOptionalJsonMember<double>(*electricalPropertiesJson, "generated_voltage", 0.0));

		electricalProperties.emplace(
			elementType,
			resistance,
			generatedVoltage);
	}

	return std::unique_ptr<Material>(
		new Material(
			name,
			strength,
			mass,
			colour,
			isHull,
			electricalProperties));
}
