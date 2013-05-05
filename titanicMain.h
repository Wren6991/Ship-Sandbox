/***************************************************************
 * Name:      titanicMain.h
 * Purpose:   Defines Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#ifndef TITANICMAIN_H
#define TITANICMAIN_H

//(*Headers(titanicFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/glcanvas.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

#include "phys.h"
#include "settingsDialog.h"

class titanicFrame: public wxFrame
{
public:

    titanicFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~titanicFrame();

    double strength;
    double buoyancy;
    double waveheight;
    double waterpressure;
    double seadepth;
    bool showstress;

    void assertSettings();

private:

    //(*Handlers(titanicFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnGLCanvas1Paint(wxPaintEvent& event);
    void OnGLCanvas1Resize(wxSizeEvent& event);
    void OnTimer1Trigger(wxTimerEvent& event);
    void OnMenuItemLoadSelected(wxCommandEvent& event);
    void OnGLCanvas1LeftDown(wxMouseEvent& event);
    void OnGLCanvas1LeftUp(wxMouseEvent& event);
    void OnGLCanvas1RightDown(wxMouseEvent& event);
    void OnGLCanvas1RightUp(wxMouseEvent& event);
    void OnGLCanvas1MouseMove(wxMouseEvent& event);
    void OnMenuItemOptionsSelected(wxCommandEvent& event);
    void OnGLCanvas1MouseWheel(wxMouseEvent& event);
    void OnMenuItemPlayPauseSelected(wxCommandEvent& event);
    //*)

    //(*Identifiers(titanicFrame)
    static const long ID_GLCANVAS1;
    static const long ID_MENUITEM1;
    static const long idMenuQuit;
    static const long mnuShow;
    static const long ID_MENUITEM2;
    static const long idMenuAbout;
    static const long ID_TIMER1;
    //*)

    //(*Declarations(titanicFrame)
    wxMenuItem* MenuItem5;
    wxMenu* Menu3;
    wxMenuItem* MenuItem4;
    wxGLCanvas* GLCanvas1;
    wxFileDialog* dlgOpen;
    wxMenuItem* MenuItem3;
    wxTimer Timer1;
    //*)

    settingsDialog *settings;

    struct
    {
        bool ldown;
        bool rdown;
        int x, y;
    } mouse;

    bool running;

    float zoomsize;
    float camx, camy;
    vec2 screen2world(vec2);

    wxGLContext* GLContext1;
    int canvaswidth, canvasheight;
    phys::world *wld;

    void loadShip(std::string filename);
    void initgl();
    void endgl();


    DECLARE_EVENT_TABLE()
};

#endif // TITANICMAIN_H
