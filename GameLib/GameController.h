/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Game.h"
#include "GameParameters.h"
#include "RenderParameters.h"
#include "vec.h"

#include <memory>
#include <string>

/*
 * This class is responsible for managing the game, from its lifetime to the user
 * interactions.
 */
class GameController
{
public:

	static std::unique_ptr<GameController> Create();

	void ResetAndLoadShip(std::wstring const & filepath);
    void AddShip(std::wstring const & filepath);
	void ReloadLastShip();

	void DoStep();
	void Render();


	//
	// Interactions
	//

	void DestroyAt(vec2 const & screenCoordinates);
	void DrawTo(vec2 const & screenCoordinates);

	bool IsRunning() const;
	void SetRunningState(bool isRunning);
	void SetCanvasSize(int width, int height);
	void Pan(vec2 const & screenOffset);
	void AdjustZoom(float amount);

	float GetStrength() const { return mGameParameters.Strength; }
	void SetStrength(float strength);	
	float GetMinStrength() const { return GameParameters::MinStrength;  }
	float GetMaxStrength() const { return GameParameters::MaxStrength; }

	float GetBuoyancy() const { return mGameParameters.Buoyancy; }
	void SetBuoyancy(float value);
	float GetMinBuoyancy() const { return GameParameters::MinBuoyancy; }
	float GetMaxBuoyancy() const { return GameParameters::MaxBuoyancy; }

	float GetWaterPressure() const { return mGameParameters.WaterPressure; }
	void SetWaterPressure(float value);
	float GetMinWaterPressure() const { return GameParameters::MinWaterPressure; }
	float GetMaxWaterPressure() const { return GameParameters::MaxWaterPressure; }

	float GetWaveHeight() const { return mGameParameters.WaveHeight; }
	void SetWaveHeight(float value);
	float GetMinWaveHeight() const { return GameParameters::MinWaveHeight; }
	float GetMaxWaveHeight() const { return GameParameters::MaxWaveHeight; }

	float GetSeaDepth() const { return mGameParameters.SeaDepth; }
	void SetSeaDepth(float value);
	float GetMinSeaDepth() const { return GameParameters::MinSeaDepth; }
	float GetMaxSeaDepth() const { return GameParameters::MaxSeaDepth; }

	bool GetDoQuickWaterFix() const { return mRenderParameters.QuickWaterFix;  }
	void SetDoQuickWaterFix(bool value);

	bool GetDoShowStress() const { return mRenderParameters.ShowStress; }
	void SetDoShowStress(bool value);

	bool GetDoUseXRayMode() const { return mRenderParameters.UseXRayMode; }
	void SetDoUseXRayMode(bool value);

private:

	GameController(
		std::unique_ptr<Game> && game,
		std::wstring const & initialShipLoaded)
		: mGameParameters()
		, mRenderParameters()
		, mIsRunning(true)
		, mLastShipLoaded(initialShipLoaded)
		, mGame(std::move(game))
	{}
	
	static vec2 Screen2World(
		vec2 const & screenCoordinates,
		RenderParameters const & renderParameters);

	static vec2 ScreenOffset2WorldOffset(
		vec2 const & screenOffset,
		RenderParameters const & renderParameters);

private:

	GameParameters mGameParameters;
	RenderParameters mRenderParameters;

	bool mIsRunning;

	std::wstring mLastShipLoaded;

	std::unique_ptr<Game> mGame;
};
