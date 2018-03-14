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

void Spring::Destroy()
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

	// Used to do more complicated checks, but easier (and better) to make everything leak when it breaks

    // Make endpoints leak and destroy their triangles
    // TODO: technically, should only destroy those triangles that contain the A-B side, and definitely
    // make both A and B leak
	mPointA->Breach();
	mPointB->Breach();

    // Remove ourselves from our endpoints
    mPointA->RemoveConnectedSpring(this);
    mPointB->RemoveConnectedSpring(this);

	// Remove ourselves
	ShipElement::Destroy();
}

void Spring::DestroyFromPoint(Point const * pointSource)
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

void Spring::Relax()
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

    //
	// Try to space the two points by the equilibrium length (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
    //

	vec2f const displacement = (mPointB->GetPosition() - mPointA->GetPosition());
	float const currentlength = displacement.length();

    // ORIGINAL:
	//correction_dir *= (mRestLength - currentlength) / (mRestLength * (mPointA->GetMass() + mPointB->GetMass()) * 0.85f); // * 0.8 => 25% overcorrection (stiffer, converges faster)

    // NEW:
    vec2f correction = displacement.normalise();
    correction *= (mRestLength - currentlength) / ((mPointA->GetMass() + mPointB->GetMass()) * 0.80f); // * 0.8 => 25% overcorrection (stiffer, converges faster)

	mPointA->SubtractFromPosition(correction * mPointB->GetMass());    // if mPointB is heavier, mPointA moves more.
	mPointB->AddToPosition(correction * mPointA->GetMass());    // (and vice versa...)
}

void Spring::Damp(float amount)
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

	vec2f springdir = (mPointA->GetPosition() - mPointB->GetPosition()).normalise();
	springdir *= ((mPointA->GetPosition() - mPointA->GetLastPosition()) - (mPointB->GetPosition() - mPointB->GetLastPosition())).dot(springdir) * amount;   // relative velocity � spring direction = projected velocity, amount = amount of projected velocity that remains after damping
	mPointA->AddToLastPosition(springdir);
	mPointB->SubtractFromLastPosition(springdir);
}

}