/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameParameters.h"
#include "phys.h"
#include "RenderParameters.h"

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
	 * Parameters are copied on purpose so that the user may keep interacting with parameters
	 * while the game is being updated in a separate thread.
	 */
	void Update(
		double dt,
		GameParameters gameParameters);
	
	
	/*
	 * Renders the game.
	 *
	 * Parameters are copied on purpose so that the user may keep interacting with parameters
	 * while the game is being rendered in a separate thread.
	 */
	void Render(RenderParameters renderParameters);

private:

	Game(
		std::vector<std::shared_ptr<material>> materials,
		std::unique_ptr<phys::world> world)
		: mMaterials(std::move(materials))
		, mWorld(std::move(world))
	{
	}

	void LoadDepth(std::wstring const & filepath);

	// TODOç make it a wstring once we have jsoncpp in unicode
	static std::vector<std::shared_ptr<material>> LoadMaterials(std::string const & filepath);

private:

	std::vector<std::shared_ptr<material>> const mMaterials;

	// TODO
    float *mOceanDepthBuffer;

    std::unique_ptr<phys::world> mWorld;
};
