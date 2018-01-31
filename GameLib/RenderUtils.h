/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "RenderParameters.h"
#include "Vectors.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include<GL/gl.h> 

class RenderUtils 
{
public:

	//
	// Render primitives
	//

	static inline void RenderTriangle(
		vec2 const & a,
		vec2 const & b,
		vec2 const & c)
	{
		glBegin(GL_TRIANGLES);
		glVertex3f(a.x, a.y, -1);
		glVertex3f(b.x, b.y, -1);
		glVertex3f(c.x, c.y, -1);
		glEnd();
	}

	static inline void RenderTriangle(
		vec2 const & posa,
		vec2 const & posb,
		vec2 const & posc,
		vec3f const & cola,
		vec3f const & colb,
		vec3f const & colc)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(cola.x, cola.y, cola.z);
		glVertex3f(posa.x, posa.y, -1);
		glColor3f(colb.x, colb.y, colb.z);
		glVertex3f(posb.x, posb.y, -1);
		glColor3f(colc.x, colc.y, colc.z);
		glVertex3f(posc.x, posc.y, -1);
		glEnd();
	}

	static inline void RenderBox(
		vec2 const & bottomleft,
		vec2 const & topright)
	{
		glBegin(GL_LINE_LOOP);
		glColor4f(1.f, 0.f, 0.f, 0.5f);
		glVertex3f(bottomleft.x, topright.y, -1);
		glVertex3f(topright.x, topright.y, -1);
		glVertex3f(topright.x, bottomleft.y, -1);
		glVertex3f(bottomleft.x, bottomleft.y, -1);
		glEnd();
	}

	static inline void SetColour(vec3f const & c)
	{
		glColor3f(c.x, c.y, c.z);
	}

	//
	// World<->Screen
	//

	static inline float ZoomToWorldY(float zoom)
	{
		return zoom * 2.0f;
	}

	static inline vec2 Screen2World(
		vec2 const & screenCoordinates,
		RenderParameters const & renderParameters)
	{
		float height = ZoomToWorldY(renderParameters.Zoom);
		float width = static_cast<float>(renderParameters.CanvasWidth) / static_cast<float>(renderParameters.CanvasHeight) * height;
		return vec2(
			(screenCoordinates.x / static_cast<float>(renderParameters.CanvasWidth) - 0.5f) * width + renderParameters.CamX,
			(screenCoordinates.y / static_cast<float>(renderParameters.CanvasHeight) - 0.5f) * -height + renderParameters.CamY);
	}

	static inline vec2 ScreenOffset2WorldOffset(
		vec2 const & screenOffset,
		RenderParameters const & renderParameters)
	{
		float height = ZoomToWorldY(renderParameters.Zoom);
		float width = static_cast<float>(renderParameters.CanvasWidth) / static_cast<float>(renderParameters.CanvasHeight) * height;

		return vec2(
			screenOffset.x / static_cast<float>(renderParameters.CanvasWidth) * width,
			screenOffset.y / static_cast<float>(renderParameters.CanvasHeight) * -height);
	}
};
