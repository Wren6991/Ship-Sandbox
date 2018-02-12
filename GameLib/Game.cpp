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
#include <map>
#include <string>

std::unique_ptr<Game> Game::Create()
{
	auto materials = LoadMaterials("Data/materials.json");
	auto oceanDepth = LoadOceanDepth("Data/depth.png");

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

void Game::LoadShip(std::string const & filepath)
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
		std::string devilErrorMessage(iluErrorString(devilError));
		throw GameException("Could not load ship \"" + filepath + "\": " + devilErrorMessage);
	}

    //
    // Check if we need to convert it
    //

    int imageFormat = ilGetInteger(IL_IMAGE_FORMAT);
    int imageType = ilGetInteger(IL_IMAGE_TYPE);
    if (IL_RGB != imageFormat || IL_UNSIGNED_BYTE != imageType)
    {
        if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
        {
            ILint devilError = ilGetError();
            std::string devilErrorMessage(iluErrorString(devilError));
            throw GameException("Could not convert ship image \"" + filepath + "\": " + devilErrorMessage);
        }
    }
	

	//
	// Create ship and add to world
	//

    ILubyte const * imageData = ilGetData();
    int const width = ilGetInteger(IL_IMAGE_WIDTH);
    int const height = ilGetInteger(IL_IMAGE_HEIGHT);

	std::unique_ptr<Physics::Ship> shp = Physics::Ship::Create(
		mWorld.get(),
		imageData,
		width,
		height,
		mMaterials);

    LogMessage("Loaded ship from ", filepath, " : W=", width, ", H=", height, ", ",
        shp->GetPoints().size(), " points, ", shp->GetSprings().size(),
        " springs, ", shp->GetTriangles().size(), " triangles, ", shp->GetElectricalElements().size(), " electrical elements.");

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

void Game::DrawTo(
    vec2 worldCoordinates,
    float strength)
{
	assert(!!mWorld);
	mWorld->DrawTo(worldCoordinates, strength);

	// TODO: publish game event
}

Physics::Point const * Game::GetNearestPointAt(vec2 const & worldCoordinates) const
{
    assert(!!mWorld);
    return mWorld->GetNearestPointAt(worldCoordinates);
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

	// Set projection (ortho) and translation 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 1, 1000);
	glTranslatef(-renderParameters.CamX, -renderParameters.CamY, 0);

    // Smooth lines and points
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);

    // Set blending function
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set primitives' thickness
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

std::vector<std::unique_ptr<Material const>> Game::LoadMaterials(std::string const & filepath)
{
	std::vector<std::unique_ptr<Material const>> materials;

	picojson::value root = Utils::ParseJSONFile(filepath);
	if (!root.is<picojson::array>())
	{
		throw GameException("File \"" + filepath + "\" does not contain a JSON array");
	}
	
	picojson::array rootArray = root.get<picojson::array>();
	for (auto const & rootElem : rootArray)
	{
		if (!rootElem.is<picojson::object>())
		{
			throw GameException("File \"" + filepath + "\" does not contain a JSON array of objects");
		}

		materials.emplace_back(Material::Create(rootElem.get<picojson::object>()));
	}

	return materials;
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

