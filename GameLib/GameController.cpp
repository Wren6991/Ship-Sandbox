/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameController.h"

#include "Log.h"

#include <IL/il.h>
#include <IL/ilu.h>

// The dt of each step
static constexpr float StepTimeDuration = 0.02f;

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
	std::string initialShipFilename = "Data/default_ship.png";
	game->LoadShip(initialShipFilename);

	return std::unique_ptr<GameController>(
		new GameController(
			std::move(game),
			initialShipFilename));
}

void GameController::ResetAndLoadShip(std::string const & filepath)
{
	assert(!!mGame);

	mGame->Reset();
	mGame->LoadShip(filepath);

	mLastShipLoaded = filepath;
}

void GameController::AddShip(std::string const & filepath)
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
	// Update game
	assert(!!mGame);
	mGame->Update(
        StepTimeDuration,
		mGameParameters);
}

void GameController::Render()
{
	// Render game
	assert(!!mGame);
	mGame->Render(mGameParameters, *mRenderContext);
}

/////////////////////////////////////////////////////////////
// Interactions
/////////////////////////////////////////////////////////////

void GameController::DestroyAt(
    vec2f const & screenCoordinates, 
    float radiusMultiplier)
{
	vec2f worldCoordinates = mRenderContext->Screen2World(screenCoordinates);

    LogMessage("DestroyAt: ", worldCoordinates.toString(), " * ", radiusMultiplier);

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
	vec2f worldCoordinates = mRenderContext->Screen2World(screenCoordinates);

	// Apply action
	assert(!!mGame);
	mGame->DrawTo(worldCoordinates, 50000.0f * strengthMultiplier);
}

Physics::Point const * GameController::GetNearestPointAt(vec2 const & screenCoordinates) const
{
    vec2f worldCoordinates = mRenderContext->Screen2World(screenCoordinates);

    assert(!!mGame);
    return mGame->GetNearestPointAt(worldCoordinates);
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
