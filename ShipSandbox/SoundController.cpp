/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-08
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "SoundController.h"

#include <GameLib/GameException.h>

#include <cassert>

SoundController::SoundController(
    std::shared_ptr<ResourceLoader> resourceLoader,
    ProgressCallback const & progressCallback)
    : mResourceLoader(std::move(resourceLoader))
{    
    //
    // Music
    //

    if (!mSinkingMusic.openFromFile(mResourceLoader->GetMusicFilepath("sinking_ship")))
    {
        throw GameException("Cannot load \"sinking_ship\" music");
    }

    mSinkingMusic.setLoop(true);
    mSinkingMusic.setVolume(50.0f);


    //
    // Sounds
    //

    progressCallback(1.0f, "Loading sounds...");

    // TODO
}

SoundController::~SoundController()
{
    Reset();
}

void SoundController::Reset()
{
    mSinkingMusic.stop();
}

///////////////////////////////////////////////////////////////////////////////////////

void SoundController::OnDestroy(
    Material const * material,
    unsigned int size)
{
    assert(nullptr != material);

    // TODO
}

void SoundController::OnBreak(
    Material const * material,
    unsigned int size)
{
    assert(nullptr != material);

    // TODO
}

void SoundController::OnSinkingBegin(unsigned int shipId)
{
    if (sf::SoundSource::Status::Playing != mSinkingMusic.getStatus())
    {
        mSinkingMusic.play();
    }
}

///////////////////////////////////////////////////////////////////////////////////////

