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
			throw GameException(L"Error: badly formed hex colour value");
			
		return vec3f(
			Hex2Int(str.substr(0, 2)) / 255.f,
			Hex2Int(str.substr(2, 2)) / 255.f,
			Hex2Int(str.substr(4, 2)) / 255.f);
	}
}

Material::Material(picojson::object const & materialJson)
{
	Name = Utils::GetOptionalJsonMember<std::string>(materialJson, "name", "Unspecified");
	Mass = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "mass", 1.0));
	Strength = static_cast<float>(Utils::GetOptionalJsonMember<double>(materialJson, "strength", 1.0) / Mass * 1000.0);
	Colour = Hex2Colour(Utils::GetMandatoryJsonMember<std::string>(materialJson, "colour"));
	IsHull = Utils::GetOptionalJsonMember<bool>(materialJson, "isHull", false);
}
