/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Physics.h"

namespace Physics
{

class Point;
class Ship;

class Triangle 
{
	friend class Point;
	friend class Ship;

	Ship *parent;
	Point *a, *b, *c;
	Triangle(Ship *_parent, Point *_a, Point *_b, Point *_c);
	~Triangle();

public:
	
	Point const * GetA() const { return a; }
	Point const * GetB() const { return b; }
	Point const * GetC() const { return c; }
};

}
