/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <picojson/picojson.h>

#include <string>

namespace Utils {

	picojson::value ParseJSONFile(std::wstring const & filename);

	template<typename T>
	T const & GetOptionalJsonMember(
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
			throw GameException(L"Error parsing JSON: requested member is not of the specified type");
		}

		return memberIt->second.get<T>();
	}

	template<typename T>
	T const & GetMandatoryJsonMember(
		picojson::object const & obj,
		std::string const & memberName)
	{
		auto const & memberIt = obj.find(memberName);
		if (obj.end() == memberIt)
		{
			throw GameException(L"Error parsing JSON: cannot find requested member");
		}

		if (!memberIt->second.is<T>())
		{
			throw GameException(L"Error parsing JSON: requested member is not of the specified type");
		}

		return memberIt->second.get<T>();
	}
}