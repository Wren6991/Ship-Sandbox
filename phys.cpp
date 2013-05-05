#include "phys.h"

#include <GL/gl.h>
#include <cmath>
#include <iostream>

// W     W    OOO    RRRR     L        DDDD
// W     W   O   O   R   RR   L        D  DDD
// W     W  O     O  R    RR  L        D    DD
// W     W  O     O  R   RR   L        D     D
// W     W  O     O  RRRR     L        D     D
// W  W  W  O     O  R RR     L        D     D
// W  W  W  O     O  R   R    L        D    DD
//  W W W    O   O   R    R   L        D  DDD
//   W W      OOO    R     R  LLLLLLL  DDDD

void phys::world::update(double dt)
{
    //std::cout << " * Updating world\n";
    time += dt;
    for (unsigned int i = 0; i < points.size(); i++)
        points[i]->update(dt);
    for (int iteration = 0; iteration < 15; iteration++)
        for (unsigned int i = 0; i < springs.size(); i++)
            springs[i]->update();
    for (std::vector<spring*>::iterator iter = springs.begin(); iter != springs.end();)
    {
        spring *spr = *iter;
        if (spr->isBroken())
        {
            springs.erase(iter++);
            delete spr;
        }
        else
            iter++;
    }
    for (unsigned int i = 0; i < ships.size(); i++)
        ships[i]->update(dt);
}

void phys::world::render(double left, double right, double bottom, double top)
{
    glColor4f(0.5, 0.5, 0.5, 1);
    glBegin(GL_QUADS);
    glVertex3f(left, -seadepth, -1);
    glVertex3f(right, -seadepth, -1);
    glVertex3f(right, bottom, -1);
    glVertex3f(left, bottom, -1);
    glEnd();
    for (unsigned int i = 0; i < points.size(); i++)
        points[i]->render();
    for (unsigned int i = 0; i < springs.size(); i++)
        springs[i]->render();
    glColor4f(0, 0.25, 1, 0.5);
    double slicewidth = (right - left) / 50.0;
    for (double slicex = left; slicex < right; slicex += slicewidth)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(slicex, waterheight(slicex), -1);
        glVertex3f(slicex + slicewidth, waterheight(slicex + slicewidth), -1);
        glVertex3f(slicex, bottom, -1);
        glVertex3f(slicex + slicewidth, bottom, -1);
        glEnd();
    }

}

double phys::world::waterheight(double x)
{
    return (sin(x * 0.1 + time) * 0.5 + sin(x * 0.3 - time * 1.1) * 0.3) * waveheight;
}

void phys::world::destroyAt(vec2 pos)
{
    for (std::vector<point*>::iterator iter = points.begin(); iter != points.end();)
    {
        if (((*iter)->pos - pos).length() < 0.5)
        {
            point *p = *iter;
            points.erase(iter++);
            delete p;           // have to remove reference before deleting, else other cleanup code will use bad memory!
        }
        else
            iter++;
    }
}

phys::world::world(vec2 _gravity, double _buoyancy, double _strength)
{
    time = 0;
    gravity = _gravity;
    buoyancy = _buoyancy;
    strength = _strength;
    waterpressure = 0.5;
    waveheight = 1.0;
    seadepth = 150;
}

phys::world::~world()
{
    // DESTROY THE WORLD??? Y/N
    for (unsigned int i = 0; i < springs.size(); i++)
        delete springs[i];
    springs.clear();
    for (unsigned int i = 0; i < points.size(); i++)
        delete points[i];
    for (unsigned int i = 0; i < ships.size(); i++)
        delete ships[i];
}

// PPPP       OOO    IIIIIII  N     N  TTTTTTT
// P   PP    O   O      I     NN    N     T
// P    PP  O     O     I     N N   N     T
// P   PP   O     O     I     N N   N     T
// PPPP     O     O     I     N  N  N     T
// P        O     O     I     N   N N     T
// P        O     O     I     N   N N     T
// P         O   O      I     N    NN     T
// P          OOO    IIIIIII  N     N     T

phys::point::point(world *_parent, vec2 _pos, double _mass, double _buoyancy)
{
    parent = _parent;
    parent->points.push_back(this);
    pos = _pos;
    lastpos = pos;
    mass = _mass;
    buoyancy = _buoyancy;
    isLeaking = false;
    water = 0;
}

void phys::point::applyForce(vec2 f)
{
    force += f;
}

void phys::point::update(double dt)
{
    this->applyForce(parent->gravity * (mass * (1 + water * parent->buoyancy * buoyancy)));
    // Buoyancy:
    if (pos.y < parent->waterheight(pos.x))
        this->applyForce(parent->gravity * (-parent->buoyancy * buoyancy * mass));
    vec2 newlastpos = pos;
    // Apply verlet integration:
    pos += (pos - lastpos) + force * (dt * dt / mass);
    // Water drag:
    if (pos.y < parent->waterheight(pos.x))
        pos += (lastpos - pos) * (1 - pow(0.7, dt));
    if (pos.y < -parent->seadepth)
        pos.y = -parent->seadepth;
    lastpos = newlastpos;
    force = vec2(0, 0);
}

vec2 phys::point::getPos()
{
    return pos;
}

void phys::point::setColor(bool isHull, double strength)
{
   if (isHull)
        glColor3f(1.2 - strength * 0.6, 0.6 - strength * 0.3, 0.1);
    else
        glColor3f(0.8 - water + strength * 0.1, 0.7 - water + strength * 0.15, strength * 0.8);
}

void phys::point::render()
{
    if (isLeaking)
    {
        glColor3f(0, 0, 1);
        glBegin(GL_POINTS);
        glVertex3f(pos.x, pos.y, -1);
        glEnd();
    }
}

phys::point::~point()
{
    //remove any springs attached to this point:
    for (std::vector<spring*>::iterator iter = parent->springs.begin(); iter != parent->springs.end();)
    {
        spring *spr = *iter;
        if (spr->a == this || spr->b == this)
        {
            parent->springs.erase(iter++);
            delete spr;
        }
        else
            iter++;
    }
    // remove any references held by ships:
    for (unsigned int i = 0; i < parent->ships.size(); i++)
        parent->ships[i]->points.erase(this);
}

//   SSS    PPPP     RRRR     IIIIIII  N     N    GGGGG
// SS   SS  P   PP   R   RR      I     NN    N   GG
// S        P    PP  R    RR     I     N N   N  GG
// SS       P   PP   R   RR      I     N N   N  G
//   SSS    PPPP     RRRR        I     N  N  N  G
//      SS  P        R RR        I     N   N N  G  GGGG
//       S  P        R   R       I     N   N N  GG    G
// SS   SS  P        R    R      I     N    NN   GG  GG
//   SSS    P        R     R  IIIIIII  N     N    GGGG

phys::spring::spring(world *_parent, point *_a, point *_b, bool _isHull, double _length, double _strength)
{
    parent = _parent;
    _parent->springs.push_back(this);
    a = _a;
    b = _b;
    if (_length == -1)
        length = (a->pos - b->pos).length();
    else
        length = _length;
    strength = _strength;
    isHull = _isHull;
}

phys::spring::~spring()
{
    if (isHull)
    {
        a->isLeaking = true;
        b->isLeaking = true;
    }
    if (true || a->water > 0.5 || b->water > 0.5)
    {
        a->isLeaking = true;
        b->isLeaking = true;
    }
    for (unsigned int i = 0; i < parent->ships.size(); i++)
    {
        ship *shp = parent->ships[i];
        if (shp->adjacentnodes.find(a) != shp->adjacentnodes.end())
            shp->adjacentnodes[a].erase(b);
        if (shp->adjacentnodes.find(b) != shp->adjacentnodes.end())
            shp->adjacentnodes[b].erase(a);
    }
}

void phys::spring::update()
{
    double correction_size = length - (a->pos - b->pos).length();
    vec2 correction_dir = (b->pos - a->pos).normalise();
    double total_mass = a->mass + b->mass;
    a->pos -= correction_dir * (b->mass / total_mass * correction_size);    // if b is heavier, a moves more.
    b->pos += correction_dir * (a->mass / total_mass * correction_size);
}

void phys::spring::render()
{
    bool isStressed = parent->showstress && (a->pos - b->pos).length() / this->length > 1 + (parent->strength * strength) * 0.15;
    glBegin(GL_LINES);
    if (isStressed)
        glColor3f(1, 0, 0);
    else
        a->setColor(isHull, strength);
    glVertex3f(a->pos.x, a->pos.y, -1);
    if (!isStressed)
        b->setColor(isHull, strength);
    glVertex3f(b->pos.x, b->pos.y, -1);
    glEnd();
}

bool phys::spring::isBroken()
{
    return (a->pos - b->pos).length() / this->length > 1 + (parent->strength * strength);
}


//   SSS    H     H  IIIIIII  PPPP
// SS   SS  H     H     I     P   PP
// S        H     H     I     P    PP
// SS       H     H     I     P   PP
//   SSS    HHHHHHH     I     PPPP
//      SS  H     H     I     P
//       S  H     H     I     P
// SS   SS  H     H     I     P
//   SSS    H     H  IIIIIII  P

phys::ship::ship(world *_parent)
{
    parent = _parent;
    parent->ships.push_back(this);
}

void phys::ship::update(double dt)
{
    leakWater(dt);
    gravitateWater(dt);
    balancePressure(dt);
}

void phys::ship::leakWater(double dt)
{
   for (std::set<point*>::iterator iter = points.begin(); iter != points.end(); iter++)
   {
        point *p = *iter;
        if (p->isLeaking && p->pos.y < parent->waterheight(p->pos.x) && p->water < 1.5)
        {
            p->water += dt * parent->waterpressure;
        }
   }
}

void phys::ship::gravitateWater(double dt)
{
    for (std::map<point*, std::set<point*> >::iterator iter = adjacentnodes.begin();
         iter != adjacentnodes.end(); iter++)
    {
        point *a = iter->first;
        for (std::set<point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
        {
            point *b = *second;
            double cos_theta = (b->pos - a->pos).normalise().dot(parent->gravity.normalise());
            if (cos_theta > 0)
            {
                double correction = std::min(0.5 * cos_theta * dt, a->water);
                a->water -= correction;
                b->water += correction;
            }
        }
    }

}

void phys::ship::balancePressure(double dt)
{
    for (std::map<point*, std::set<point*> >::iterator iter = adjacentnodes.begin();
         iter != adjacentnodes.end(); iter++)
    {
        point *a = iter->first;
        if (a->water < 1)   // if water content is not above threshold, no need to force water out
            continue;
        for (std::set<point*>::iterator second = iter->second.begin(); second != iter->second.end(); second++)
        {
            point *b = *second;
            double correction = (b->water - a->water) * 8 * dt; // can tune this number; value of 1 means will equalise in 1 second.
            a->water += correction;
            b->water -= correction;
        }
    }
}


