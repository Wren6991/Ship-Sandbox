/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-11
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

namespace Physics {

ElectricalElement::ElectricalElement(    
    Ship * parentShip,
    Point * point,
    Type type)
    : ShipElement(parentShip)
    , mPoint(point)
    , mType(type)
    , mLastGraphVisitStepSequenceNumber(0u)
{
    // Add ourselves to the point
    assert(nullptr == point->GetConnectedElectricalElement());
    point->SetConnectedElectricalElement(this);
}

void ElectricalElement::Destroy()
{
    // Remove ourselves
    ShipElement::Destroy();
}

}