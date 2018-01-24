/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

/*
* Parameters that affect the rendering of the game.
*/
struct RenderParameters
{
	float Zoom;
	float CamX;
	float CamY;

	int CanvasWidth;
	int CanvasHeight;

	bool ShowStress;
	bool UseXRayMode;
	bool QuickWaterFix;

	RenderParameters()
		: Zoom(75.0f)
		, CamX(0.0f)
		, CamY(0.0f)
		, CanvasWidth()
		, CanvasHeight()
		, ShowStress(false)
		, UseXRayMode(false)
		, QuickWaterFix(false)
	{
	}
};

