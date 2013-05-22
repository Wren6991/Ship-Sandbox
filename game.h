#ifndef _GAME_H_
#define _GAME_H_

#include "phys.h"



class game
{
public:

    struct
    {
        bool ldown;
        bool rdown;
        int x, y;
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

    bool running;

    float zoomsize;
    float camx, camy;
    int canvaswidth, canvasheight;
    void assertSettings();
    vec2 screen2world(vec2);

    phys::world *wld;
    game();
    void render();
    void update();
};







#endif // _GAME_H_
