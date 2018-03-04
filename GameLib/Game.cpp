/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Game.h"

#include "GameException.h"
#include "Log.h"

#include <cassert>
#include <map>
#include <string>

std::unique_ptr<Game> Game::Create(std::shared_ptr<ResourceLoader> resourceLoader)
{
	auto materials = resourceLoader->LoadMaterials();
	auto oceanDepth = LoadOceanDepth("Data/depth.png");

	std::unique_ptr<Physics::World> world = std::make_unique<Physics::World>();

	return std::unique_ptr<Game>(
		new Game(
			std::move(materials),
			std::move(oceanDepth),
			std::move(world),
            resourceLoader));
}

void Game::Reset()
{
	mWorld.reset(new Physics::World());
}

void Game::LoadShip(std::string const & filepath)
{
    auto structureImage = mResourceLoader->LoadStructureImage(filepath);

	//
	// Create ship and add to world
	//

	std::unique_ptr<Physics::Ship> shp = Physics::Ship::Create(
		mWorld.get(),
        structureImage->Data.get(),
        structureImage->Width,
        structureImage->Height,
		mMaterials);

    LogMessage("Loaded ship from ", filepath, " : W=", structureImage->Width, ", H=", structureImage->Height, ", ",
        shp->GetPoints().size(), " points, ", shp->GetSprings().size(),
        " springs, ", shp->GetTriangles().size(), " triangles, ", shp->GetElectricalElements().size(), " electrical elements.");

	mWorld->AddShip(std::move(shp));
}

void Game::DestroyAt(
	vec2f worldCoordinates,
	float radius)
{
	assert(!!mWorld);
	mWorld->DestroyAt(worldCoordinates, radius);

	// TODO: publish game event
}

void Game::DrawTo(
    vec2f worldCoordinates,
    float strength)
{
	assert(!!mWorld);
	mWorld->DrawTo(worldCoordinates, strength);

	// TODO: publish game event
}

Physics::Point const * Game::GetNearestPointAt(
    vec2 const & worldCoordinates,
    float radius) const
{
    assert(!!mWorld);
    return mWorld->GetNearestPointAt(worldCoordinates,  radius);
}

void Game::Update(
	float dt,
	GameParameters const & gameParameters)
{
	assert(!!mWorld);

	mWorld->Update(
		dt,
		gameParameters);
}

void Game::Render(
	GameParameters const & gameParameters,
	RenderContext & renderContext) const
{
	assert(!!mWorld);

	mWorld->Render(
		gameParameters,
		renderContext);

    glFlush();
}

std::vector<float> Game::LoadOceanDepth(std::string const & filepath)
{
    // TBD
    (void)filepath;
	/*wxImage depthimage(filename, wxBITMAP_TYPE_PNG);
	oceandepthbuffer = new float[2048];
	for (unsigned i = 0; i < 2048; i++)
	{
	float xpos = i / 16.f;
	oceandepthbuffer[i] = depthimage.GetRed(floorf(xpos), 0) * (floorf(xpos) - xpos) + depthimage.GetRed(ceilf(xpos), 0) * (1 - (floorf(xpos) - xpos))
	;//+ depthimage.GetGreen(i % 256, 0) * 0.0625f;
	oceandepthbuffer[i] = oceandepthbuffer[i] * 1.f - 180.f;
	}*/
	return std::vector<float>();
}

