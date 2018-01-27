#ifndef _PHYS_H_INCLUDED_
#define _PHYS_H_INCLUDED_

#include "Scheduler.h"

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "material.h"
#include "Vectors.h"

namespace phys
{
	class point; class spring; class ship; class game; struct AABB; struct BVHNode;
	class world
	{
		friend class point;
		friend class spring;
		friend class ship;
		struct springCalculateTask;
		struct pointIntegrateTask;
		Scheduler springScheduler;
		std::vector <point*> points;
		std::vector <spring*> springs;
		std::vector <ship*> ships;
		BVHNode *collisionTree;
		float waterheight(float x);
		float oceanfloorheight(float x);
		void doSprings(double dt);
		vec2 const mGravity;
		vec2 const mGravityNormal;
		float const mGravityLength;
		void buildBVHTree(bool splitInX, std::vector<point*> &pointlist, BVHNode *thisnode, int depth = 1);
	public:
		float const *oceandepthbuffer;
		float buoyancy;
		float strength;
		float waterpressure;
		float waveheight;
		float seadepth;
		bool showstress;
		bool quickwaterfix;
		bool xraymode;
		float time;
		void update(double dt);
		void render(double left, double right, double bottom, double top);
		void renderLand(double left, double right, double bottom, double top);
		void renderWater(double left, double right, double bottom, double top);
		void destroyAt(vec2 pos, float radius);
		void drawTo(vec2 target);
		world(vec2 gravity = vec2(0, -9.8), double _buoyancy = 4, double _strength = 0.01);
		~world();
	};

	struct world::springCalculateTask : Scheduler::ITask
	{
		springCalculateTask(world *_wld, int _first, int _last);
		world *wld;
		int first, last;
		virtual void Process();
	};

	struct world::pointIntegrateTask : Scheduler::ITask
	{
		pointIntegrateTask(world *_wld, int _first, int _last, float _dt);
		world *wld;
		float dt;
		int first, last;
		virtual void Process();
	};


	class ship
	{
	public:
		world *wld;
		struct triangle {
			ship *parent;
			point *a, *b, *c;
			triangle(phys::ship *_parent, point *_a, point *_b, point *_c);
			~triangle();
		};
		std::set<point*> points;
		std::set<spring*> springs;
		std::map<point*, std::set<point*> > adjacentnodes;
		std::set<triangle*> triangles;
		void render();
		void leakWater(double dt);
		void gravitateWater(double dt);
		void balancePressure(double dt);

		ship(world *_parent);
		~ship();
		void update(double dt);
	};

	class point
	{
		world *wld;
		friend class spring;
		friend class world;
		friend class ship;
		static constexpr float radius = 0.4f;
		vec2 pos;
		vec2 lastpos;
		vec2 force;
		double buoyancy;
		double water;
		double getPressure();
	public:
		std::set<ship::triangle*> tris;
		material *mtl;
		bool isLeaking;
		point(world *_parent, vec2 _pos, material *_mtl, double _buoyancy);
		~point();
		void applyForce(vec2 f);
		void breach();  // set to leaking and remove any incident triangles
		void update(double dt);
		vec2 getPos();
		vec3f getColour(vec3f basecolour);
		AABB getAABB();
		void render();
	};

	class spring
	{
		friend class world;
		friend class point;
		friend class ship;
		world *wld;
		point *a, *b;
		double length;
		material *mtl;
	public:
		spring(world *_parent, point *_a, point *_b, material *_mtl, double _length = -1);
		~spring();
		void update();
		void damping(float amount);
		void render(bool isStressed = false);
		bool isStressed();
		bool isBroken();
	};

	struct AABB
	{
		vec2 bottomleft, topright;
		AABB() {}
		AABB(vec2 _bottomleft, vec2 _topright);
		void extendTo(AABB other);
		void render();
	};

	struct BVHNode
	{
		AABB volume;
		BVHNode *l, *r;
		bool isLeaf;
		int pointCount;
		static const int MAX_DEPTH = 15;
		static const int MAX_N_POINTS = 10;
		point* points[MAX_N_POINTS];
		static BVHNode *allocateTree(int depth = MAX_DEPTH);
	};
}


#endif // _PHYS_H_INCLUDED_