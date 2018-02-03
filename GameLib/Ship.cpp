/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "GameOpenGL.h"
#include "Log.h"
#include "RenderUtils.h"

#include <algorithm>
#include <cassert>
// TODO
//#include <cmath>

namespace Physics {

//   SSS    H     H  IIIIIII  PPPP
// SS   SS  H     H     I     P   PP
// S        H     H     I     P    PP
// SS       H     H     I     P   PP
//   SSS    HHHHHHH     I     PPPP
//      SS  H     H     I     P
//       S  H     H     I     P
// SS   SS  H     H     I     P
//   SSS    H     H  IIIIIII  P

std::unique_ptr<Ship> Ship::Create(
	World * parentWorld,
	unsigned char const * structureImageData,
	int structureImageWidth,
	int structureImageHeight,
	std::vector<std::unique_ptr<Material const>> const & allMaterials)
{
	// Prepare materials dictionary
	std::map<vec3f, Material const *> colourdict;
	for (auto const & material: allMaterials)
		colourdict[material->Colour] = material.get();

	//
	// Process image points and create points, springs, and triangles for this ship
	//

	Ship *shp = new Ship(parentWorld);

	size_t nodeCount = 0;
	size_t leakingNodeCount = 0;
	size_t springCount = 0;
	size_t trianglesCount = 0;

	std::unique_ptr<std::unique_ptr<Point *[]>[]> points(new std::unique_ptr<Point *[]>[structureImageWidth]);

	float const halfWidth = static_cast<float>(structureImageWidth) / 2.0f;

	for (int x = 0; x < structureImageWidth; ++x)
	{
		points[x] = std::unique_ptr<Point *[]>(new Point *[structureImageHeight]);

		for (int y = 0; y < structureImageHeight; ++y)
		{
			// R G B
			vec3f colour(
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 0] / 255.f,
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 1] / 255.f,
				structureImageData[(x + (structureImageHeight - y - 1) * structureImageWidth) * 3 + 2] / 255.f);

			if (colourdict.find(colour) != colourdict.end())
			{
				auto mtl = colourdict[colour];

				Point * pt = new Point(
					parentWorld,
					vec2(static_cast<float>(x) - halfWidth, static_cast<float>(y)),
					mtl,
					mtl->IsHull ? 0.0f : 1.0f);  // no buoyancy if it's a hull section

				points[x][y] = pt;
				shp->mPoints.insert(pt);
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

	static const int Directions[8][2] = {
		{ 1,  0 },	// E
		{ 1, -1 },	// NE
		{ 0, -1 },	// N
		{ -1, -1 },	// NW
		{ -1,  0 },	// W
		{ -1,  1 },	// SW
		{ 0,  1 },	// S
		{ 1,  1 }	// SE
	};

	for (int x = 0; x < structureImageWidth; ++x)
	{
		for (int y = 0; y < structureImageHeight; ++y)
		{
			Point *a = points[x][y];
			if (nullptr == a)
				continue;

			bool aIsHull = a->GetMaterial()->IsHull;

			// Check if a is leaking; a is leaking if:
			// - a is not hull, AND
			// - there is at least a hole at E, S, W, N
			if (!aIsHull)
			{
				if ((x < structureImageWidth - 1 && !points[x + 1][y])
					|| (y < structureImageHeight - 1 && !points[x][y + 1])
					|| (x > 0 && !points[x - 1][y])
					|| (y > 0 && !points[x][y - 1]))
				{
					a->SetLeaking();

					++leakingNodeCount;
				}
			}

			// First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y),
			// i.e. E, NE, N, NW - this covers each pair of points in each direction
			for (int i = 0; i < 4; ++i)
			{
				int adjx1 = x + Directions[i][0];
				int adjy1 = y + Directions[i][1];
				// Valid coordinates?
				if (adjx1 >= 0 && adjx1 < structureImageWidth && adjy1 >= 0)
				{
					assert(adjy1 < structureImageHeight); // The four directions we're checking do not include S

					Point * b = points[adjx1][adjy1]; // adjacent point in direction (i)				
					if (nullptr != b)
					{
						// b is adjacent to a at one of E, NE, N, NW						

						//
						// Create a<->b spring
						// 

						bool springIsHull = aIsHull && b->GetMaterial()->IsHull;
						Material const * const mtl = b->GetMaterial()->IsHull ? a->GetMaterial() : b->GetMaterial();    // the spring is hull iff both nodes are hull; if not we use the non-hull material.
						shp->mSprings.insert(new Spring(parentWorld, a, b, mtl));
						++springCount;

						// TODO: rename to AdjacentNonHullNodes
						if (!springIsHull)
						{
							shp->mAdjacentnodes[a].insert(b);
							shp->mAdjacentnodes[b].insert(a);
						}

						// Check adjacent point in next CW direction (for constructing triangles)
						int adjx2 = x + Directions[i + 1][0];
						int adjy2 = y + Directions[i + 1][1];
						// Valid coordinates?
						if (adjx2 >= 0 && adjx2 < structureImageWidth && adjy2 >= 0)
						{
							assert(adjy2 < structureImageHeight); // The five directions we're checking do not include S

							Point *c = points[adjx2][adjy2];
							if (nullptr != c)
							{
								shp->mTriangles.insert(new Triangle(shp, a, b, c));

								++trianglesCount;
							}
						}
					}
				}
			}
		}
	}

	LogMessage(L"Created ship: W=", structureImageWidth, L", H=", structureImageHeight, ", ",
		nodeCount, L" points, of which ", leakingNodeCount, " leaking, ", springCount,
		L" springs, ", trianglesCount, L" triangles, and ", shp->mAdjacentnodes.size(), " adjacency entries.");

	return std::unique_ptr<Ship>(shp);
}

Ship::~Ship()
{
	/*for (unsigned int i = 0; i < triangles.size(); i++)
	delete triangles[i];*/
}

void Ship::LeakWater(
	float dt,
	GameParameters const & gameParameters)
{
	// Stuff some water into all the leaking nodes, if they're not under too much pressure
	for (Point * p : mPoints)
	{
		float const pressure = p->GetPressure();
		if (p->GetIsLeaking()
			&& p->GetPosition().y < mParentWorld->GetWaterHeight(p->GetPosition().x, gameParameters) // p is in water
			&& p->GetWater() < 1.5f)	// Water pressure not already excedent // TBD: should 1.5 be 'pressure'?
		{
			p->AdjustWater(dt * gameParameters.WaterPressureAdjustment * (pressure - p->GetWater()));
		}
	}
}

void Ship::GravitateWater(float dt)
{
	// Water flows into adjacent nodes in a quantity proportional to the cos of angle the beam makes
	// against gravity (parallel with gravity => 1 (full flow), perpendicular = 0)
	for (std::map<Point *, std::set<Point*> >::iterator iter = mAdjacentnodes.begin();
		iter != mAdjacentnodes.end(); iter++)
	{
		Point *a = iter->first;
		for (std::set<Point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
		{
			Point *b = *second;
			float cos_theta = (b->GetPosition() - a->GetPosition()).normalise().dot(mParentWorld->GetGravityNormal());
			if (cos_theta > 0.0f)
			{
				float correction = std::min(0.5f * cos_theta * dt, a->GetWater());   // The 0.5 can be tuned, it's just to stop all the water being stuffed into the first node...
				a->AdjustWater(-correction);
				b->AdjustWater(correction);
			}
		}
	}
}

void Ship::BalancePressure(float dt)
{
	// If there's too much water in this node, try and push it into the others
	// (This needs to iterate over multiple frames for pressure waves to spread through water)
	for (std::map<Point*, std::set<Point*> >::iterator iter = mAdjacentnodes.begin();
		iter != mAdjacentnodes.end(); iter++)
	{
		Point *a = iter->first;
		if (a->GetWater() < 1)   // if water content is not above threshold, no need to force water out
			continue;

		for (std::set<Point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
		{
			Point *b = *second;
			float correction = (b->GetWater() - a->GetWater()) * 8.0f * dt; // can tune this number; value of 1 means will equalise in 1 second.
			a->AdjustWater(correction);
			b->AdjustWater(-correction);
		}
	}
}

void Ship::Update(
	float dt,
	GameParameters const & gameParameters)
{
	LeakWater(dt, gameParameters);

	for (int i = 0; i < 4; i++)
	{
		GravitateWater(dt);
		BalancePressure(dt);
	}

	for (int i = 0; i < 4; i++)
		BalancePressure(dt);
}


void Ship::Render() const
{
	// Render triangles
	for (auto t: mTriangles)
	{
		t->Render();
	}
}

}