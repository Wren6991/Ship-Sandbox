/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameOpenGL.h"
#include "Physics.h"
#include "RenderUtils.h"

namespace Physics
{

class Triangle 
{
public:

	Triangle(
		Ship * parentShip,
		Point * a,
		Point * b,
		Point * c)
		: mParentShip(parentShip)
		, mPointA(a)
		, mPointB(b)
		, mPointC(c)
	{
		a->AddTriangle(this);
		b->AddTriangle(this);
		c->AddTriangle(this);
	}

	~Triangle();

	inline Point const * GetPointA() const { return mPointA; }
	inline Point const * GetPointB() const { return mPointB; }
	inline Point const * GetPointC() const { return mPointC; }

	inline void Render() const
	{
		RenderUtils::RenderTriangle(
			mPointA->GetPosition(),
			mPointB->GetPosition(),
			mPointC->GetPosition(),
			mPointA->GetColour(mPointA->GetMaterial()->Colour),
			mPointB->GetColour(mPointB->GetMaterial()->Colour),
			mPointC->GetColour(mPointC->GetMaterial()->Colour));
	}

private:
	
	Ship * const mParentShip;
	Point * const mPointA;
	Point * const mPointB;
	Point * const mPointC;
};

}
