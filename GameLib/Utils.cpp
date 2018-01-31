/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Utils.h"

#include "GameException.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace /* anonymous */ {

	std::string GetFileContents(std::wstring const & filename)
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

namespace Utils {

	picojson::value ParseJSONFile(std::wstring const & filename)
	{
		std::string fileContents = GetFileContents(filename);

		picojson::value jsonContent;
		std::string parseError = picojson::parse(jsonContent, fileContents);
		if (!parseError.empty())
		{
			throw GameException(L"File \"" + filename + L"\" does not contain valid JSON");
		}

		return jsonContent;
	}

}