/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "Physics.h"

#include <cassert>

namespace Physics
{

/*
 * Base class of all elements owned by a ship.
 * Provides:
 * - Facilities for deleting element from the ship's main containers of element pointers
 * - A pointer to the parent ship
 */
template<typename TElement>
class ShipElement
{
protected:

	ShipElement(Ship * parentShip)
		: mParentShip(parentShip)
		, mIsDeleted(false)
	{
		assert(nullptr != mParentShip);
	}

	/*
	 * Marks this element as deleted and notifies the ship about this.
	 */
	inline void Destroy()
	{
		mIsDeleted = true;
		mParentShip->RegisterDestruction<TElement>(static_cast<TElement *>this);
	}

public:

	inline Ship * GetParentShip() noexcept { return mParentShip; }

	inline bool IsDeleted() const noexcept { return mIsDeleted;  }

private:
	
	Ship * const mParentShip;
	bool mIsDeleted;
};

}
