/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameSettings.h"
#include "phys.h"
#include "RenderSettings.h"

#include <memory>
#include <vector>

class Game
{
public:

	static std::unique_ptr<Game> Create();

	void Reset();
	void LoadShip(std::wstring const & filepath);

	/*
	 * Runs a game simulation step.
	 *
	 * Settings are copied on purpose so that the user may keep interacting with settings
	 * while the game is being updated in a separate thread.
	 */
	void Update(
		double dt,
		GameSettings gameSettings);
	
	
	/*
	 * Renders the game.
	 *
	 * Settings are copied on purpose so that the user may keep interacting with settings
	 * while the game is being rendered in a separate thread.
	 */
	void Render(RenderSettings renderSettings);

private:

	Game(
		std::vector<std::shared_ptr<material>> materials,
		std::unique_ptr<phys::world> world)
		: mMaterials(std::move(materials))
		, mWorld(std::move(world))
	{
	}

	void LoadDepth(std::wstring const & filepath);

	static std::vector<std::shared_ptr<material>> LoadMaterials(std::wstring const & filepath);

private:

	std::vector<std::shared_ptr<material>> const mMaterials;

	// TODO
    float *mOceanDepthBuffer;

    std::unique_ptr<phys::world> mWorld;
};
