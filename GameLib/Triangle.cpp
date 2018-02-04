/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

namespace Physics {

Triangle::Triangle(
    Ship * parentShip,
    Point * a,
    Point * b,
    Point * c)
    : ShipElement(parentShip)
    , mPointA(a)
    , mPointB(b)
    , mPointC(c)
{
    // Add ourselves to each point
    a->AddConnectedTriangle(this);
    b->AddConnectedTriangle(this);
    c->AddConnectedTriangle(this);
}

void Triangle::Destroy()
{
    // Remove ourselves from each point
	mPointA->RemoveConnectedTriangle(this);
	mPointB->RemoveConnectedTriangle(this);
	mPointC->RemoveConnectedTriangle(this);

    // Remove ourselves from ship
    ShipElement::Destroy();

    // Delete ourselves
    delete this;
}

}