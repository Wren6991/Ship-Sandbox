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

	// Initialize game
	game->LoadShip(L"ship.png");

	return std::unique_ptr<GameController>(
		new GameController(std::move(game)));
}

void GameController::Reset(std::wstring const & filepath)
{
	assert(!!mGame);

	mGame->Reset();
	mGame->LoadShip(filepath);
}

void GameController::AddShip(std::wstring const & filepath)
{
	assert(!!mGame);

	mGame->LoadShip(filepath);
}

void GameController::Update()
{
	assert(!!mGame);

	// Apply queued actions
	// TODO

	if (mIsRunning)
	{
		// Update game now, copying the settings
		mGame->Update(
			0.02, // TODO: setting? if not, constant
			mGameSettings);
	}
}

void GameController::Render()
{
	assert(!!mGame);

	// Render game, copying the settings
	mGame->Render(mRenderSettings);
}

void GameController::DestroyAt(vec2 const & screenCoordinates)
{
	vec2 worldCoordinates = Screen2World(
		screenCoordinates,
		mRenderSettings);

	// Queue action, we'll apply it at the next Update()
	// TODO
}

void GameController::DrawAt(vec2 const & screenCoordinates)
{
	vec2 worldCoordinates = Screen2World(
		screenCoordinates,
		mRenderSettings);

	// Queue action, we'll apply it at the next Update()
	// TODO
}

void GameController::Pan(vec2 const & screenOffset)
{
	vec2 worldOffset = ScreenOffset2WorldOffset(
		screenOffset,
		mRenderSettings);

	mRenderSettings.CamX -= worldOffset.x;
	mRenderSettings.CamY -= worldOffset.y;
}

vec2 GameController::Screen2World(
	vec2 const & screenCoordinates,
	RenderSettings const & renderSettings)
{
	float height = renderSettings.Zoom * 2.0f;
	float width = static_cast<float>(renderSettings.CanvasWidth) / static_cast<float>(renderSettings.CanvasHeight) * height;
	return vec2((screenCoordinates.x / static_cast<float>(renderSettings.CanvasWidth) - 0.5f) * width + renderSettings.CamX,
		(screenCoordinates.y / static_cast<float>(renderSettings.CanvasHeight) - 0.5f) * -height + renderSettings.CamY);
}

vec2 GameController::ScreenOffset2WorldOffset(
	vec2 const & screenOffset,
	RenderSettings const & renderSettings)
{
	float height = renderSettings.Zoom * 2.0f;
	float width = static_cast<float>(renderSettings.CanvasWidth) / static_cast<float>(renderSettings.CanvasHeight) * height;
	return vec2((screenOffset.x / static_cast<float>(renderSettings.CanvasWidth) - 0.5f) * width,
		(screenOffset.y / static_cast<float>(renderSettings.CanvasHeight) - 0.5f) * -height);
}
