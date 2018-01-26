/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameController.h"

#include "Log.h"

std::unique_ptr<GameController> GameController::Create()
{
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
	if (mIsRunning)
	{
		// Update game now, copying the parameters
		assert(!!mGame);
		mGame->Update(
			0.02, // TODO: setting? if not, constant
			mGameParameters);
	}
}

void GameController::Render()
{
	// Render game, copying the parameters
	assert(!!mGame);
	mGame->Render(mRenderParameters);
}

/////////////////////////////////////////////////////////////
// Interactions
/////////////////////////////////////////////////////////////

void GameController::DestroyAt(vec2 const & screenCoordinates)
{
	vec2 worldCoordinates = Screen2World(
		screenCoordinates,
		mRenderParameters);

	// Apply action
	assert(!!mGame);
	mGame->DestroyAt(
		worldCoordinates,
		mGameParameters.DestroyRadius);
}

void GameController::DrawTo(vec2 const & screenCoordinates)
{
	vec2 worldCoordinates = Screen2World(
		screenCoordinates,
		mRenderParameters);

	// Apply action
	assert(!!mGame);
	mGame->DrawTo(worldCoordinates);
}

bool GameController::IsRunning() const
{
	return mIsRunning;
}

void GameController::SetRunningState(bool isRunning)
{
	mIsRunning = isRunning;
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
	vec2 worldOffset = ScreenOffset2WorldOffset(
		screenOffset,
		mRenderParameters);

	mRenderParameters.CamX -= worldOffset.x;
	mRenderParameters.CamY -= worldOffset.y;
}

void GameController::AdjustZoom(float amount)
{
	mRenderParameters.Zoom *= amount;
}

void GameController::SetStrength(float value)
{
	LogDebug("GameController::SetStrength(", value, ")");

	mGameParameters.Strength = value;
}

void GameController::SetBuoyancy(float value)
{
	LogDebug("GameController::SetBuoyancy(", value, ")");

	mGameParameters.Buoyancy = value;
}

void GameController::SetWaterPressure(float value)
{
	LogDebug("GameController::SetWaterPressure(", value, ")");

	mGameParameters.WaterPressure = value;
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

vec2 GameController::Screen2World(
	vec2 const & screenCoordinates,
	RenderParameters const & renderParameters)
{
	float height = renderParameters.Zoom * 2.0f;
	float width = static_cast<float>(renderParameters.CanvasWidth) / static_cast<float>(renderParameters.CanvasHeight) * height;
	return vec2((screenCoordinates.x / static_cast<float>(renderParameters.CanvasWidth) - 0.5f) * width + renderParameters.CamX,
		(screenCoordinates.y / static_cast<float>(renderParameters.CanvasHeight) - 0.5f) * -height + renderParameters.CamY);
}

vec2 GameController::ScreenOffset2WorldOffset(
	vec2 const & screenOffset,
	RenderParameters const & renderParameters)
{
	float height = renderParameters.Zoom * 2.0f;
	float width = static_cast<float>(renderParameters.CanvasWidth) / static_cast<float>(renderParameters.CanvasHeight) * height;

	return vec2(
		screenOffset.x / static_cast<float>(renderParameters.CanvasWidth) * width,
		screenOffset.y / static_cast<float>(renderParameters.CanvasHeight) * -height);
}
