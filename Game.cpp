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
{ 1,  0},	// E
{ 1, -1},	// NE
{ 0, -1},	// N
{-1, -1},	// NW
{-1,  0},	// W
{-1,  1},	// SW
{ 0,  1},	// S
{ 1,  1}	// SE
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

	std::unique_ptr<std::unique_ptr<phys::point *[]>[]> points(new std::unique_ptr<phys::point *[]>[width]);

	//
	// Process image points and create points, springs, and triangles for this ship
	//

	size_t nodeCount = 0;
	size_t leakingNodeCount = 0;
	size_t springCount = 0;
	size_t trianglesCount = 0;

	for (int x = 0; x < width; ++x)
	{
		points[x] = std::unique_ptr<phys::point *[]>(new phys::point *[height]);

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

				phys::point * pt = new phys::point(
					mWorld.get(), 
					vec2(static_cast<float>(x) - halfWidth, static_cast<float>(y)),
					mtl.get(), 
					mtl->isHull ? 0 : 1);  // no buoyancy if it's a hull section

				points[x][y] = pt;
				shp->points.insert(pt);
				++nodeCount;
			}
			else
			{
				points[x][y] = nullptr;
			}
		}
	}

	// Points have been generated, so fill in all the beams between them.
	// If beam joins two hull nodes, it is a hull beam.
	// If a non-hull node has empty space on one of its four sides, it is automatically leaking.

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			phys::point *a = points[x][y];
			if (nullptr == a)
				continue;

			bool aIsHull = a->mtl->isHull;

			// Check if a is leaking; a is leaking if:
			// - a is not hull, AND
			// - there is at least a hole at E, S, W, N
			if (!aIsHull)
			{
				if ((x < width - 1 && !points[x + 1][y])
					|| (y < height - 1 && !points[x][y + 1])
					|| (x > 0 && !points[x - 1][y])
					|| (y > 0 && !points[x][y - 1]))
				{
					a->isLeaking = true;

					++leakingNodeCount;
				}
			}

			// First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y),
			// i.e. E, NE, N, NW - this covers each pair of points in each direction
			for (int i = 0; i < 4; ++i)
			{
				int adjx1 = x + directions[i][0];
				int adjy1 = y + directions[i][1];
				if (adjx1 >= 0 && adjx1 < width && adjy1 >= 0)
				{
					assert(adjy1 < height); // The four directions we're checking do not include S

					phys::point * b = points[adjx1][adjy1]; // adjacent point in direction (i)				
					if (b)
					{
						// b is adjacent to a at one of E, NE, N, NW						

						//
						// Create a<->b spring
						// 

						bool springIsHull = aIsHull && b->mtl->isHull;
						material *mtl = b->mtl->isHull ? a->mtl : b->mtl;    // the spring is hull iff both nodes are hull; if so we use the hull material.
						shp->springs.insert(new phys::spring(mWorld.get(), a, b, mtl, -1));
						++springCount;

						// TODO: rename to AdjacentNonHullNodes
						if (!springIsHull)
						{
							shp->adjacentnodes[a].insert(b);
							shp->adjacentnodes[b].insert(a);
						}

						// Check adjacent point in next CW direction (for constructing triangles)
						int adjx2 = x + directions[i + 1][0];
						int adjy2 = y + directions[i + 1][1];
						if (adjx2 >= 0 && adjx2 < width && adjy2 >= 0 && adjy2 < height)
						{
							phys::point *c = points[adjx2][adjy2];
							if (c)
							{
								shp->triangles.insert(new phys::ship::triangle(shp, a, b, c));

								++trianglesCount;
							}
						}
					}
				}
			}
		}
	}

	ilDeleteImage(imghandle);

	LogMessage(L"Loaded ship \"", filepath, L"\": W=", width, L", H=", height, ", ", nodeCount, L" points, of which ", leakingNodeCount, " leaking, ", springCount, L" springs, and ", trianglesCount, L" triangles.");
}

void Game::DestroyAt(
	vec2 worldCoordinates,
	float radius)
{
	assert(!!mWorld);
	mWorld->destroyAt(worldCoordinates, radius);

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

