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

    enum class Characteristics
    {
        Hull    = 1,    // Does not take water
        Rope    = 2     // Ropes are drawn differently
    };

public:

	Spring(
		Ship * parentShip,
		Point * a,
		Point * b,
        Characteristics characteristics,
		Material const * material)
		: Spring(
			parentShip,
			a,
			b,			
			(a->GetPosition() - b->GetPosition()).length(),
            characteristics,
			material)
	{
	}

	Spring(
		Ship * parentShip,
		Point * a,
		Point * b,
		float restLength,
        Characteristics characteristics,
		Material const *material);

    void Destroy(Point const * pointSource);

    /*
     * Calculates the current strain - due to tension or compression - and acts depending on it.
     */
    inline void UpdateStrain(        
        GameParameters const & gameParameters,
        IGameEventHandler * gameEventHandler)
    {
        float const effectiveStrength = gameParameters.StrengthAdjustment * mMaterial->Strength;

        float strain = GetStrain();
        if (strain > effectiveStrength)
        {
            // It's broken!
            this->Destroy(nullptr);

            // Notify
            gameEventHandler->OnBreak(
                mMaterial, 
                GetParentShip()->GetParentWorld()->IsUnderwater(*mPointA, gameParameters),
                1);
        }
        else if (strain > 0.4f * effectiveStrength)
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

    inline bool IsHull() const { return 0 != (static_cast<int>(mCharacteristics) & static_cast<int>(Characteristics::Hull)); }
    inline bool IsRope() const { return 0 != (static_cast<int>(mCharacteristics) & static_cast<int>(Characteristics::Rope)); }

	inline Material const * GetMaterial() const { return mMaterial; };

    inline void Relax()
    {
        //
        // Try to space the two points by the equilibrium length 
        // (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
        //

        // 0.8 => Spring returns in dt to 80% of the equilibrium length, not all the way; the world is soft
        // 1.0 => Spring returns in dt to the equilibrium length; the world is stiff
        // 1.5 => Spring overshoots in dt by 50%; the world is unstable and tends to explode
        static const float stiffness = 0.8f;

        vec2f const displacement = (mPointB->GetPosition() - mPointA->GetPosition());
        float const displacementLength = displacement.length();
        vec2f correction = displacement.normalise(displacementLength);
        correction *= stiffness * (mRestLength - displacementLength) / (mPointA->GetMass() + mPointB->GetMass());

        // correction > 0 -> compressed, & correction is oriented towards B
        mPointA->AddToPosition(-correction * mPointB->GetMass()); // If mPointB is heavier, mPointA moves more...
        mPointB->AddToPosition(correction * mPointA->GetMass()); // ...and vice versa
    }


    /*
     * Damps the velocities of the two points, as if the points were also connected by a damper
     * along the same direction as the spring.
     *
     * Rather than modeling an actual damping force (whose deceleration would be inversely proportional
     * to a point's mass), this function simply models a decrease of the velocity.
     */
    inline void Damp(float amount)
    {
        // Get damp direction
        vec2f dampCorrection = (mPointA->GetPosition() - mPointB->GetPosition()).normalise();

        // Relative velocity dot spring direction = projected velocity;
        // amount = amount of projected velocity that damping reduces by;
        // result is oriented along the spring
        //
        // dampCorrection > 0 -> point A is faster
        dampCorrection *= ((mPointA->GetPosition() - mPointA->GetLastPosition()) - (mPointB->GetPosition() - mPointB->GetLastPosition())).dot(dampCorrection) * amount;   

        // Update velocities - slow down A by dampCorrection and speed up B by dampCorrection
        mPointA->AddToLastPosition(dampCorrection);
        mPointB->AddToLastPosition(-dampCorrection);
    }

private:

    // Strain: 
    // 0  = no tension nor compression
    // >0 = tension or compression, symmetrical
    inline float GetStrain() const
    {
        float dx = (mPointA->GetPosition() - mPointB->GetPosition()).length();
        return fabs(this->mRestLength - dx) / this->mRestLength;
    }

private:

	Point * const mPointA;
	Point * const mPointB;
	
    float const mRestLength;
    Characteristics const mCharacteristics;
	Material const * const mMaterial;

    // State variable that tracks when we enter and exit the stressed state
    bool mIsStressed;
};

}
