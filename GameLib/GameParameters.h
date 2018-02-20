/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Vectors.h"

/*
* Parameters that affect the game (physics, world).
*/
struct GameParameters
{
	vec2 const Gravity;
	vec2 const GravityNormal;
	float const GravityMagnitude;

	float StrengthAdjustment;
	static constexpr float MinStrengthAdjustment = 0.0025f;
	static constexpr float MaxStrengthAdjustment = 0.5f;

	float BuoyancyAdjustment;
	static constexpr float MinBuoyancyAdjustment = 0.0f;
	static constexpr float MaxBuoyancyAdjustment = 10.0f;

	float WaterPressureAdjustment;
	static constexpr float MinWaterPressureAdjustment = 0.0f;
	static constexpr float MaxWaterPressureAdjustment = 2.0f;

	float WaveHeight;
	static constexpr float MinWaveHeight = 0.0f;
	static constexpr float MaxWaveHeight = 30.0f;

	float SeaDepth;
	static constexpr float MinSeaDepth = 20.0f;
	static constexpr float MaxSeaDepth = 300.0f;

	float DestroyRadius;
	static constexpr float MinDestroyRadius = 0.1f;
	static constexpr float MaxDestroyRadius = 10.0f;

    size_t NumberOfClouds;

	GameParameters()
		: Gravity(0.0f, -9.8f)
		, GravityNormal(Gravity.normalise())
		, GravityMagnitude(Gravity.length())
		, StrengthAdjustment(0.0075f)
		, BuoyancyAdjustment(4.0f)
		, WaterPressureAdjustment(0.75f)
		, WaveHeight(2.5f)
		, SeaDepth(150.0f)
		, DestroyRadius(0.55f)
        , NumberOfClouds(10)
	{
	}
};
