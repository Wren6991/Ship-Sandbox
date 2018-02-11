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
    static constexpr float DefaultZoom = 75.0f;

	float CamX;
    static constexpr float DefaultCamX = 0.0f;

	float CamY;
    static constexpr float DefaultCamY= 0.0f;

	int CanvasWidth;
	int CanvasHeight;

	bool ShowStress;
	bool UseXRayMode;
	bool QuickWaterFix;

	RenderParameters()
		: Zoom(DefaultZoom)
		, CamX(DefaultCamX)
		, CamY(DefaultCamY)
		, CanvasWidth()
		, CanvasHeight()
		, ShowStress(false)
		, UseXRayMode(false)
		, QuickWaterFix(false)
	{
	}
};

