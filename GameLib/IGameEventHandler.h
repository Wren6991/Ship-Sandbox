/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-05
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Material.h"

/*
 * This interface defines the methods that game event handlers must implement.
 *
 * The methods are default-implemented to facilitate implementation of handlers that 
 * only care about a subset of the events.
 */
class IGameEventHandler
{
public:

    virtual ~IGameEventHandler()
    {
    }

    virtual void OnDestroy(
        Material const * /*material*/, 
        unsigned int /*size*/ )
    {
        // Default-implemented
    }

    virtual void OnStress(
        Material const * /*material*/,
        unsigned int /*size*/)
    {
        // Default-implemented
    }

    virtual void OnBreak(
        Material const * /*material*/,
        unsigned int /*size*/)
    {
        // Default-implemented
    }

    virtual void OnSinkingBegin(unsigned int /*shipId*/)
    {
        // Default-implemented
    }
};
