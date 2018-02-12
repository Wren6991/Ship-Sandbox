/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-11
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

namespace Physics {

Cable::Cable(
    Ship * parentShip,
    Point * point)
    : ElectricalElement(        
        parentShip,
        point,
        ElectricalElement::Type::Cable)
{
}

}