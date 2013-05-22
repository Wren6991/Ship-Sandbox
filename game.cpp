#include "game.h"

vec2 game::screen2world(vec2 pos)
{
    float height = zoomsize * 2.0;
    float width = (float)canvaswidth / canvasheight * height;
    return vec2((pos.x / canvaswidth - 0.5) * width + camx,
                (pos.y / canvasheight - 0.5) * -height + camy);
}

void game::assertSettings()
{
    wld->buoyancy = buoyancy;
    wld->strength = strength;
    wld->waterpressure = waterpressure;
    wld->waveheight = waveheight;
    wld->seadepth = seadepth;
    wld->showstress = showstress;
}

void game::update()
{
    if (mouse.ldown)
    {
        if (tool == TOOL_SMASH)
            wld->destroyAt(screen2world(vec2(mouse.x, mouse.y)));
        else if (tool == TOOL_GRAB)
            wld->drawTo(screen2world(vec2(mouse.x, mouse.y)));
    }
    if (running)
        wld->update(0.02);
}

void game::render()
{
    float halfheight = zoomsize;
    float halfwidth = (float)canvaswidth / canvasheight * halfheight;
    wld->render(camx - halfwidth, camx + halfwidth, camy - halfheight, camx + halfheight);
}

game::game()
{
    wld = new phys::world();
    buoyancy = 4.0;
    strength = 0.01;
    waveheight = 1.0;
    waterpressure = 0.5;
    seadepth = 150;
    showstress = false;
    assertSettings();
}
