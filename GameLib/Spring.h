/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "IGameEventHandler.h"
#include "Material.h"
#include "GameParameters.h"
#include "Physics.h"
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
		float restLength,
		Material const *material);

	void Destroy();

    void DestroyFromPoint(Point const * pointSource);

    /*
     * Calculates the current tension strain and acts depending on it.
     */
    inline void UpdateTensionStrain(        
        float strengthAdjustment,
        IGameEventHandler * gameEventHandler)
    {
        float const effectiveStrength = strengthAdjustment * mMaterial->Strength;

        float tensionStrain = GetTensionStrain();
        if (tensionStrain > 1.0f + effectiveStrength)
        {
            // It's broken!
            this->Destroy();

            // Notify
            gameEventHandler->OnBreak(mMaterial, 1);
        }
        else if (tensionStrain > 1.0f + 0.25f * effectiveStrength)
        {
            // It's stressed!
            if (!mIsStressed)
            {
                mIsStressed = true;

                // Notify
                gameEventHandler->OnStress(mMaterial, 1);
            }
        }
        else
        {
            // Just fine
            mIsStressed = false;
        }
    }

	inline bool IsStressed() const
	{
        return mIsStressed;
	}

    inline Point * GetPointA() { return mPointA; }
	inline Point const * GetPointA() const { return mPointA; }

    inline Point * GetPointB() { return mPointB; }
	inline Point const * GetPointB() const { return mPointB; }

	inline Material const * GetMaterial() const { return mMaterial; };

	void Update();

	void DoDamping(float amount);

private:

    // Tension strain: <1=no tension stress, >1=stressed
    inline float GetTensionStrain() const
    {
        return (mPointA->GetPosition() - mPointB->GetPosition()).length() / this->mRestLength;
    }

private:

	Point * const mPointA;
	Point * const mPointB;
	
    float const mRestLength;
	Material const * const mMaterial;

    // State variable to track when we enter and exit the stressed state
    bool mIsStressed;
};

}
