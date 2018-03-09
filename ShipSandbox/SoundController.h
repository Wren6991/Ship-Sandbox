/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-08
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <GameLib/IGameEventHandler.h>
#include <GameLib/ResourceLoader.h>
#include <GameLib/TupleKeys.h>

#include <SFML/Audio.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <deque>
#include <memory>
#include <string>

class SoundController : public IGameEventHandler
{
public:

    SoundController(
        std::shared_ptr<ResourceLoader> resourceLoader,
        ProgressCallback const & progressCallback);

	virtual ~SoundController();

    void Update();

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

    enum class SizeType : int
    {
        Min = 0,

        Small = 0,
        Medium = 1,
        Large = 2,

        Max = 2
    };

    static SizeType StrToSizeType(std::string const & str)
    {
        std::string lstr = str;
        std::transform(
            lstr.begin(),
            lstr.end(),
            lstr.begin(),
            [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

        if (lstr == "small")
            return SizeType::Small;
        else if (lstr == "medium")
            return SizeType::Medium;
        else if (lstr == "large")
            return SizeType::Large;
        else
            throw GameException("Unrecognized SizeType \"" + str + "\"");
    }

private:

    void PlayCrashSound(
        Material const * material,
        unsigned int size,
        bool isUnderwater);

    void ChooseAndPlaySound(std::vector<std::unique_ptr<sf::SoundBuffer>> const & soundBuffers);

private:

    std::shared_ptr<ResourceLoader> mResourceLoader;

    //
    // Sounds
    //

    unordered_tuple_map<
        std::tuple<Material::SoundProperties::SoundElementType, SizeType, bool>,
        std::vector<std::unique_ptr<sf::SoundBuffer>>> mCrashSoundBuffers;

    struct PlayingSound
    {
        std::unique_ptr<sf::Sound> const Sound;
        std::chrono::steady_clock::time_point const StartedTimestamp;

        PlayingSound(
            std::unique_ptr<sf::Sound> sound,
            std::chrono::steady_clock::time_point startedTimestamp)
            : Sound(std::move(sound))
            , StartedTimestamp(startedTimestamp)
        {
        }

    };

    std::deque<PlayingSound> mCurrentlyPlayingSounds;

    //
    // Music
    //

    sf::Music mSinkingMusic;
};
