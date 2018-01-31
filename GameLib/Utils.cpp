/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Utils.h"

#include "GameException.h"

#include <cwchar>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace /* anonymous */ {

	std::string GetTextFileContents(std::wstring const & filename)
	{
		
		std::ifstream file(filename.c_str(), std::ios::in);
		if (!file.is_open())
		{
			throw GameException(L"Cannot open file \"" + filename + L"\"");
		}

		std::stringstream ss;
		ss << file.rdbuf();

		return ss.str();
	}
}

std::wstring Utils::ConvertAsciiString(std::string const & asciiStr)
{
	std::mbstate_t state = std::mbstate_t();
	char const * asciiStrData = asciiStr.data();
	std::size_t len = 1 + std::mbsrtowcs(NULL, &asciiStrData, asciiStr.size(), &state);
	std::wstring wstr;
	wstr.resize(len);
	std::mbsrtowcs(&(wstr[0]), &asciiStrData, wstr.size(), &state);

	return wstr;
}

picojson::value Utils::ParseJSONFile(std::wstring const & filename)
{
	std::string fileContents = GetTextFileContents(filename);

	picojson::value jsonContent;
	std::string parseError = picojson::parse(jsonContent, fileContents);
	if (!parseError.empty())
	{
		throw GameException(L"Error parsing JSON file \"" + filename + L"\": " + ConvertAsciiString(parseError));
	}

	return jsonContent;
}
