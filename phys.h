#ifndef _PHYS_H_INCLUDED_
#define _PHYS_H_INCLUDED_

#include <map>
#include <set>
#include <vector>
#include "material.h"
#include "vec.h"

namespace phys
{
    class point; class spring; class ship; class game;
    class world
    {
        friend class point;
        friend class spring;
        friend class ship;
        std::vector <point*> points;
        std::vector <spring*> springs;
        std::vector <ship*> ships;
        double waterheight(double x);
        vec2 gravity;
    public:
        double buoyancy;
        double strength;
        double waterpressure;
        double waveheight;
        double seadepth;
        bool showstress;
        double time;
        void update(double dt);
        void render(double left, double right, double bottom, double top);
        void destroyAt(vec2 pos);
        void drawTo(vec2 target);
        world(vec2 _gravity = vec2(0, -9.8), double _buoyancy = 4, double _strength = 0.01);
        ~world();
    };

    class point
    {
        world *parent;
        friend class spring;
        friend class world;
        friend class ship;
        vec2 pos;
        vec2 lastpos;
        vec2 force;
        double buoyancy;
        double water;
        double getPressure();
    public:
        material *mtl;
        bool isLeaking;
        point(world *_parent, vec2 _pos, material *_mtl, double _buoyancy);
        ~point();
        void applyForce(vec2 f);
        void update(double dt);
        vec2 getPos();
        void setColor(vec3f basecolour);
        void render();
    };

    class spring
    {
        friend class world;
        friend class point;
        friend class ship;
        world *parent;
        point *a, *b;
        double length;
        material *mtl;
    public:
        spring(world *_parent, point *_a, point *_b, material *_mtl, double _length = -1);
        ~spring();
        void update();
        void render();
        bool isBroken();
    };

    struct ship
    {
        world *parent;
        std::set<point*> points;
        std::set<spring*> springs;
        std::map<point*, std::set<point*> > adjacentnodes;
        void leakWater(double dt);
        void gravitateWater(double dt);
        void balancePressure(double dt);
    public:
        ship(world *_parent);
        void update(double dt);
    };
}


#endif // _PHYS_H_INCLUDED_
