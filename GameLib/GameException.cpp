/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameException.h"

#include "Utils.h"

GameException::GameException(std::string const & errorMessage)
	: mErrorMessage(Utils::ConvertAsciiString(errorMessage))
{
}
