/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "GameController.h"

#include "GameMath.h"
#include "Log.h"

// The dt of each step
static constexpr float StepTimeDuration = 0.02f;

std::unique_ptr<GameController> GameController::Create(
    std::shared_ptr<ResourceLoader> resourceLoader,
    ProgressCallback const & progressCallback)
{
    // Create game dispatcher
    std::shared_ptr<GameEventDispatcher> gameEventDispatcher = std::make_shared<GameEventDispatcher>();

	// Create game
	std::unique_ptr<Game> game = Game::Create(
        gameEventDispatcher,
        resourceLoader);

    // Create render context
    std::unique_ptr<RenderContext> renderContext = std::make_unique<RenderContext>(
        *resourceLoader,
        [&progressCallback](float progress, std::string const & message)
        {
            progressCallback(0.9f * progress, message);
        });

	//
	// Load initial ship
	//

    progressCallback(1.0f, "Loading initial ship...");

    std::string const initialShipFilename = "Data/default_ship.png";
    auto shipDefinition = resourceLoader->LoadShipDefinition(initialShipFilename);

	game->AddShip(shipDefinition);

    renderContext->AddShip(0, shipDefinition.TextureImage); // TODO: refactor this and get ID


    //
    // Create controller
    //

	return std::unique_ptr<GameController>(
		new GameController(
			std::move(game),
			initialShipFilename,
            std::move(renderContext),
            std::move(gameEventDispatcher),
            std::move(resourceLoader)));
}

void GameController::RegisterGameEventHandler(IGameEventHandler * gameEventHandler)
{
    assert(!!mGameEventDispatcher);
    mGameEventDispatcher->RegisterSink(gameEventHandler);
}

void GameController::ResetAndLoadShip(std::string const & filepath)
{
	assert(!!mGame);    
	mGame->Reset();

    auto shipDefinition = mResourceLoader->LoadShipDefinition(filepath);
    mGame->AddShip(shipDefinition);
    
    assert(!!mRenderContext);
    mRenderContext->Reset();
    mRenderContext->AddShip(0, shipDefinition.TextureImage); // TODO: refactor this and get ID

	mLastShipLoaded = filepath;
}

void GameController::AddShip(std::string const & filepath)
{
	assert(!!mGame);    
    auto shipDefinition = mResourceLoader->LoadShipDefinition(filepath);
    mGame->AddShip(shipDefinition);

    assert(!!mRenderContext);
    mRenderContext->AddShip(0, shipDefinition.TextureImage); // TODO: refactor this and get ID

	mLastShipLoaded = filepath;
}

void GameController::ReloadLastShip()
{
	if (mLastShipLoaded.empty())
	{
		throw std::runtime_error("No ship has been loaded yet");
	}

	assert(!!mGame);    
	mGame->Reset();

    auto shipDefinition = mResourceLoader->LoadShipDefinition(mLastShipLoaded);
    mGame->AddShip(shipDefinition);

    assert(!!mRenderContext);
    mRenderContext->Reset();
    mRenderContext->AddShip(0, shipDefinition.TextureImage); // TODO: refactor this and get ID
}

void GameController::DoStep()
{
	// Update game
	assert(!!mGame);
	mGame->Update(
        StepTimeDuration,
		mGameParameters);

    // Flush events
    mGameEventDispatcher->Flush();
}

void GameController::Render()
{
    //
    // Do zoom smoothing
    //

    if (mCurrentZoom != mTargetZoom)
    {
        SmoothToTarget(
            mCurrentZoom,
            mStartingZoom,
            mTargetZoom,
            mStartZoomTimestamp);

        mRenderContext->SetZoom(mCurrentZoom);
    }


    //
    // Do camera smoothing
    //

    if (mCurrentCameraPosition != mTargetCameraPosition)
    {
        SmoothToTarget(
            mCurrentCameraPosition.x,
            mStartingCameraPosition.x,
            mTargetCameraPosition.x,
            mStartCameraPositionTimestamp);

        SmoothToTarget(
            mCurrentCameraPosition.y,
            mStartingCameraPosition.y,
            mTargetCameraPosition.y,
            mStartCameraPositionTimestamp);

        mRenderContext->SetCameraWorldPosition(mCurrentCameraPosition);
    }

    //
	// Render game
    //

	assert(!!mGame);
	mGame->Render(mGameParameters, *mRenderContext);
}

/////////////////////////////////////////////////////////////
// Interactions
/////////////////////////////////////////////////////////////

void GameController::DestroyAt(
    vec2f const & screenCoordinates, 
    float radiusMultiplier)
{
	vec2f worldCoordinates = mRenderContext->ScreenToWorld(screenCoordinates);

    LogMessage("DestroyAt: ", worldCoordinates.toString(), " * ", radiusMultiplier);

	// Apply action
	assert(!!mGame);
	mGame->DestroyAt(
		worldCoordinates,
		mGameParameters.DestroyRadius * radiusMultiplier,
        mGameParameters);

    // Flush events
    mGameEventDispatcher->Flush();
}

void GameController::DrawTo(
    vec2 const & screenCoordinates,
    float strengthMultiplier)
{
	vec2f worldCoordinates = mRenderContext->ScreenToWorld(screenCoordinates);

	// Apply action
	assert(!!mGame);
	mGame->DrawTo(worldCoordinates, 50000.0f * strengthMultiplier);

    // Notify
    mGameEventDispatcher->OnDraw();

    // Flush events
    mGameEventDispatcher->Flush();
}

Physics::Point const * GameController::GetNearestPointAt(vec2 const & screenCoordinates) const
{
    vec2f worldCoordinates = mRenderContext->ScreenToWorld(screenCoordinates);

    assert(!!mGame);
    return mGame->GetNearestPointAt(worldCoordinates, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////

void GameController::SmoothToTarget(
    float & currentValue,
    float startingValue,
    float targetValue,
    std::chrono::steady_clock::time_point startingTime)
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // Amplitude - summing up pieces from zero to PI yields PI/2
    float amp = (targetValue - startingValue) / (Pi<float> / 2.0f);

    // X - after SmoothMillis we want PI
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startingTime);
    float x = static_cast<float>(elapsed.count()) * Pi<float> / static_cast<float>(SmoothMillis);
    float dv = amp * sinf(x) * sinf(x);

    float oldCurrentValue = currentValue;
    currentValue += dv;

    // Check if we've overshot
    if ((targetValue - oldCurrentValue) * (targetValue - currentValue) < 0.0f)
    {
        // Overshot
        currentValue = targetValue;
    }
}
