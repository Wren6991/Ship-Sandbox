/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-08
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <GameLib/IGameEventHandler.h>
#include <GameLib/ResourceLoader.h>

#include <SFML/Audio.hpp>

#include <memory>
#include <string>

class SoundController : public IGameEventHandler
{
public:

    SoundController(
        std::shared_ptr<ResourceLoader> resourceLoader,
        ProgressCallback const & progressCallback);

	virtual ~SoundController();

    void Reset();

public:

    virtual void OnDestroy(
        Material const * material,
        unsigned int size) override;

    virtual void OnBreak(
        Material const * material,
        unsigned int size) override;

    virtual void OnSinkingBegin(unsigned int shipId) override;

private:

    // Music
    sf::Music mSinkingMusic;

    std::shared_ptr<ResourceLoader> mResourceLoader;
};
