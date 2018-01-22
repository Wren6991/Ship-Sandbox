/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Game2.h"
#include "GameSettings.h"
#include "RenderSettings.h"
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

	void Reset(std::wstring const & filepath);
    void AddShip(std::wstring const & filepath);

	void DoStep();

	//
	// Interactions
	//

	void SetRunningState(bool isRunning);
	void DestroyAt(vec2 const & screenCoordinates);
	void DrawAt(vec2 const & screenCoordinates);
	void SetCanvasSize(int width, int height);
	void Pan(vec2 const & screenOffset);
	void AdjustZoom(float amount);

private:

	GameController(
		std::unique_ptr<Game> && game)
		: mGameSettings()
		, mRenderSettings()
		, mIsRunning(true)
		, mGame(std::move(game))
	{}
	
	static vec2 Screen2World(
		vec2 const & screenCoordinates,
		RenderSettings const & renderSettings);

	static vec2 ScreenOffset2WorldOffset(
		vec2 const & screenOffset,
		RenderSettings const & renderSettings);

private:

	GameSettings mGameSettings;
	RenderSettings mRenderSettings;

	bool mIsRunning;

	std::unique_ptr<Game> mGame;
};
