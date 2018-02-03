/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "AABB.h"

#include "GameOpenGL.h"
#include "RenderUtils.h"

AABB::AABB(vec2 _bottomleft, vec2 _topright)
{
	bottomleft = _bottomleft;
	topright = _topright;
}

void AABB::extendTo(AABB other)
{
	if (other.bottomleft.x < bottomleft.x)
		bottomleft.x = other.bottomleft.x;
	if (other.bottomleft.y < bottomleft.y)
		bottomleft.y = other.bottomleft.y;
	if (other.topright.x > topright.x)
		topright.x = other.topright.x;
	if (other.topright.y > topright.y)
		topright.y = other.topright.y;
}

void AABB::render() const
{
	RenderUtils::RenderBox(
		bottomleft,
		topright);
}
