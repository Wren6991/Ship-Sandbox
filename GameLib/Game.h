/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameParameters.h"
#include "IGameEventHandler.h"
#include "Physics.h"
#include "RenderContext.h"
#include "ResourceLoader.h"

#include <memory>
#include <vector>

class Game
{
public:

	static std::unique_ptr<Game> Create(
        std::shared_ptr<IGameEventHandler> gameEventHandler,
        std::shared_ptr<ResourceLoader> resourceLoader);

	void Reset();

	void LoadShip(std::string const & filepath);

	void DestroyAt(
        vec2 worldCoordinates, 
        float radius);

	void DrawTo(
        vec2 worldCoordinates,         
        float strength);

    Physics::Point const * GetNearestPointAt(
        vec2 const & worldCoordinates,
        float radius) const;

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
		RenderContext & renderContext) const;

private:

	Game(
		std::vector<std::unique_ptr<Material const>> materials,
		std::vector<float> oceanDepth,
		std::unique_ptr<Physics::World> world,
        std::shared_ptr<IGameEventHandler> gameEventHandler,
        std::shared_ptr<ResourceLoader> resourceLoader)
		: mMaterials(std::move(materials))
		, mOceanDepth(std::move(oceanDepth))
		, mWorld(std::move(world))
        , mGameEventHandler(std::move(gameEventHandler))
        , mResourceLoader(std::move(resourceLoader))
	{
	}

	static std::vector<float> LoadOceanDepth(std::string const & filepath);

private:

	std::vector<std::unique_ptr<Material const>> const mMaterials;
	std::vector<float> const mOceanDepth;

    std::unique_ptr<Physics::World> mWorld;

    std::shared_ptr<IGameEventHandler> mGameEventHandler;
    std::shared_ptr<ResourceLoader> mResourceLoader;
};
