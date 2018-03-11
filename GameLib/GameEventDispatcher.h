/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-05
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "IGameEventHandler.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

class GameEventDispatcher : public IGameEventHandler
{
public:

    GameEventDispatcher()
        : mDestroyEvents()
        , mDrawEvent(false)
        , mStressEvents()
        , mBreakEvents()
        , mSinkingBeginEvents()
        , mSinks()
    {
    }

public:

    virtual void OnDestroy(
        Material const * material, 
        unsigned int size) override
    {
        mDestroyEvents[material] += size;
    }

    virtual void OnDraw() override
    {
        mDrawEvent = true;
    }

    virtual void OnStress(
        Material const * material,
        unsigned int size) override
    {
        mStressEvents[material] += size;
    }

    virtual void OnBreak(
        Material const * material, 
        unsigned int size) override
    {
        mBreakEvents[material] += size;
    }

    virtual void OnSinkingBegin(unsigned int shipId) override
    {
        if (mSinkingBeginEvents.end() == std::find(mSinkingBeginEvents.begin(), mSinkingBeginEvents.end(), shipId))
        {
            mSinkingBeginEvents.push_back(shipId);
        }
    }

public:

    /*
     * Flushes all events aggregated so far and clears the state.
     */
    void Flush()
    {
        // Publish aggregations
        for (IGameEventHandler * sink : mSinks)
        {
            for (auto const & entry : mDestroyEvents)
            {
                sink->OnDestroy(entry.first, entry.second);
            }

            if (mDrawEvent)
            {
                sink->OnDraw();
            }

            for (auto const & entry : mStressEvents)
            {
                sink->OnStress(entry.first, entry.second);
            }

            for (auto const & entry : mBreakEvents)
            {
                sink->OnBreak(entry.first, entry.second);
            }

            for (auto const & shipId : mSinkingBeginEvents)
            {
                sink->OnSinkingBegin(shipId);
            }
        }

        // Clear collections
        mDestroyEvents.clear();
        mDrawEvent = false;
        mStressEvents.clear();
        mBreakEvents.clear();
        mSinkingBeginEvents.clear();
    }

    void RegisterSink(IGameEventHandler * sink)
    {
        mSinks.push_back(sink);
    }

private:

    // The current events being aggregated
    std::unordered_map<Material const *, unsigned int> mDestroyEvents;
    bool mDrawEvent;
    std::unordered_map<Material const *, unsigned int> mStressEvents;
    std::unordered_map<Material const *, unsigned int> mBreakEvents;
    std::vector<unsigned int> mSinkingBeginEvents;

    // The registered sinks
    std::vector<IGameEventHandler *> mSinks;
};
