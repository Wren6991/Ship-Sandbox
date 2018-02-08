/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameController.h"

#include "Log.h"
#include "RenderUtils.h"

#include <IL/il.h>
#include <IL/ilu.h>

std::unique_ptr<GameController> GameController::Create()
{
	// Initialize DevIL
	ilInit();
	iluInit();

	// Create game
	std::unique_ptr<Game> game = Game::Create();

	//
	// Initialize game
	//

	// Load initial ship
	std::wstring initialShipFilename = L"Data/default_ship.png";
	game->LoadShip(initialShipFilename);

	return std::unique_ptr<GameController>(
		new GameController(
			std::move(game),
			initialShipFilename));
}

void GameController::ResetAndLoadShip(std::wstring const & filepath)
{
	assert(!!mGame);

	mGame->Reset();
	mGame->LoadShip(filepath);

	mLastShipLoaded = filepath;
}

void GameController::AddShip(std::wstring const & filepath)
{
	assert(!!mGame);

	mGame->LoadShip(filepath);

	mLastShipLoaded = filepath;
}

void GameController::ReloadLastShip()
{
	if (mLastShipLoaded.empty())
	{
		throw std::runtime_error("No ship has been loaded yet");
	}

	assert(!!mGame);

	mGame->Reset();
	mGame->LoadShip(mLastShipLoaded);
}

void GameController::DoStep()
{
	// Update game, copying the parameters
	assert(!!mGame);
	mGame->Update(
		0.02f, // TODO: setting? if not, constant
		mGameParameters);
}

void GameController::Render()
{
	// Render game, copying the parameters
	assert(!!mGame);
	mGame->Render(mGameParameters, mRenderParameters);
}

/////////////////////////////////////////////////////////////
// Interactions
/////////////////////////////////////////////////////////////

void GameController::DestroyAt(
    vec2 const & screenCoordinates, 
    float radiusMultiplier)
{
	vec2 worldCoordinates = RenderUtils::Screen2World(
		screenCoordinates,
		mRenderParameters);

    LogMessage(L"DestroyAt: ", worldCoordinates.toString(), " * ", radiusMultiplier);

	// Apply action
	assert(!!mGame);
	mGame->DestroyAt(
		worldCoordinates,
		mGameParameters.DestroyRadius * radiusMultiplier);
}

void GameController::DrawTo(
    vec2 const & screenCoordinates,
    float strengthMultiplier)
{
	vec2 worldCoordinates = RenderUtils::Screen2World(
		screenCoordinates,
		mRenderParameters);

	// Apply action
	assert(!!mGame);
	mGame->DrawTo(worldCoordinates, 50000.0f * strengthMultiplier);
}

void GameController::SetCanvasSize(
	int width,
	int height)
{
	mRenderParameters.CanvasWidth = width;
	mRenderParameters.CanvasHeight = height;
}

void GameController::Pan(vec2 const & screenOffset)
{
	vec2 worldOffset = RenderUtils::ScreenOffset2WorldOffset(
		screenOffset,
		mRenderParameters);

	mRenderParameters.CamX -= worldOffset.x;
	mRenderParameters.CamY -= worldOffset.y;
}

void GameController::AdjustZoom(float amount)
{
	mRenderParameters.Zoom *= amount;
}

void GameController::SetStrengthAdjustment(float value)
{
	LogDebug("GameController::SetStrengthAdjustment(", value, ")");

	mGameParameters.StrengthAdjustment = value;
}

void GameController::SetBuoyancyAdjustment(float value)
{
	LogDebug("GameController::SetBuoyancyAdjustment(", value, ")");

	mGameParameters.BuoyancyAdjustment = value;
}

void GameController::SetWaterPressureAdjustment(float value)
{
	LogDebug("GameController::SetWaterPressureAdjustment(", value, ")");

	mGameParameters.WaterPressureAdjustment = value;
}

void GameController::SetWaveHeight(float value)
{
	LogDebug("GameController::SetWaveHeight(", value, ")");

	mGameParameters.WaveHeight = value;
}

void GameController::SetSeaDepth(float value)
{
	LogDebug("GameController::SetSeaDepth(", value, ")");

	mGameParameters.SeaDepth = value;
}

void GameController::SetDestroyRadius(float value)
{
	LogDebug("GameController::SetDestroyRadius(", value, ")");

	mGameParameters.DestroyRadius = value;
}

void GameController::SetDoQuickWaterFix(bool value)
{
	LogDebug("GameController::SetDoQuickWaterFix(", value, ")");

	mRenderParameters.QuickWaterFix = value;
}

void GameController::SetDoShowStress(bool value)
{
	LogDebug("GameController::SetDoShowStress(", value, ")");

	mRenderParameters.ShowStress = value;
}

void GameController::SetDoUseXRayMode(bool value)
{
	LogDebug("GameController::SetDoUseXRayMode(", value, ")");

	mRenderParameters.UseXRayMode = value;
}
