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

	void DestroyAt(vec2 worldCoordinates, float radius);
	void DrawTo(vec2 worldCoordinates);

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
		std::vector<std::unique_ptr<Material const>> materials,
		std::vector<float> oceanDepth,
		std::unique_ptr<phys::world> world)
		: mMaterials(std::move(materials))
		, mOceanDepth(std::move(oceanDepth))
		, mWorld(std::move(world))
	{
	}

	static std::vector<std::unique_ptr<Material const>> LoadMaterials(std::wstring const & filepath);
	static std::vector<float> LoadOceanDepth(std::wstring const & filepath);

private:

	std::vector<std::unique_ptr<Material const>> const mMaterials;
	std::vector<float> const mOceanDepth;

    std::unique_ptr<phys::world> mWorld;
};
