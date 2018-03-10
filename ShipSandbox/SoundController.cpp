/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-08
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "SoundController.h"

#include <GameLib/GameException.h>
#include <GameLib/Material.h>

#include <cassert>
#include <regex>

SoundController::SoundController(
    std::shared_ptr<ResourceLoader> resourceLoader,
    ProgressCallback const & progressCallback)
    : mResourceLoader(std::move(resourceLoader))
    , mCurrentVolume(100.0f)
    , mCrashSoundBuffers()
    , mCurrentlyPlayingSounds()
    , mSinkingMusic()
{    
    //
    // Initialize random engine
    //

    std::seed_seq seed_seq({ 1, 242, 19730528 });
    mRandomEngine = std::ranlux48_base(seed_seq);

    //
    // Music
    //

    if (!mSinkingMusic.openFromFile(mResourceLoader->GetMusicFilepath("sinking_ship")))
    {
        throw GameException("Cannot load \"sinking_ship\" music");
    }

    mSinkingMusic.setLoop(true);


    //
    // Sounds
    //

    auto filenames = mResourceLoader->GetSoundFilenames();
    for (size_t i = 0; i < filenames.size(); ++i)    
    {
        std::string const & filename = filenames[i];

        // Notify progress
        progressCallback(static_cast<float>(i + 1) / static_cast<float>(filenames.size()), "Loading sounds...");
        

        //
        // Load sound buffer
        //

        std::unique_ptr<sf::SoundBuffer> soundBuffer = std::make_unique<sf::SoundBuffer>();
        if (!soundBuffer->loadFromFile(mResourceLoader->GetSoundFilepath(filename)))
        {
            throw GameException("Cannot load sound \"" + filename + "\"");
        }



        //
        // Parse filename
        //        

        std::regex soundTypeRegex(R"(([^_]+)_.+)");
        std::smatch soundTypeMatch;
        if (!std::regex_match(filename, soundTypeMatch, soundTypeRegex))
        {
            throw GameException("Sound filename \"" + filename + "\" is not recognized");
        }

        assert(soundTypeMatch.size() == 1 + 1);
        SoundType soundType = StrToSoundType(soundTypeMatch[1].str());
        if (soundType == SoundType::Break || soundType == SoundType::Destroy || soundType == SoundType::Stress)
        {
            //
            // Crash sound
            //

            std::regex crashRegex(R"((\w+)_(\w+)_(\w+)_(?:(underwater)_)?\d+)");
            std::smatch crashMatch;
            if (!std::regex_match(filename, crashMatch, crashRegex))
            {
                throw GameException("Crash sound filename \"" + filename + "\" is not recognized");
            }

            assert(crashMatch.size() == 1 + 4);

            // 1. Parse SoundElementType
            Material::SoundProperties::SoundElementType soundElementType = Material::SoundProperties::StrToSoundElementType(crashMatch[2].str());

            // 2. Parse Size
            SizeType sizeType = StrToSizeType(crashMatch[3].str());

            // 3. Parse Underwater
            bool isUnderwater;
            if (crashMatch[4].matched)
            {
                assert(crashMatch[4].str() == "underwater");
                isUnderwater = true;
            }
            else
            {
                isUnderwater = false;
            }


            //
            // Store sound buffer
            //

            mCrashSoundBuffers[std::make_tuple(soundType, soundElementType, sizeType, isUnderwater)].emplace_back(std::move(soundBuffer));
        }
        else
        {
            throw GameException("Sound type of sound filename \"" + filename + "\" is not recognized");
        }
    }
}

SoundController::~SoundController()
{
    Reset();
}

void SoundController::SetMute(bool isMute)
{
    if (isMute)
        sf::Listener::setGlobalVolume(0.0f);
    else
        sf::Listener::setGlobalVolume(mCurrentVolume);
}

void SoundController::SetVolume(float volume)
{
    mCurrentVolume = volume;
    sf::Listener::setGlobalVolume(mCurrentVolume);
}

void SoundController::Update()
{
    // TODO: scavenge mCurrentlyPlayingSounds
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

    PlayCrashSound(SoundType::Destroy, material, size, /*TODO*/false);
}

void SoundController::OnStress(
    Material const * material,
    unsigned int size)
{
    assert(nullptr != material);

    PlayCrashSound(SoundType::Stress, material, size, /*TODO*/false);
}

void SoundController::OnBreak(
    Material const * material,
    unsigned int size)
{
    assert(nullptr != material);

    PlayCrashSound(SoundType::Break, material, size, /*TODO*/false);
}

void SoundController::OnSinkingBegin(unsigned int shipId)
{
    if (sf::SoundSource::Status::Playing != mSinkingMusic.getStatus())
    {
        mSinkingMusic.play();
    }
}

///////////////////////////////////////////////////////////////////////////////////////

void SoundController::PlayCrashSound(
    SoundType soundType,
    Material const * material,
    unsigned int size,
    bool isUnderwater)
{
    assert(nullptr != material);

    if (!material->Sound)
        return;

    // Convert size
    SizeType sizeType;
    if (size < 2)
        sizeType = SizeType::Small;
    else if (size < 10)
        sizeType = SizeType::Medium;
    else
        sizeType = SizeType::Large;


    //
    // Find vector
    //
    
    auto it = mCrashSoundBuffers.find(std::make_tuple(soundType, material->Sound->ElementType, sizeType, isUnderwater));

    if (it == mCrashSoundBuffers.end())
    {
        // Find a smaller one
        for (int s = static_cast<int>(sizeType) - 1; s >= static_cast<int>(SizeType::Min); --s)
        {
            it = mCrashSoundBuffers.find(std::make_tuple(soundType, material->Sound->ElementType, static_cast<SizeType>(s), isUnderwater));
            if (it != mCrashSoundBuffers.end())
            {
                break;
            }
        }
    }

    if (it == mCrashSoundBuffers.end())
    {
        // Find this or smaller size with different underwater
        for (int s = static_cast<int>(sizeType); s >= static_cast<int>(SizeType::Min); --s)
        {
            it = mCrashSoundBuffers.find(std::make_tuple(soundType, material->Sound->ElementType, static_cast<SizeType>(s), !isUnderwater));
            if (it != mCrashSoundBuffers.end())
            {
                break;
            }
        }
    }

    if (it == mCrashSoundBuffers.end())
    {
        // No luck
        return;
    }

    
    //
    // Play sound
    //

    ChooseAndPlaySound(it->second);
}

void SoundController::ChooseAndPlaySound(std::vector<std::unique_ptr<sf::SoundBuffer>> const & soundBuffers)
{
    //
    // Choose sound buffer
    //

    assert(!soundBuffers.empty());

    std::uniform_int_distribution<size_t> dis(0, soundBuffers.size() - 1);
    sf::SoundBuffer * soundBuffer = soundBuffers[dis(mRandomEngine)].get();

    // TODO: see if playing since too little, etc.

    // TODOTEST
    std::unique_ptr<sf::Sound> sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*soundBuffer);

    sound->play();

    mCurrentlyPlayingSounds.emplace_back(
        std::move(sound),
        std::chrono::steady_clock::now());
}