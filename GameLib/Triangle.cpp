/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

namespace Physics {

Triangle::~Triangle()
{
	GetParentShip()->mTriangles.erase(this);
	mPointA->RemoveTriangle(this);
	mPointB->RemoveTriangle(this);
	mPointC->RemoveTriangle(this);
}

}