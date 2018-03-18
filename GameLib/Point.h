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
#include "GameParameters.h"
#include "Material.h"
#include "Physics.h"
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
		float buoyancy,
        int elementIndex);

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

    inline void AddForce(vec2 const & force)
    {
        mForce += force;
    }

    inline void ZeroForce() { mForce = vec2(0, 0); }

	
    inline float GetBuoyancy() const { return mBuoyancy;  }


    inline bool IsLeaking() const { return mIsLeaking; }

    inline void SetLeaking() { mIsLeaking = true; }


    // Dimensionally akin to Water Pressure
	inline float GetWater() const { return mWater; }

	inline void AddWater(float dWater) 
    { 
        mWater += dWater; 
    }


    inline float GetLight() const { return mLight; }

    inline void ZeroLight()
    {
        mLight = 0.0f;
    }

    inline void AdjustLight(float light)
    {
        if (light > mLight)
        {
            mLight = light;
        }
    }

    inline int GetElementIndex() const { return mElementIndex; }

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


    inline ElectricalElement const * GetConnectedElectricalElement() const
    {
        return mConnectedElectricalElement;
    }

    inline ElectricalElement * GetConnectedElectricalElement()
    {
        return mConnectedElectricalElement;
    }

    inline void SetConnectedElectricalElement(ElectricalElement * electricalElement)
    {
        assert(nullptr == mConnectedElectricalElement);
        mConnectedElectricalElement = electricalElement;
    }

    inline uint64_t GetConnectedComponentId() const { return mConnectedComponentId;  }
    inline void SetConnectedComponentId(uint64_t connectedComponentId) { mConnectedComponentId = connectedComponentId; }

    inline uint64_t GetCurrentConnectedComponentDetectionStepSequenceNumber() const { return mCurrentConnectedComponentDetectionStepSequenceNumber; }
    inline void SetConnectedComponentDetectionStepSequenceNumber(uint64_t connectedComponentDetectionStepSequenceNumber) { mCurrentConnectedComponentDetectionStepSequenceNumber = connectedComponentDetectionStepSequenceNumber; }

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

	void Breach();

	void Update(
		float dt,
        float dragCoefficient,
		GameParameters const & gameParameters);

private:

	static vec2 const AABBRadius;	

	vec2 mPosition;
	vec2 mLastPosition;
    vec2 mForce;

	Material const * mMaterial;
	
	float mBuoyancy;

    bool mIsLeaking;

	// Total quantity of water, 0.0->+INF (== internal water pressure)
	float mWater;

    // Total illumination, 0.0->1.0
    float mLight;

    // The ID of this point, used by graphics elements to refer to vertices
    int const mElementIndex;

    FixedSizeVector<Spring *, 8U> mConnectedSprings;
    FixedSizeVector<Triangle *, 12U> mConnectedTriangles;    
    ElectricalElement * mConnectedElectricalElement;

    size_t mConnectedComponentId; // Starts from 1
    size_t mCurrentConnectedComponentDetectionStepSequenceNumber;

private:

    void DestroyConnectedTriangles();
};

}
