/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameEventDispatcher.h"
#include "GameParameters.h"
#include "MaterialDatabase.h"
#include "Physics.h"
#include "ProgressCallback.h"
#include "RenderContext.h"
#include "ResourceLoader.h"
#include "Vectors.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

/*
 * This class is responsible for managing the game, from its lifetime to the user
 * interactions.
 */
class GameController
{
public:

    static std::unique_ptr<GameController> Create(
        std::shared_ptr<ResourceLoader> resourceLoader,
        ProgressCallback const & progressCallback);

    void RegisterGameEventHandler(IGameEventHandler * gameEventHandler);

    void ResetAndLoadShip(std::filesystem::path const & filepath);
    void AddShip(std::filesystem::path const & filepath);
    void ReloadLastShip();

    void DoStep();
    void Render();


    //
    // Interactions
    //

    void DestroyAt(vec2f const & screenCoordinates, float radiusMultiplier);
    void DrawTo(vec2f const & screenCoordinates, float strengthMultiplier);
    Physics::Point const * GetNearestPointAt(vec2 const & screenCoordinates) const;

    void SetCanvasSize(int width, int height) { mRenderContext->SetCanvasSize(width, height); }

    void Pan(vec2f const & screenOffset)
    {
        vec2f worldOffset = mRenderContext->ScreenOffsetToWorldOffset(screenOffset);

        mTargetCameraPosition = mTargetCameraPosition + worldOffset;
        mStartingCameraPosition = mCurrentCameraPosition;
        mStartCameraPositionTimestamp = std::chrono::steady_clock::now();
    }

    void PanImmediate(vec2f const & screenOffset)
    {
        vec2f worldOffset = mRenderContext->ScreenOffsetToWorldOffset(screenOffset);
        mRenderContext->AdjustCameraWorldPosition(worldOffset);

        mTargetCameraPosition = mCurrentCameraPosition = mRenderContext->GetCameraWorldPosition();
    }

    void ResetPan()
    {
        mRenderContext->SetCameraWorldPosition(vec2f(0, 0));

        mTargetCameraPosition = mCurrentCameraPosition = mRenderContext->GetCameraWorldPosition();
    }

    void AdjustZoom(float amount) 
    { 
        mTargetZoom = mTargetZoom * amount;
        mStartingZoom = mCurrentZoom;
        mStartZoomTimestamp = std::chrono::steady_clock::now();
    }

    void ResetZoom() 
    { 
        mRenderContext->SetZoom(1.0); 

        mTargetZoom = mCurrentZoom = mRenderContext->GetZoom();
    }

    float GetStrengthAdjustment() const { return mGameParameters.StrengthAdjustment; }
    void SetStrengthAdjustment(float value) { mGameParameters.StrengthAdjustment = value; }
    float GetMinStrengthAdjustment() const { return GameParameters::MinStrengthAdjustment;  }
    float GetMaxStrengthAdjustment() const { return GameParameters::MaxStrengthAdjustment; }

    float GetBuoyancyAdjustment() const { return mGameParameters.BuoyancyAdjustment; }
    void SetBuoyancyAdjustment(float value) { mGameParameters.BuoyancyAdjustment = value; }
    float GetMinBuoyancyAdjustment() const { return GameParameters::MinBuoyancyAdjustment; }
    float GetMaxBuoyancyAdjustment() const { return GameParameters::MaxBuoyancyAdjustment; }

    float GetWaterPressureAdjustment() const { return mGameParameters.WaterPressureAdjustment; }
    void SetWaterPressureAdjustment(float value) { mGameParameters.WaterPressureAdjustment = value; }
    float GetMinWaterPressureAdjustment() const { return GameParameters::MinWaterPressureAdjustment; }
    float GetMaxWaterPressureAdjustment() const { return GameParameters::MaxWaterPressureAdjustment; }

    float GetWaveHeight() const { return mGameParameters.WaveHeight; }
    void SetWaveHeight(float value) { mGameParameters.WaveHeight = value; }
    float GetMinWaveHeight() const { return GameParameters::MinWaveHeight; }
    float GetMaxWaveHeight() const { return GameParameters::MaxWaveHeight; }

    float GetSeaDepth() const { return mGameParameters.SeaDepth; }
    void SetSeaDepth(float value) { mGameParameters.SeaDepth = value; }
    float GetMinSeaDepth() const { return GameParameters::MinSeaDepth; }
    float GetMaxSeaDepth() const { return GameParameters::MaxSeaDepth; }

    float GetDestroyRadius() const { return mGameParameters.DestroyRadius; }
    void SetDestroyRadius(float value) { mGameParameters.DestroyRadius = value; }
    float GetMinDestroyRadius() const { return GameParameters::MinDestroyRadius; }
    float GetMaxDestroyRadius() const { return GameParameters::MaxDestroyRadius; }

    float GetAmbientLightIntensity() const { return mRenderContext->GetAmbientLightIntensity(); }
    void SetAmbientLightIntensity(float value) { mRenderContext->SetAmbientLightIntensity(value); }

    float GetWaterTransparency() const { return mRenderContext->GetWaterTransparency(); }
    void SetWaterTransparency(float value) { mRenderContext->SetWaterTransparency(value); }

    float GetLightDiffusionAdjustment() const { return mGameParameters.LightDiffusionAdjustment; }
    void SetLightDiffusionAdjustment(float value) { mGameParameters.LightDiffusionAdjustment = value; }

    bool GetShowShipThroughWater() const { return mRenderContext->GetShowShipThroughWater();  }
    void SetShowShipThroughWater(bool value) { mRenderContext->SetShowShipThroughWater(value); }

    ShipRenderMode GetShipRenderMode() const { return mRenderContext->GetShipRenderMode(); }
    void SetShipRenderMode(ShipRenderMode shipRenderMode) { mRenderContext->SetShipRenderMode(shipRenderMode); }

    bool GetShowShipStress() const { return mRenderContext->GetShowStressedSprings(); }
    void SetShowShipStress(bool value) { mRenderContext->SetShowStressedSprings(value); }

    vec2f ScreenToWorld(vec2f const & screenCoordinates) const
    {
        return mRenderContext->ScreenToWorld(screenCoordinates);
    }

private:

    GameController(
        std::unique_ptr<RenderContext> renderContext,
        std::shared_ptr<GameEventDispatcher> gameEventDispatcher,
        std::shared_ptr<ResourceLoader> resourceLoader,
        MaterialDatabase && materials)
        : mGameParameters()
        , mLastShipLoadedFilePath()
        , mRenderContext(std::move(renderContext))
        , mGameEventDispatcher(std::move(gameEventDispatcher))
        , mResourceLoader(std::move(resourceLoader))
        , mWorld(new Physics::World(
            mGameEventDispatcher,
            mGameParameters))
        , mMaterials(std::move(materials))        
         // Smoothing
        , mCurrentZoom(mRenderContext->GetZoom())
        , mTargetZoom(mCurrentZoom)
        , mStartingZoom(mCurrentZoom)
        , mStartZoomTimestamp()
        , mCurrentCameraPosition(mRenderContext->GetCameraWorldPosition())
        , mTargetCameraPosition(mCurrentCameraPosition)
        , mStartingCameraPosition(mCurrentCameraPosition)
        , mStartCameraPositionTimestamp()
    {
    }
    
    static void SmoothToTarget(
        float & currentValue,
        float startingValue,
        float targetValue,
        std::chrono::steady_clock::time_point startingTime);

    void Reset();

    void AddShip(ShipDefinition shipDefinition);

private:

    //
    // Our current state
    //

    GameParameters mGameParameters;
    std::filesystem::path mLastShipLoadedFilePath;

    //
    // The doers 
    //

    std::unique_ptr<RenderContext> mRenderContext;
    std::shared_ptr<GameEventDispatcher> mGameEventDispatcher;
    std::shared_ptr<ResourceLoader> mResourceLoader;

    //
    // The world
    //

    std::unique_ptr<Physics::World> mWorld;
    MaterialDatabase mMaterials;
        

    //
    // The current render parameters that we're smoothing to
    //

    static constexpr int SmoothMillis = 500;

    float mCurrentZoom;
    float mTargetZoom;
    float mStartingZoom;
    std::chrono::steady_clock::time_point mStartZoomTimestamp;

    vec2f mCurrentCameraPosition;
    vec2f mTargetCameraPosition;
    vec2f mStartingCameraPosition;
    std::chrono::steady_clock::time_point mStartCameraPositionTimestamp;
};
