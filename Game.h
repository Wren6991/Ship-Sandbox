#ifndef _GAME_H_
#define _GAME_H_

#include <vector>
#include "phys.h"


class game
{
    std::vector <material*> materials;
public:

    struct MouseInfo
    {
        bool ldown;
        bool rdown;
		int x;
		int y;
		int lastx;
		int lasty;

		MouseInfo()
			: ldown(false)
			, rdown(false)
			, x(0)
			, y(0)
			, lastx(0)
			, lasty(0)
		{
		}
    } mouse;

    enum tool_type {
        TOOL_SMASH,
        TOOL_GRAB
    } tool;

    double strength;
    double buoyancy;
    double waveheight;
    double waterpressure;
    double seadepth;
    bool showstress;
    bool quickwaterfix;
    bool xraymode;

    bool running;

    float zoomsize;
    float camx, camy;
    int canvaswidth, canvasheight;
    void loadShip(std::wstring filename);
    void loadDepth(std::wstring filename);
    void assertSettings();
    vec2 screen2world(vec2);

    std::wstring lastFilename;
    float *oceandepthbuffer;

    phys::world *wld;
    game();
    void render();
    void update();
};







#endif // _GAME_H_
