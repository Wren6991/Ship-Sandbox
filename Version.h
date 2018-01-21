/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-21
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <cassert>
#include <string>

#define VERSION L"2.0 Alpha"

enum class VersionFormat
{
	Short,
	Long
};

std::wstring GetVersionInfo(VersionFormat versionFormat)
{
	switch (versionFormat)
	{
		case VersionFormat::Short:
		{
			return std::wstring(VERSION);
		}

		case VersionFormat::Long:
		{
			return std::wstring(L"Ship Sandbox Alpha v" VERSION L"\n(c) Luke Wren 2013\n(c) Gabriele Giuseppini 2018\nLicensed to Francis Racicot");
		}

		default:
		{
			assert(false);
			return L"";
		}
	}
}
