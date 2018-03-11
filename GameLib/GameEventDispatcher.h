/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-05
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "IGameEventHandler.h"
#include "TupleKeys.h"

#include <algorithm>
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
        bool isUnderwater,
        unsigned int size) override
    {
        mDestroyEvents[std::make_tuple(material, isUnderwater)] += size;
    }

    virtual void OnDraw() override
    {
        mDrawEvent = true;
    }

    virtual void OnStress(
        Material const * material,
        bool isUnderwater,
        unsigned int size) override
    {
        mStressEvents[std::make_tuple(material, isUnderwater)] += size;
    }

    virtual void OnBreak(
        Material const * material,
        bool isUnderwater,
        unsigned int size) override
    {
        mBreakEvents[std::make_tuple(material, isUnderwater)] += size;
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
                sink->OnDestroy(std::get<0>(entry.first), std::get<1>(entry.first), entry.second);
            }

            if (mDrawEvent)
            {
                sink->OnDraw();
            }

            for (auto const & entry : mStressEvents)
            {
                sink->OnStress(std::get<0>(entry.first), std::get<1>(entry.first), entry.second);
            }

            for (auto const & entry : mBreakEvents)
            {
                sink->OnBreak(std::get<0>(entry.first), std::get<1>(entry.first), entry.second);
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
    unordered_tuple_map<std::tuple<Material const *, bool>, unsigned int> mDestroyEvents;
    bool mDrawEvent;
    unordered_tuple_map<std::tuple<Material const *, bool>, unsigned int> mStressEvents;
    unordered_tuple_map<std::tuple<Material const *, bool>, unsigned int> mBreakEvents;
    std::vector<unsigned int> mSinkingBeginEvents;

    // The registered sinks
    std::vector<IGameEventHandler *> mSinks;
};
