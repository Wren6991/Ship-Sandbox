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

    void Destroy(Point const * pointSource);

    /*
     * Calculates the current tension strain and acts depending on it.
     */
    inline void UpdateTensionStrain(        
        GameParameters const & gameParameters,
        IGameEventHandler * gameEventHandler)
    {
        float const effectiveStrength = gameParameters.StrengthAdjustment * mMaterial->Strength;

        float tensionStrain = GetTensionStrain();
        if (tensionStrain > 1.0f + effectiveStrength)
        {
            // It's broken!
            this->Destroy(nullptr);

            // Notify
            gameEventHandler->OnBreak(
                mMaterial, 
                GetParentShip()->GetParentWorld()->IsUnderwater(*mPointA, gameParameters),
                1);
        }
        else if (tensionStrain > 1.0f + 0.25f * effectiveStrength)
        {
            // It's stressed!
            if (!mIsStressed)
            {
                mIsStressed = true;

                // Notify
                gameEventHandler->OnStress(
                    mMaterial, 
                    GetParentShip()->GetParentWorld()->IsUnderwater(*mPointA, gameParameters),
                    1);
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

    inline void Relax()
    {
        //
        // Try to space the two points by the equilibrium length (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
        //

        vec2f const displacement = (mPointB->GetPosition() - mPointA->GetPosition());
        float const displacementLength = displacement.length();
        vec2f correction = displacement.normalise(displacementLength);
        correction *= (mRestLength - displacementLength) / ((mPointA->GetMass() + mPointB->GetMass()) * 0.80f); // * 0.8 => 25% overcorrection (stiffer, converges faster)

        // correction > 0 -> compressed, & correction is oriented towards B
        mPointA->AddToPosition(-correction * mPointB->GetMass()); // If mPointB is heavier, mPointA moves more...
        mPointB->AddToPosition(correction * mPointA->GetMass()); // ...and vice versa
    }


    inline void Damp(float amount)
    {
        // Get damp direction
        vec2f dampCorrection = (mPointA->GetPosition() - mPointB->GetPosition()).normalise();

        // Relative velocity dot spring direction = projected velocity;
        // amount = amount of projected velocity that remains after damping;
        // result is oriented along the spring
        //
        // dampCorrection > 0 -> point A is faster
        dampCorrection *= ((mPointA->GetPosition() - mPointA->GetLastPosition()) - (mPointB->GetPosition() - mPointB->GetLastPosition())).dot(dampCorrection) * amount;   

        // Update velocities - slow A by dampCorrection and speed up B by dampCorrection
        mPointA->AddToLastPosition(dampCorrection); 
        mPointB->AddToLastPosition(-dampCorrection);
    }

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
