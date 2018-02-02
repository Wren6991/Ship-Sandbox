/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"
#include "Physics.h"
#include "Vectors.h"

#include <set>

namespace Physics
{

class Point;
class Spring;
class Triangle;
class World;

class Ship
{
public:
	World *wld;
	std::set<Point*> points;
	std::set<Spring*> springs;
	std::map<Point *, std::set<Point*>> adjacentnodes;
	std::set<Triangle*> triangles;
	void render() const;
	void leakWater(double dt);
	void gravitateWater(double dt);
	void balancePressure(double dt);


	static std::unique_ptr<Ship> Create(
		World * parentWorld,
		unsigned char const * structureImageData,
		int structureImageWidth,
		int structureImageHeight,
		std::vector<std::unique_ptr<Material const>> const & allMaterials);
		
	~Ship();

	void update(double dt);

private:
	Ship(World *_parent);
};

}