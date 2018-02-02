/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Vectors.h"

// Axis-Aligned Bounding Box
class AABB
{
public:
	vec2 bottomleft, topright;
	AABB() {}
	AABB(vec2 _bottomleft, vec2 _topright);
	void extendTo(AABB other);
	void render() const;
};
