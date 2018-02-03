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
	World * parentWorld,
	Point * a,
	Point * b,
	float length,
	Material const *material)
	: mParentWorld(parentWorld)
	, mPointA(a)
	, mPointB(b)
	, mLength(length)
	, mMaterial(material)
{
	// TODO: NUKE and make this inline
	mParentWorld->mSprings.push_back(this);
}

Spring::~Spring()
{
	// Used to do more complicated checks, but easier (and better) to make everything leak when it breaks
	mPointA->Breach();
	mPointB->Breach();

	// Scour out any references to this spring
	for (unsigned int i = 0; i < mParentWorld->mShips.size(); i++)
	{
		Ship *shp = mParentWorld->mShips[i].get();
		if (shp->mAdjacentnodes.find(mPointA) != shp->mAdjacentnodes.end())
			shp->mAdjacentnodes[mPointA].erase(mPointB);
		if (shp->mAdjacentnodes.find(mPointB) != shp->mAdjacentnodes.end())
			shp->mAdjacentnodes[mPointB].erase(mPointA);
	}
	std::vector <Spring*>::iterator iter = std::find(mParentWorld->mSprings.begin(), mParentWorld->mSprings.end(), this);
	if (iter != mParentWorld->mSprings.end())
		mParentWorld->mSprings.erase(iter);
}

void Spring::Update()
{
	// Try to space the two points by the equilibrium length (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
	vec2f correction_dir = (mPointB->GetPosition() - mPointA->GetPosition());
	float currentlength = correction_dir.length();
	correction_dir *= (mLength - currentlength) / (mLength * (mPointA->GetMass() + mPointB->GetMass()) * 0.85f); // * 0.8 => 25% overcorrection (stiffer, converges faster)
	mPointA->GetPosition() -= correction_dir * mPointB->GetMass();    // if mPointB is heavier, mPointA moves more.
	mPointB->GetPosition() += correction_dir * mPointA->GetMass();    // (and vice versa...)
}

void Spring::DoDamping(float amount)
{
	vec2f springdir = (mPointA->GetPosition() - mPointB->GetPosition()).normalise();
	springdir *= ((mPointA->GetPosition() - mPointA->GetLastPosition()) - (mPointB->GetPosition() - mPointB->GetLastPosition())).dot(springdir) * amount;   // relative velocity � spring direction = projected velocity, amount = amount of projected velocity that remains after damping
	mPointA->GetLastPosition() += springdir;
	mPointB->GetLastPosition() -= springdir;
}

}