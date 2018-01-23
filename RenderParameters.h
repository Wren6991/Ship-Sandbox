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
	bool ShowStress;
	bool UseXRayMode;

	float Zoom;
	float CamX;
	float CamY;

	int CanvasWidth;
	int CanvasHeight;

	RenderParameters()
		: ShowStress(false)
		, UseXRayMode(false)
		, Zoom(75.0f)
		, CamX(0.0f)
		, CamY(0.0f)
		, CanvasWidth()
		, CanvasHeight()
	{
	}
};

