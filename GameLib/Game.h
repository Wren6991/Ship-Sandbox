/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameParameters.h"
#include "Physics.h"
#include "RenderParameters.h"

#include <memory>
#include <vector>

class Game
{
public:

	static std::unique_ptr<Game> Create();

	void Reset();
	void LoadShip(std::string const & filepath);

	void DestroyAt(
        vec2 worldCoordinates, 
        float radius);

	void DrawTo(
        vec2 worldCoordinates,         
        float strength);

	/*
	 * Runs a game simulation step.
	 */
	void Update(
		float dt,
		GameParameters const & gameParameters);
	
	
	/*
	 * Renders the game.
	 */
	void Render(
		GameParameters const & gameParameters,
		RenderParameters const & renderParameters) const;

private:

	Game(
		std::vector<std::unique_ptr<Material const>> materials,
		std::vector<float> oceanDepth,
		std::unique_ptr<Physics::World> world)
		: mMaterials(std::move(materials))
		, mOceanDepth(std::move(oceanDepth))
		, mWorld(std::move(world))
	{
	}

	static std::vector<std::unique_ptr<Material const>> LoadMaterials(std::string const & filepath);
	static std::vector<float> LoadOceanDepth(std::string const & filepath);

private:

	std::vector<std::unique_ptr<Material const>> const mMaterials;
	std::vector<float> const mOceanDepth;

    std::unique_ptr<Physics::World> mWorld;
};
