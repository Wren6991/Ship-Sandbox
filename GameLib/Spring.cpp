/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Physics.h"

#include "RenderUtils.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#include <GL/gl.h>

#include <algorithm>
#include <cassert>
// TODO
//#include <cmath>

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

Spring::Spring(World *_parent, Point *_a, Point *_b, Material const *_mtl, double _length)
{
	wld = _parent;
	_parent->springs.push_back(this);
	a = _a;
	b = _b;
	if (_length == -1)
		length = (a->mPosition - b->mPosition).length();
	else
		length = _length;
	mtl = _mtl;
}

Spring::~Spring()
{
	// Used to do more complicated checks, but easier (and better) to make everything leak when it breaks
	a->Breach();
	b->Breach();
	// Scour out any references to this spring
	for (unsigned int i = 0; i < wld->ships.size(); i++)
	{
		Ship *shp = wld->ships[i].get();
		if (shp->adjacentnodes.find(a) != shp->adjacentnodes.end())
			shp->adjacentnodes[a].erase(b);
		if (shp->adjacentnodes.find(b) != shp->adjacentnodes.end())
			shp->adjacentnodes[b].erase(a);
	}
	std::vector <Spring*>::iterator iter = std::find(wld->springs.begin(), wld->springs.end(), this);
	if (iter != wld->springs.end())
		wld->springs.erase(iter);
}

void Spring::update()
{
	// Try to space the two points by the equilibrium length (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
	vec2f correction_dir = (b->mPosition - a->mPosition);
	float currentlength = correction_dir.length();
	correction_dir *= (length - currentlength) / (length * (a->mMaterial->Mass + b->mMaterial->Mass) * 0.85); // * 0.8 => 25% overcorrection (stiffer, converges faster)
	a->mPosition -= correction_dir * b->mMaterial->Mass;    // if b is heavier, a moves more.
	b->mPosition += correction_dir * a->mMaterial->Mass;    // (and vice versa...)
}

void Spring::damping(float amount)
{
	vec2f springdir = (a->mPosition - b->mPosition).normalise();
	springdir *= (a->mPosition - a->mLastPosition - (b->mPosition - b->mLastPosition)).dot(springdir) * amount;   // relative velocity � spring direction = projected velocity, amount = amount of projected velocity that remains after damping
	a->mLastPosition += springdir;
	b->mLastPosition -= springdir;
}

void Spring::render(bool showStress) const
{
	// If member is heavily stressed, highlight it in red (ignored if world's showstress field is false)
	glBegin(GL_LINES);
	if (showStress)
		glColor3f(1, 0, 0);
	else
		RenderUtils::SetColour(a->GetColour(mtl->Colour));
	glVertex3f(a->mPosition.x, a->mPosition.y, -1);
	if (!showStress)
		RenderUtils::SetColour(b->GetColour(mtl->Colour));
	glVertex3f(b->mPosition.x, b->mPosition.y, -1);
	glEnd();
}

bool Spring::isStressed()
{
	// Check whether strain is more than the word's base strength * this object's relative strength
	return (a->mPosition - b->mPosition).length() / this->length > 1 + (wld->strength * mtl->Strength) * 0.25;
}

bool Spring::isBroken()
{
	// Check whether strain is more than the word's base strength * this object's relative strength
	return (a->mPosition - b->mPosition).length() / this->length > 1 + (wld->strength * mtl->Strength);
}

}