/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <stdexcept>
#include <string>

class GameException : public std::exception
{
public:

	GameException(std::wstring const & errorMessage)
		: mErrorMessage(errorMessage)
	{}

	std::wstring GetErrorMessage() const
	{
		return mErrorMessage;
	}

private:

	std::wstring const mErrorMessage;
};