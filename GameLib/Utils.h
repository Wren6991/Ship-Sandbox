/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameException.h"

#include <picojson/picojson.h>

#include <optional>
#include <string>

class Utils 
{
public:

	static picojson::value ParseJSONFile(std::string const & filename);

	template<typename T>
	static T const & GetOptionalJsonMember(
		picojson::object const & obj,
		std::string const & memberName,
		T const & defaultValue)
	{
		auto const & memberIt = obj.find(memberName);
		if (obj.end() == memberIt)
		{
			return defaultValue;
		}

		if (!memberIt->second.is<T>())
		{
			throw GameException("Error parsing JSON: requested member \"" + memberName + "\" is not of the specified type");
		}

		return memberIt->second.get<T>();
	}

	static std::optional<picojson::object> GetOptionalJsonObject(
		picojson::object const & obj,
		std::string const & memberName)
	{
		auto const & memberIt = obj.find(memberName);
		if (obj.end() == memberIt)
		{
			return std::nullopt;
		}

		if (!memberIt->second.is<picojson::object>())
		{
			throw GameException("Error parsing JSON: requested member \"" + memberName + "\" is not of the object type");
		}

		return memberIt->second.get<picojson::object>();
	}

	template<typename T>
	static T const & GetMandatoryJsonMember(
		picojson::object const & obj,
		std::string const & memberName)
	{
		auto const & memberIt = obj.find(memberName);
		if (obj.end() == memberIt)
		{
			throw GameException("Error parsing JSON: cannot find member \"" + memberName + "\"");
		}

		if (!memberIt->second.is<T>())
		{
			throw GameException("Error parsing JSON: requested member \"" + memberName + "\" is not of the specified type");
		}

		return memberIt->second.get<T>();
	}
};