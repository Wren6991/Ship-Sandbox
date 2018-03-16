/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include <cassert>

namespace Physics {

//   SSS    PPPP     RRRR     IIIIIII  N     N    GGGGG
// SS   SS  P   PP   R   RR      I     NN    N   GG
// S        P    PP  R    RR     I     N N   N  GG
// SS       P   PP   R   RR      I     N N   N  G
//   SSS    PPPP     RRRR        I     N  N  N  G
//      SS  P        R RR        I     N   N N  G  GGGG
//       S  P        R   R       I     N   N N  GG    G
// SS   SS  P        R    R      I     N    NN   GG  GG
//   SSS    P        R     R  IIIIIII  N     N    GGGG

Spring::Spring(
	Ship * parentShip,
	Point * a,
	Point * b,
	float restLength,
	Material const *material)
	: ShipElement(parentShip)
	, mPointA(a)
	, mPointB(b)
	, mRestLength(restLength)
	, mMaterial(material)
    , mIsStressed(false)
{
    // Add ourselves to our endpoints
    a->AddConnectedSpring(this);
    b->AddConnectedSpring(this);
}

void Spring::Destroy(Point const * pointSource)
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

    // Used to do more complicated checks, but easier (and better) to make everything leak when it breaks

    // Make endpoints leak and destroy their triangles
    // Note: technically, should only destroy those triangles that contain the A-B side, and definitely
    // make both A and B leak
    if (mPointA != pointSource)
        mPointA->Breach();
    if (mPointB != pointSource)
        mPointB->Breach();

    // Remove ourselves from our endpoints
    if (mPointA != pointSource)
        mPointA->RemoveConnectedSpring(this);
    if (mPointB != pointSource)
        mPointB->RemoveConnectedSpring(this);

    // Remove ourselves
    ShipElement::Destroy();
}

}