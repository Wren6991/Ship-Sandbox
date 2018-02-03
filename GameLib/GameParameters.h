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
	float StrengthAdjustment;
	static constexpr float MinStrengthAdjustment = 0.0025f;
	static constexpr float MaxStrengthAdjustment = 0.5f;

	float BuoyancyAdjustment;
	static constexpr float MinBuoyancyAdjustment = 0.0f;
	static constexpr float MaxBuoyancyAdjustment = 10.0f;

	float WaterPressureAdjustment;
	static constexpr float MinWaterPressureAdjustment = 0.0f;
	static constexpr float MaxWaterPressureAdjustment = 1.0f;

	float WaveHeight;
	static constexpr float MinWaveHeight = 0.0f;
	static constexpr float MaxWaveHeight = 30.0f;

	float SeaDepth;
	static constexpr float MinSeaDepth = 50.0f;
	static constexpr float MaxSeaDepth = 1000.0f;

	float DestroyRadius;
	static constexpr float MinDestroyRadius = 0.1f;
	static constexpr float MaxDestroyRadius = 10.0f;

	GameParameters()
		: StrengthAdjustment(0.01f)
		, BuoyancyAdjustment(4.0f)
		, WaterPressureAdjustment(0.3f)
		, WaveHeight(1.0f)
		, SeaDepth(150.0f)
		, DestroyRadius(0.5f)
	{
	}
};
