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
	float length,
	Material const *material)
	: ShipElement(parentShip)
	, mPointA(a)
	, mPointB(b)
	, mLength(length)
	, mMaterial(material)
{
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

	// Scour out any references to this spring
	if (GetParentShip()->mAdjacentNonHullPoints.find(mPointA) != GetParentShip()->mAdjacentNonHullPoints.end())
		GetParentShip()->mAdjacentNonHullPoints[mPointA].erase(mPointB);
	if (GetParentShip()->mAdjacentNonHullPoints.find(mPointB) != GetParentShip()->mAdjacentNonHullPoints.end())
		GetParentShip()->mAdjacentNonHullPoints[mPointB].erase(mPointA);

	// Remove ourselves
	ShipElement::Destroy();
}

void Spring::Update()
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

	// Try to space the two points by the equilibrium length (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
	vec2f correction_dir = (mPointB->GetPosition() - mPointA->GetPosition());
	float currentlength = correction_dir.length();
	correction_dir *= (mLength - currentlength) / (mLength * (mPointA->GetMass() + mPointB->GetMass()) * 0.85f); // * 0.8 => 25% overcorrection (stiffer, converges faster)
	mPointA->GetPosition() -= correction_dir * mPointB->GetMass();    // if mPointB is heavier, mPointA moves more.
	mPointB->GetPosition() += correction_dir * mPointA->GetMass();    // (and vice versa...)
}

void Spring::DoDamping(float amount)
{
    assert(!mPointA->IsDeleted());
    assert(!mPointB->IsDeleted());

	vec2f springdir = (mPointA->GetPosition() - mPointB->GetPosition()).normalise();
	springdir *= ((mPointA->GetPosition() - mPointA->GetLastPosition()) - (mPointB->GetPosition() - mPointB->GetLastPosition())).dot(springdir) * amount;   // relative velocity � spring direction = projected velocity, amount = amount of projected velocity that remains after damping
	mPointA->GetLastPosition() += springdir;
	mPointB->GetLastPosition() -= springdir;
}

}