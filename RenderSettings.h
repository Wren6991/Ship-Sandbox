/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

/*
* Settings that affect the rendering of the game.
*/
struct RenderSettings
{
	bool ShowStress;
	bool UseXRayMode;

	float Zoom;
	float CamX;
	float CamY;

	int CanvasWidth;
	int CanvasHeight;

	RenderSettings()
		: ShowStress(false)
		, UseXRayMode(false)
		, Zoom(30.0f)
		, CamX(0.0f)
		, CamY(0.0f)
		, CanvasWidth()
		, CanvasHeight()
	{
	}
};

