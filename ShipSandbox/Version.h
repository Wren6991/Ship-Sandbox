/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-21
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cassert>
#include <string>

#define VERSION "2.0 Alpha"

enum class VersionFormat
{
	Short,
    Medium,
	Long
};

inline std::string GetVersionInfo(VersionFormat versionFormat)
{
	switch (versionFormat)
	{
		case VersionFormat::Short:
		{
			return std::string(VERSION);
		}

        case VersionFormat::Medium:
        {
            return std::string("Ship Sandbox v" VERSION);
        }

		case VersionFormat::Long:
		{
			return std::string("Ship Sandbox v" VERSION "\n(c) Luke Wren 2013\n(c) Gabriele Giuseppini 2018\nThis version licensed to Mattia");
		}

		default:
		{
			assert(false);
			return "";
		}
	}
}
