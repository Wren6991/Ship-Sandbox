#include "render.h"

#include<GL/gl.h>

void render::triangle(vec2 a, vec2 b, vec2 c)
{
    glBegin(GL_TRIANGLES);
    glVertex3f(a.x, a.y, -1);
    glVertex3f(b.x, b.y, -1);
    glVertex3f(c.x, c.y, -1);
    glEnd();
}

void render::triangle(vec2 posa, vec2 posb, vec2 posc, vec3f cola, vec3f colb, vec3f colc)
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

void render::setColour(vec3f c)
{
    glColor3f(c.x, c.y, c.z);
}
