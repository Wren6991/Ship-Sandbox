#ifndef _RENDER_H_
#define _RENDER_H_

#include "vec.h"

namespace render
{
    void triangle(vec2 a, vec2 b, vec2 c);
    void triangle(vec2 posa, vec2 posb, vec2 posc, vec3f cola, vec3f colb, vec3f colc);
    void box(vec2 bottomleft, vec2 topright);
    void setColour(vec3f c);
}

#endif // _RENDER_H_
