/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <stdexcept>
#include <string>

class GameException : public std::runtime_error
{
public:

	GameException(std::wstring const & errorMessage)
		: std::runtime_error(reinterpret_cast<char const *>(errorMessage.c_str()))
	{}

	std::wstring GetErrorMessage() const
	{
		return std::wstring(reinterpret_cast<wchar_t const *>(this->what()));
	}
};