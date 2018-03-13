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
#include <memory>
#include <random>
#include <string>
#include <vector>

class SoundController : public IGameEventHandler
{
public:

    SoundController(
        std::shared_ptr<ResourceLoader> resourceLoader,
        ProgressCallback const & progressCallback);

	virtual ~SoundController();

    void SetMute(bool isMute);

    void SetVolume(float volume);

    void HighFrequencyUpdate();

    void LowFrequencyUpdate();

    void Reset();

public:

    virtual void OnDestroy(
        Material const * material,
        bool isUnderwater,
        unsigned int size) override;

    virtual void OnDraw() override;

    virtual void OnStress(
        Material const * material,
        bool isUnderwater,
        unsigned int size) override;

    virtual void OnBreak(
        Material const * material,
        bool isUnderwater,
        unsigned int size) override;

    virtual void OnSinkingBegin(unsigned int shipId) override;

private:

    enum class SoundType
    {
        Break,
        Destroy,
        Draw,
        Stress,
    };

    static SoundType StrToSoundType(std::string const & str)
    {
        std::string lstr = str;
        std::transform(
            lstr.begin(),
            lstr.end(),
            lstr.begin(),
            [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

        if (lstr == "break")
            return SoundType::Break;
        else if (lstr == "destroy")
            return SoundType::Destroy;
        else if (lstr == "draw")
            return SoundType::Draw;
        else if (lstr == "stress")
            return SoundType::Stress;
        else
            throw GameException("Unrecognized SoundType \"" + str + "\"");
    }

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

    struct SoundInfo
    {
        std::vector<std::unique_ptr<sf::SoundBuffer>> SoundBuffers;
        size_t LastPlayedSoundIndex;

        SoundInfo()
            : SoundBuffers()
            , LastPlayedSoundIndex(0u)
        {
        }
    };

    struct PlayingSound
    {
        SoundType Type;
        std::unique_ptr<sf::Sound> Sound;
        std::chrono::steady_clock::time_point StartedTimestamp;

        PlayingSound(
            SoundType type,
            std::unique_ptr<sf::Sound> sound,
            std::chrono::steady_clock::time_point startedTimestamp)
            : Type(type)
            , Sound(std::move(sound))
            , StartedTimestamp(startedTimestamp)
        {
        }
    };

private:

    void PlayCrashSound(
        SoundType soundType,
        Material const * material,
        unsigned int size,
        bool isUnderwater);

    void ChooseAndPlaySound(
        SoundType soundType,
        SoundInfo & soundInfo);

    void ScavengeStoppedSounds();

    void ScavengeOldestSound(SoundType soundType);    

private:

    std::shared_ptr<ResourceLoader> mResourceLoader;

    std::ranlux48_base mRandomEngine;

    float mCurrentVolume;

    //
    // Sounds
    //

    static constexpr size_t MaxPlayingSounds{ 100 };
    static constexpr std::chrono::milliseconds MinDeltaTimeSound{ 100 };

    unordered_tuple_map<
        std::tuple<SoundType, Material::SoundProperties::SoundElementType, SizeType, bool>,
        SoundInfo> mCrashSoundBuffers;

    std::unique_ptr<sf::SoundBuffer> mDrawSoundBuffer;
    std::unique_ptr<sf::Sound> mDrawSound;

    std::vector<PlayingSound> mCurrentlyPlayingSounds;

    //
    // Music
    //

    sf::Music mSinkingMusic;
};
