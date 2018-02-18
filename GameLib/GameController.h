/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Game.h"
#include "GameParameters.h"
#include "RenderContext.h"
#include "ResourceLoader.h"
#include "Vectors.h"

#include <chrono>
#include <memory>
#include <string>

/*
 * This class is responsible for managing the game, from its lifetime to the user
 * interactions.
 */
class GameController
{
public:

    static std::unique_ptr<GameController> Create();

    void ResetAndLoadShip(std::string const & filepath);
    void AddShip(std::string const & filepath);
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
        vec2f worldOffset = mRenderContext->ScreenOffset2WorldOffset(screenOffset);

        mTargetCameraPosition = mTargetCameraPosition - worldOffset;
        mStartingCameraPosition = mCurrentCameraPosition;
        mStartCameraPositionTimestamp = std::chrono::steady_clock::now();
    }

    void PanImmediate(vec2f const & screenOffset)
    {
        vec2f worldOffset = mRenderContext->ScreenOffset2WorldOffset(screenOffset);
        mRenderContext->AdjustCameraWorldPosition(worldOffset);

        mTargetCameraPosition = mCurrentCameraPosition = mRenderContext->GetCameraWorldPosition();
    }

    void ResetPan()
    {
        mRenderContext->SetCameraWorldPosition(vec2f(0, 0));
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
    }

    float GetStrengthAdjustment() const { return mGameParameters.StrengthAdjustment; }
    void SetStrengthAdjustment(float value);
    float GetMinStrengthAdjustment() const { return GameParameters::MinStrengthAdjustment;  }
    float GetMaxStrengthAdjustment() const { return GameParameters::MaxStrengthAdjustment; }

    float GetBuoyancyAdjustment() const { return mGameParameters.BuoyancyAdjustment; }
    void SetBuoyancyAdjustment(float value);
    float GetMinBuoyancyAdjustment() const { return GameParameters::MinBuoyancyAdjustment; }
    float GetMaxBuoyancyAdjustment() const { return GameParameters::MaxBuoyancyAdjustment; }

    float GetWaterPressureAdjustment() const { return mGameParameters.WaterPressureAdjustment; }
    void SetWaterPressureAdjustment(float value);
    float GetMinWaterPressureAdjustment() const { return GameParameters::MinWaterPressureAdjustment; }
    float GetMaxWaterPressureAdjustment() const { return GameParameters::MaxWaterPressureAdjustment; }

    float GetWaveHeight() const { return mGameParameters.WaveHeight; }
    void SetWaveHeight(float value);
    float GetMinWaveHeight() const { return GameParameters::MinWaveHeight; }
    float GetMaxWaveHeight() const { return GameParameters::MaxWaveHeight; }

    float GetSeaDepth() const { return mGameParameters.SeaDepth; }
    void SetSeaDepth(float value);
    float GetMinSeaDepth() const { return GameParameters::MinSeaDepth; }
    float GetMaxSeaDepth() const { return GameParameters::MaxSeaDepth; }

    float GetDestroyRadius() const { return mGameParameters.DestroyRadius; }
    void SetDestroyRadius(float value);
    float GetMinDestroyRadius() const { return GameParameters::MinDestroyRadius; }
    float GetMaxDestroyRadius() const { return GameParameters::MaxDestroyRadius; }

    bool GetShowStress() const { return mRenderContext->GetShowStress(); }
    void SetShowStress(bool value) { mRenderContext->SetShowStress(value); }

    bool GetUseXRayMode() const { return mRenderContext->GetUseXRayMode(); }
    void SetUseXRayMode(bool value) { mRenderContext->SetUseXRayMode(value); }

    bool GetShowShipThroughWater() const { return mRenderContext->GetShowShipThroughWater();  }
    void SetShowShipThroughWater(bool value) { mRenderContext->SetShowShipThroughWater(value); }

    bool GetDrawPointsOnly() const { return mRenderContext->GetDrawPointsOnly(); }
    void SetDrawPointsOnly(bool value) { mRenderContext->SetDrawPointsOnly(value); }

private:

    GameController(
        std::unique_ptr<Game> game,
        std::string const & initialShipLoaded,
        std::shared_ptr<ResourceLoader> resourceLoader)
        : mResourceLoader(std::move(resourceLoader))
        , mGame(std::move(game))
        , mLastShipLoaded(initialShipLoaded)
        , mGameParameters()
        , mRenderContext(new RenderContext(mResourceLoader))
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

private:

    std::shared_ptr<ResourceLoader> mResourceLoader;

    //
    // The game
    //

    std::unique_ptr<Game> mGame;

    std::string mLastShipLoaded;

    //
    // The game (dynamics) parameters
    //

    GameParameters mGameParameters;

    //
    // The render context
    //

    std::unique_ptr<RenderContext> mRenderContext;

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
