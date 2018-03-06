/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-05
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "IGameEventHandler.h"

#include <unordered_map>
#include <vector>

class GameEventDispatcher : public IGameEventHandler
{
public:

    GameEventDispatcher()
        : mDestroyEvents()
        , mBreakEvents()
        , mIsSinkingBeginEventFired(false)
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

    virtual void OnBreak(
        Material const * material, 
        unsigned int size) override
    {
        mBreakEvents[material] += size;
    }

    virtual void OnSinkingBegin() override
    {
        mIsSinkingBeginEventFired = true;
    }

public:

    void OnStepStart()
    {
        mDestroyEvents.clear();
        mBreakEvents.clear();
        mIsSinkingBeginEventFired = false;
    }

    void OnStepEnd()
    {
        // Publish aggregations
        for (IGameEventHandler * sink : mSinks)
        {
            for (auto const & entry : mDestroyEvents)
            {
                sink->OnDestroy(entry.first, entry.second);
            }

            for (auto const & entry : mBreakEvents)
            {
                sink->OnBreak(entry.first, entry.second);
            }

            if (mIsSinkingBeginEventFired)
            {
                sink->OnSinkingBegin();
            }
        }
    }

    void RegisterSink(IGameEventHandler * sink)
    {
        mSinks.push_back(sink);
    }

private:

    // The current events being aggregated
    std::unordered_map<Material const *, unsigned int> mDestroyEvents;
    std::unordered_map<Material const *, unsigned int> mBreakEvents;
    bool mIsSinkingBeginEventFired;

    // The registered sinks
    std::vector<IGameEventHandler *> mSinks;
};
