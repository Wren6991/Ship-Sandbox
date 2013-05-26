#ifndef _GAME_H_
#define _GAME_H_

#include <vector>
#include <wx/image.h>
#include "phys.h"


class game
{
    std::vector <material*> materials;
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
    bool quickwaterfix;
    bool xraymode;

    bool running;

    float zoomsize;
    float camx, camy;
    int canvaswidth, canvasheight;
    void loadShip(std::string filename);
    void loadDepth(std::string filename);
    void assertSettings();
    vec2 screen2world(vec2);

    std::string lastFilename;
    float *oceandepthbuffer;

    phys::world *wld;
    game();
    void render();
    void update();
};







#endif // _GAME_H_
