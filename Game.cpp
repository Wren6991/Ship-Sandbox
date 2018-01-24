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
#include "util.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <json/json.h>

#include <cassert>
#include <iostream>
#include <map>
#include <string>

const int directions[8][2] = {
{ 1,  0},
{ 1, -1},
{ 0, -1},
{-1, -1},
{-1,  0},
{-1,  1},
{ 0,  1},
{ 1,  1}
};

std::unique_ptr<Game> Game::Create()
{
	auto materials = LoadMaterials(L"data/materials.json");
	auto oceanDepth = LoadOceanDepth(L"data/depth.png");

	std::unique_ptr<phys::world> world = std::make_unique<phys::world>();

	return std::unique_ptr<Game>(
		new Game(
			std::move(materials),
			std::move(oceanDepth),
			std::move(world)));
}

void Game::Reset()
{
	mWorld.reset(new phys::world());
}

void Game::LoadShip(std::wstring const & filepath)
{
	int nodecount = 0, springcount = 0;

	std::map<vec3f, std::shared_ptr<material>> colourdict;
	for (unsigned int i = 0; i < mMaterials.size(); i++)
		colourdict[mMaterials[i]->colour] = mMaterials[i];

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

	ILubyte *data = ilGetData();

	int const width = ilGetInteger(IL_IMAGE_WIDTH);
	int const height = ilGetInteger(IL_IMAGE_HEIGHT);
	float const halfWidth = static_cast<float>(width) / 2.0f;

	phys::ship *shp = new phys::ship(mWorld.get());

	std::map<int, std::map <int, phys::point*> > points;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			// assume R G B:
			vec3f colour(
				data[(x + (height - y - 1) * width) * 3 + 0] / 255.f,
				data[(x + (height - y - 1) * width) * 3 + 1] / 255.f,
				data[(x + (height - y - 1) * width) * 3 + 2] / 255.f);

			if (colourdict.find(colour) != colourdict.end())
			{
				auto mtl = colourdict[colour];

				points[x][y] = new phys::point(
					mWorld.get(), 
					vec2(static_cast<float>(x) - halfWidth, static_cast<float>(y)),
					mtl.get(), 
					mtl->isHull ? 0 : 1);  // no buoyancy if it's a hull section

				shp->points.insert(points[x][y]);
				nodecount++;
			}
			else
			{
				points[x][y] = 0;
			}
		}
	}

	// Points have been generated, so fill in all the beams between them.
	// If beam joins two hull nodes, it is a hull beam.
	// If a non-hull node has empty space on one of its four sides, it is automatically leaking.

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			phys::point *a = points[x][y];
			if (!a)
				continue;
			// First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y) - this covers each pair of points in each direction
			for (int i = 0; i < 4; i++)
			{
				phys::point *b = points[x + directions[i][0]][y + directions[i][1]];                        // adjacent point in direction (i)
				phys::point *c = points[x + directions[(i + 1) % 8][0]][y + directions[(i + 1) % 8][1]];    // adjacent point in next CW direction (for constructing triangles)
				if (b)
				{
					bool pointIsHull = a->mtl->isHull;
					bool isHull = pointIsHull && b->mtl->isHull;
					material *mtl = b->mtl->isHull ? a->mtl : b->mtl;    // the spring is hull iff both nodes are hull; if so we use the hull material.
					shp->springs.insert(new phys::spring(mWorld.get(), a, b, mtl, -1));
					if (!isHull)
					{
						shp->adjacentnodes[a].insert(b);
						shp->adjacentnodes[b].insert(a);
					}
					if (!(pointIsHull || (points[x + 1][y] && points[x][y + 1] && points[x - 1][y] && points[x][y - 1])))   // check for gaps next to non-hull areas:
					{
						a->isLeaking = true;
					}
					if (c)
						shp->triangles.insert(new phys::ship::triangle(shp, a, b, c));
					springcount++;
				}
			}
		}
	}

	ilDeleteImage(imghandle);

	LogMessage(L"Loaded ship \"", filepath, L"\": W=", width, L", H=", height, ", ", nodecount, L" points, ", springcount, L" springs.");
}

void Game::DestroyAt(vec2 worldCoordinates)
{
	assert(!!mWorld);
	mWorld->destroyAt(worldCoordinates);

	// TODO: publish game event
}

void Game::DrawTo(vec2 worldCoordinates)
{
	assert(!!mWorld);
	mWorld->drawTo(worldCoordinates);

	// TODO: publish game event
}

void Game::Update(
	double dt,
	GameParameters gameParameters)
{
	assert(!!mWorld);

	// TODO
	mWorld->strength = gameParameters.Strength;
	mWorld->buoyancy = gameParameters.Buoyancy;
	mWorld->waterpressure = gameParameters.WaterPressure;
	mWorld->waveheight = gameParameters.WaveHeight;
	mWorld->seadepth = gameParameters.SeaDepth;
	mWorld->oceandepthbuffer = mOceanDepth.data();

	mWorld->update(dt);
}

void Game::Render(RenderParameters renderParameters)
{
	assert(!!mWorld);

	// TODO
	mWorld->quickwaterfix = renderParameters.QuickWaterFix;
	mWorld->showstress = renderParameters.ShowStress;
	mWorld->xraymode = renderParameters.UseXRayMode;

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
	// TODO
	// glFrustum(-halfWidth, halfWidth, -halfHeight, halfHeight, 1, 1000);
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

	mWorld->render(
		renderParameters.CamX - halfWidth,
		renderParameters.CamX + halfWidth,
		renderParameters.CamY - halfHeight,
		renderParameters.CamY + halfHeight);

	glFlush();
}

std::vector<std::shared_ptr<material>> Game::LoadMaterials(std::wstring const & filepath)
{
	std::vector<std::shared_ptr<material>> materials;

	Json::Value matroot = jsonParseFile(filepath);
	for (unsigned int i = 0; i < matroot.size(); i++)
		materials.emplace_back(new material(matroot[i]));

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

