/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

/*
* Parameters that affect the game (physics, world).
*/
struct GameParameters
{
	float Strength;
	static constexpr float MinStrength = 0.0025f;
	static constexpr float MaxStrength = 0.5f;

	float Buoyancy;
	static constexpr float MinBuoyancy = 0.0f;
	static constexpr float MaxBuoyancy = 10.0f; 

	float WaveHeight;
	static constexpr float MinWaveHeight = 0.0f;
	static constexpr float MaxWaveHeight = 30.0f;

	float WaterPressure;
	static constexpr float MinWaterPressure = 0.0f;
	static constexpr float MaxWaterPressure = 1.0f;

	float SeaDepth;
	static constexpr float MinSeaDepth = 50.0f;
	static constexpr float MaxSeaDepth = 1000.0f;

	float DestroyRadius;

	GameParameters()
		: Strength(0.01f)
		, Buoyancy(4.0f)
		, WaveHeight(1.0f)
		, WaterPressure(0.3f)
		, SeaDepth(150.0f)
		, DestroyRadius(0.5f)
	{
	}
};
