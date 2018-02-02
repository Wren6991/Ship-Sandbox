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
class Ship; 
class Triangle;
class World;

class Spring
{
	friend class World;
	friend class Point;
	friend class Ship;

	World *wld;
	Point *a, *b;
	double length;
	Material const * mtl;
public:
	Spring(World *_parent, Point *_a, Point *_b, Material const *_mtl, double _length = -1);
	~Spring();
	void update();
	void damping(float amount);
	void render(bool isStressed = false) const;
	bool isStressed();
	bool isBroken();
	Point const * GetPointA() const { return a;  }
	Point const * GetPointB() const { return b; }
	Material const * GetMaterial() const { return mtl; };
};

}
