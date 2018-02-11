/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Game.h"
#include "GameParameters.h"
#include "RenderParameters.h"
#include "Vectors.h"

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

	void ResetAndLoadShip(std::string const & filepath);
    void AddShip(std::string const & filepath);
	void ReloadLastShip();

	void DoStep();
	void Render();


	//
	// Interactions
	//

	void DestroyAt(vec2 const & screenCoordinates, float radiusMultiplier);
	void DrawTo(vec2 const & screenCoordinates, float strengthMultiplier);

	void SetCanvasSize(int width, int height);

	void Pan(vec2 const & screenOffset);
    void ResetPan();

	void AdjustZoom(float amount);
    void ResetZoom();

	float GetStrengthAdjustment() const { return mGameParameters.StrengthAdjustment; }
	void SetStrengthAdjustment(float value);
	float GetMinStrengthAdjustment() const { return GameParameters::MinStrengthAdjustment;  }
	float GetMaxStrengthAdjustment() const { return GameParameters::MaxStrengthAdjustment; }

	float GetBuoyancyAdjustment() const { return mGameParameters.BuoyancyAdjustment; }
	void SetBuoyancyAdjustment(float value);
	float GetMinBuoyancyAdjustment() const { return GameParameters::MinBuoyancyAdjustment; }
	float GetMaxBuoyancyAdjustment() const { return GameParameters::MaxBuoyancyAdjustment; }

	float GetWaterPressureAdjustment() const { return mGameParameters.WaterPressureAdjustment; }
	void SetWaterPressureAdjustment(float value);
	float GetMinWaterPressureAdjustment() const { return GameParameters::MinWaterPressureAdjustment; }
	float GetMaxWaterPressureAdjustment() const { return GameParameters::MaxWaterPressureAdjustment; }

	float GetWaveHeight() const { return mGameParameters.WaveHeight; }
	void SetWaveHeight(float value);
	float GetMinWaveHeight() const { return GameParameters::MinWaveHeight; }
	float GetMaxWaveHeight() const { return GameParameters::MaxWaveHeight; }

	float GetSeaDepth() const { return mGameParameters.SeaDepth; }
	void SetSeaDepth(float value);
	float GetMinSeaDepth() const { return GameParameters::MinSeaDepth; }
	float GetMaxSeaDepth() const { return GameParameters::MaxSeaDepth; }

	float GetDestroyRadius() const { return mGameParameters.DestroyRadius; }
	void SetDestroyRadius(float value);
	float GetMinDestroyRadius() const { return GameParameters::MinDestroyRadius; }
	float GetMaxDestroyRadius() const { return GameParameters::MaxDestroyRadius; }

	bool GetDoQuickWaterFix() const { return mRenderParameters.QuickWaterFix;  }
	void SetDoQuickWaterFix(bool value);

	bool GetDoShowStress() const { return mRenderParameters.ShowStress; }
	void SetDoShowStress(bool value);

	bool GetDoUseXRayMode() const { return mRenderParameters.UseXRayMode; }
	void SetDoUseXRayMode(bool value);

private:

	GameController(
		std::unique_ptr<Game> && game,
		std::string const & initialShipLoaded)
		: mGameParameters()
		, mRenderParameters()
		, mLastShipLoaded(initialShipLoaded)
		, mGame(std::move(game))
	{}
	
private:

	GameParameters mGameParameters;
	RenderParameters mRenderParameters;

	std::string mLastShipLoaded;

	std::unique_ptr<Game> mGame;
};
