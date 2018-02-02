/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "RenderUtils.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include <GL/gl.h>

#include <algorithm>
#include <cassert>

namespace Physics {

Triangle::Triangle(Ship *_parent, Point *_a, Point *_b, Point *_c)
{
	parent = _parent;
	a = _a;
	b = _b;
	c = _c;
	a->mTriangles.insert(this);
	b->mTriangles.insert(this);
	c->mTriangles.insert(this);
}

Triangle::~Triangle()
{
	parent->triangles.erase(this);
	a->mTriangles.erase(this);
	b->mTriangles.erase(this);
	c->mTriangles.erase(this);
}

}