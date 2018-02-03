/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Game.h"

#include "GameException.h"
#include "GameOpenGL.h"
#include "Log.h"
#include "Utils.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <picojson/picojson.h>

#include <cassert>
#include <iostream>// TODO: nuke
#include <map>
#include <string>

std::unique_ptr<Game> Game::Create()
{
	auto materials = LoadMaterials(L"Data/materials.json");
	auto oceanDepth = LoadOceanDepth(L"Data/depth.png");

	std::unique_ptr<Physics::World> world = std::make_unique<Physics::World>();

	return std::unique_ptr<Game>(
		new Game(
			std::move(materials),
			std::move(oceanDepth),
			std::move(world)));
}

void Game::Reset()
{
	mWorld.reset(new Physics::World());
}

void Game::LoadShip(std::wstring const & filepath)
{
	//
	// Load image
	//

	ILuint imghandle;
	ilGenImages(1, &imghandle);
	ilBindImage(imghandle);

	ILconst_string ilFilename(filepath.c_str());
	if (!ilLoadImage(ilFilename))
	{
		ILint devilError = ilGetError();
		std::wstring devilErrorMessage(iluErrorString(devilError));
		throw GameException(L"Could not load ship \"" + filepath + L"\": " + devilErrorMessage);
	}
	
	ILubyte const * imageData = ilGetData();
	int const width = ilGetInteger(IL_IMAGE_WIDTH);
	int const height = ilGetInteger(IL_IMAGE_HEIGHT);


	//
	// Create ship and add to world
	//

	std::unique_ptr<Physics::Ship> shp = Physics::Ship::Create(
		mWorld.get(),
		imageData,
		width,
		height,
		mMaterials);

	mWorld->AddShip(std::move(shp));

	//
	// Delete image
	//

	ilDeleteImage(imghandle);
}

void Game::DestroyAt(
	vec2 worldCoordinates,
	float radius)
{
	assert(!!mWorld);
	mWorld->DestroyAt(worldCoordinates, radius);

	// TODO: publish game event
}

void Game::DrawTo(vec2 worldCoordinates)
{
	assert(!!mWorld);
	mWorld->DrawTo(worldCoordinates);

	// TODO: publish game event
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
	RenderParameters const & renderParameters) const
{
	assert(!!mWorld);

	float halfHeight = renderParameters.Zoom;
	float halfWidth = static_cast<float>(renderParameters.CanvasWidth) / static_cast<float>(renderParameters.CanvasHeight) * halfHeight;

	// Clear canvas (blue)
	glViewport(0, 0, renderParameters.CanvasWidth, renderParameters.CanvasHeight);
	glClearColor(0.529f, 0.808f, 0.980f, 1.0f); //(cornflower blue)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: see if all of this may be cached (with ParametersGenerationSequenceNumber check)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 1, 1000);
	glTranslatef(-renderParameters.CamX, -renderParameters.CamY, 0);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(0.15f * renderParameters.CanvasHeight / renderParameters.Zoom);
	glLineWidth(0.1f * renderParameters.CanvasHeight / renderParameters.Zoom);
	glColor3f(0, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mWorld->Render(
		renderParameters.CamX - halfWidth,
		renderParameters.CamX + halfWidth,
		renderParameters.CamY - halfHeight,
		renderParameters.CamY + halfHeight,
		gameParameters,
		renderParameters);

	glFlush();
}

std::vector<std::unique_ptr<Material const>> Game::LoadMaterials(std::wstring const & filepath)
{
	std::vector<std::unique_ptr<Material const>> materials;

	picojson::value root = Utils::ParseJSONFile(filepath);
	if (!root.is<picojson::array>())
	{
		throw GameException(L"File \"" + filepath + L"\" does not contain a JSON array");
	}
	
	picojson::array rootArray = root.get<picojson::array>();
	for (auto const & rootElem : rootArray)
	{
		if (!rootElem.is<picojson::object>())
		{
			throw GameException(L"File \"" + filepath + L"\" does not contain a JSON array of objects");
		}

		materials.emplace_back(Material::Create(rootElem.get<picojson::object>()));
	}

	return materials;
}

std::vector<float> Game::LoadOceanDepth(std::wstring const & filepath)
{
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

