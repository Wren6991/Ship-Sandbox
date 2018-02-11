/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "AABB.h"
#include "FixedSizeVector.h"
#include "GameOpenGL.h"
#include "GameParameters.h"
#include "Material.h"
#include "Physics.h"
#include "RenderUtils.h"
#include "Vectors.h"

#include <set>
#include <vector>

namespace Physics
{	

class Point : public ShipElement<Point> 
{
public:

	Point(
		Ship * parentShip,
		vec2 const & position,
		Material const * material,
		float buoyancy);

    void Destroy();

	inline vec2 const & GetPosition() const {  return mPosition;  }	
    inline void AddToPosition(vec2 const & dPosition) { mPosition += dPosition; }    
    inline void SubtractFromPosition(vec2 const & dPosition) { mPosition -= dPosition; }
	
	inline vec2 const & GetLastPosition() const { return mLastPosition; }    
    inline void AddToLastPosition(vec2 const & dPosition) { mLastPosition += dPosition; }
    inline void SubtractFromLastPosition(vec2 const & dPosition) { mLastPosition -= dPosition; }

	inline Material const * GetMaterial() const { return mMaterial; }

	inline float GetMass() const { return mMaterial->Mass; }

	inline vec2 const & GetForce() const { return mForce; }	
    inline void ZeroForce() { mForce = vec2(0, 0); }

	inline float GetBuoyancy() const { return mBuoyancy;  }

    // Dimensionally akin to Water Pressure
	inline float GetWater() const { return mWater; }
	inline void AdjustWater(float dWater) 
    { 
        mWater += dWater; 
    }

	inline bool IsLeaking() const { return mIsLeaking;  }
	inline void SetLeaking() { mIsLeaking = true; }

    
    inline auto const & GetConnectedSprings() const { return mConnectedSprings; }

    inline void AddConnectedSpring(Spring * spring) 
    { 
        mConnectedSprings.push_back(spring); 
    }

    inline void RemoveConnectedSpring(Spring * spring) 
    { 
        bool found = mConnectedSprings.erase_first(spring);

        assert(found);
        (void)found;
    }


    inline auto const & GetConnectedTriangles() const 
    { 
        return mConnectedTriangles; 
    }

    inline void AddConnectedTriangle(Triangle * triangle) 
    { 
        mConnectedTriangles.push_back(triangle); 
    }

    inline void RemoveConnectedTriangle(Triangle * triangle) 
    { 
        bool found = mConnectedTriangles.erase_first(triangle); 

        assert(found);
        (void)found;
    }

    inline vec3f GetColour(vec3f const & baseColour) const
    {
        static const vec3f WetColour(0.0f, 0.0f, 0.8f);

        float const colorWetness = fminf(mWater, 1.0f) * 0.7f;
        return baseColour * (1.0f - colorWetness) + WetColour * colorWetness;
    }

    float GetExternalWaterPressure(
        float waterLevel,
        GameParameters const & gameParameters) const
    {
        // Negative Y == under water line
        if (mPosition.y < waterLevel)
        {
            return gameParameters.GravityMagnitude * (waterLevel - mPosition.y) * 0.1f;  // 0.1 = scaling constant, represents 1/ship width
        }
        else
        {
            return 0.0f;
        }
    }

	inline AABB GetAABB() const noexcept
	{
		return AABB(mPosition - AABBRadius, mPosition + AABBRadius);
	}

	inline void ApplyForce(vec2 const & force)
	{ 
		mForce += force; 
	}	

	void Breach();

	void Update(
		float dt,
		GameParameters const & gameParameters);

private:

	static vec2 const AABBRadius;	

	vec2 mPosition;
	vec2 mLastPosition;

	Material const * mMaterial;
	vec2 mForce;
	float mBuoyancy;
	// TBD: WaterPressure?
	float mWater;
	bool mIsLeaking;

    FixedSizeVector<Spring *, 8U> mConnectedSprings;
    FixedSizeVector<Triangle *, 12U> mConnectedTriangles;    

private:

    void DestroyConnectedTriangles();
};

}
