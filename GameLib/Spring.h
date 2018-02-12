/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"
#include "GameOpenGL.h"
#include "GameParameters.h"
#include "Physics.h"
#include "RenderUtils.h"
#include "Vectors.h"

namespace Physics
{

class Spring : public ShipElement<Spring>
{
public:

	Spring(
		Ship * parentShip,
		Point * a,
		Point * b,
		Material const * material)
		: Spring(
			parentShip,
			a,
			b,			
			(a->GetPosition() - b->GetPosition()).length(),
			material)
	{
	}

	Spring(
		Ship * parentShip,
		Point * a,
		Point * b,
		float length,
		Material const *material);

	void Destroy();

    void DestroyFromPoint(Point const * pointSource);

	inline bool IsStressed(float strengthAdjustment) const
	{
		// Check whether strain is more than a fraction of the word's base strength * this object's relative strength
		return GetTensionStrain() > 1 + 0.25f * (strengthAdjustment * mMaterial->Strength);
	}

	inline bool IsBroken(float strengthAdjustment) const
	{
		// Check whether strain is more than the whole word's base strength * this object's relative strength
		return GetTensionStrain() > 1 + (strengthAdjustment * mMaterial->Strength);
	}

	// Tension strain: <1=no tension stress, >1=stressed
	inline float GetTensionStrain() const
	{
		return (mPointA->GetPosition() - mPointB->GetPosition()).length() / this->mLength;
	}

    inline Point * GetPointA() { return mPointA; }
	inline Point const * GetPointA() const { return mPointA; }

    inline Point * GetPointB() { return mPointB; }
	inline Point const * GetPointB() const { return mPointB; }

	inline Material const * GetMaterial() const { return mMaterial; };

	void Update();

	void DoDamping(float amount);

	inline void Render(bool isStressed) const
	{
		//
		// If member is stressed, highlight it in red
		//

		glBegin(GL_LINES);

		if (isStressed)
            RenderUtils::SetColour(1.f, 0.f, 0.f);
		else
			RenderUtils::SetColour(mPointA->GetColour(mMaterial->Colour));

		glVertex3f(mPointA->GetPosition().x, mPointA->GetPosition().y, -1);

		if (!isStressed)
			RenderUtils::SetColour(mPointB->GetColour(mMaterial->Colour));

		glVertex3f(mPointB->GetPosition().x, mPointB->GetPosition().y, -1);

		glEnd();
	}

private:

	Point * const mPointA;
	Point * const mPointB;
	float mLength;
	Material const * const mMaterial;
};

}
