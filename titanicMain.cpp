/***************************************************************
 * Name:      titanicMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Luke Wren (wren6991@gmail.com)
 * Created:   2013-04-30
 * Copyright: Luke Wren (http://github.com/Wren6991)
 * License:
 **************************************************************/

#include "titanicMain.h"
#include <map>
#include <wx/msgdlg.h>
#include "phys.h"

//(*InternalHeaders(titanicFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)


//helper functions

enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    return "Ship Sandbox Alpha v1.2\n(c) Luke Wren 2013\nLicensed to Francis Racicot";
}

//(*IdInit(titanicFrame)
const long titanicFrame::ID_GLCANVAS1 = wxNewId();
const long titanicFrame::ID_MENUITEM1 = wxNewId();
const long titanicFrame::idMenuQuit = wxNewId();
const long titanicFrame::ID_MENUITEM3 = wxNewId();
const long titanicFrame::ID_MENUITEM4 = wxNewId();
const long titanicFrame::mnuShow = wxNewId();
const long titanicFrame::ID_MENUITEM2 = wxNewId();
const long titanicFrame::idMenuAbout = wxNewId();
const long titanicFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(titanicFrame,wxFrame)
    //(*EventTable(titanicFrame)
    //*)
END_EVENT_TABLE()

titanicFrame::titanicFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(titanicFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, _("Ship Sandbox Alpha 1.2"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    int GLCanvasAttributes_1[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLCanvas1 = new wxGLCanvas(this, ID_GLCANVAS1, wxDefaultPosition, wxSize(640,480), 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    BoxSizer1->Add(GLCanvas1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("Load Ship\tCtrl+O"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu4 = new wxMenu();
    MenuItemSmash = new wxMenuItem(Menu4, ID_MENUITEM3, _("Smash"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItemSmash);
    MenuItemGrab = new wxMenuItem(Menu4, ID_MENUITEM4, _("Grab"), wxEmptyString, wxITEM_RADIO);
    Menu4->Append(MenuItemGrab);
    MenuBar1->Append(Menu4, _("Tools"));
    Menu3 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu3, mnuShow, _("Show\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(Menu3, ID_MENUITEM2, _("Play/Pause\tCtrl+P"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuBar1->Append(Menu3, _("Options"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(20, false);
    dlgOpen = new wxFileDialog(this, _("Select Ship Image"), wxEmptyString, wxEmptyString, _("(*.png)|*.png"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1LeftDown,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1LeftUp,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1RightDown,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1RightUp,0,this);
    GLCanvas1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1MouseMove,0,this);
    GLCanvas1->Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1MouseWheel,0,this);
    GLCanvas1->Connect(wxEVT_SIZE,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1Resize,0,this);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemLoadSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnQuit);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemSmashSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemGrabSelected);
    Connect(mnuShow,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemOptionsSelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemPlayPauseSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&titanicFrame::OnTimer1Trigger);
    //*)

    settings = new settingsDialog(this);

    GLContext1 = new wxGLContext(GLCanvas1);

    gm.loadShip("ship.png");

    gm.zoomsize = 30;
    gm.running = true;

    gm.camx = 0;
    gm.camy = 0;

    Timer1.Start();
    gm.canvaswidth = 200;
    gm.canvasheight = 200;
}

titanicFrame::~titanicFrame()
{
    //(*Destroy(titanicFrame)
    //*)
}

void titanicFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void titanicFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
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




//   GGGGG  RRRR        A     PPPP     H     H  IIIIIII    CCC      SSS
//  GG      R   RR     A A    P   PP   H     H     I      CC CC   SS   SS
// GG       R    RR   A   A   P    PP  H     H     I     CC    C  S
// G        R   RR   A     A  P   PP   H     H     I     C        SS
// G        RRRR     AAAAAAA  PPPP     HHHHHHH     I     C          SSS
// G  GGGG  R RR     A     A  P        H     H     I     C             SS
// GG    G  R   R    A     A  P        H     H     I     CC    C        S
//  GG  GG  R    R   A     A  P        H     H     I      CC CC   SS   SS
//   GGGG   R     R  A     A  P        H     H  IIIIIII    CCC      SSS

void titanicFrame::initgl()
{
    // Set the context, clear the gm.canvas and set up all the matrices.
    GLContext1->SetCurrent(*GLCanvas1);

    glViewport(0, 0, gm.canvaswidth, gm.canvasheight);
    glClearColor(0.529, 0.808, 0.980, 1); //(cornflower blue)
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float halfheight = gm.zoomsize;
    float halfwidth = (float)gm.canvaswidth / gm.canvasheight * halfheight;
    glFrustum(-halfwidth, halfwidth, -halfheight, halfheight, 1, 1000);
    glTranslatef(-gm.camx, -gm.camy, 0);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(90.f / gm.zoomsize);
    glLineWidth(80.f / gm.zoomsize);
    glColor3f(0, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void titanicFrame::endgl()
{
    // Flush all the draw operations and flip the back buffer onto the screen.
    glFlush();
    GLCanvas1->SwapBuffers();
    GLCanvas1->Refresh();
}

void titanicFrame::OnGLCanvas1Resize(wxSizeEvent& event)
{
    gm.canvaswidth = event.GetSize().GetX();
    gm.canvasheight = event.GetSize().GetY();
    GLCanvas1->Refresh();
}



void titanicFrame::OnTimer1Trigger(wxTimerEvent& event)
{
    // Main timing event!
    initgl();
    gm.update();
    gm.render();
    endgl();
}

void titanicFrame::OnMenuItemLoadSelected(wxCommandEvent& event)
{
    if (dlgOpen->ShowModal() == wxID_OK)
    {
        std::string filename = dlgOpen->GetPath().ToStdString();
        delete gm.wld;
        gm.wld = new phys::world;
        gm.assertSettings();
        gm.loadShip(filename);
    }
}

// M     M    OOO    U     U    SSS    EEEEEEE
// MM   MM   O   O   U     U  SS   SS  E
// M M M M  O     O  U     U  S        E
// M  M  M  O     O  U     U  SS       E
// M     M  O     O  U     U    SSS    EEEE
// M     M  O     O  U     U       SS  E
// M     M  O     O  U     U        S  E
// M     M   O   O    U   U   SS   SS  E
// M     M    OOO      UUU      SSS    EEEEEEE

void titanicFrame::OnGLCanvas1LeftDown(wxMouseEvent& event)
{
    gm.mouse.ldown = true;
}

void titanicFrame::OnGLCanvas1LeftUp(wxMouseEvent& event)
{
    gm.mouse.ldown = false;
}

void titanicFrame::OnGLCanvas1RightDown(wxMouseEvent& event)
{
    gm.mouse.rdown = true;
}

void titanicFrame::OnGLCanvas1RightUp(wxMouseEvent& event)
{
    gm.mouse.rdown = false;
}

void titanicFrame::OnGLCanvas1MouseMove(wxMouseEvent& event)
{
    int oldx = gm.mouse.x;
    int oldy = gm.mouse.y;
    gm.mouse.x = event.GetX();
    gm.mouse.y = event.GetY();
    if (gm.mouse.rdown)
    {
        vec2 difference = gm.screen2world(vec2(gm.mouse.x, gm.mouse.y)) - gm.screen2world(vec2(oldx, oldy));
        gm.camx -= difference.x;
        gm.camy -= difference.y;
    }
}

void titanicFrame::OnMenuItemOptionsSelected(wxCommandEvent& event)
{
    settings->Show();
}

void titanicFrame::OnGLCanvas1MouseWheel(wxMouseEvent& event)
{
    gm.zoomsize *= pow(0.998, event.GetWheelRotation());
}

void titanicFrame::OnMenuItemPlayPauseSelected(wxCommandEvent& event)
{
    gm.running = !gm.running;
}

void titanicFrame::OnMenuItemSmashSelected(wxCommandEvent& event)
{
    gm.tool = game::TOOL_SMASH;
}

void titanicFrame::OnMenuItemGrabSelected(wxCommandEvent& event)
{
    gm.tool = game::TOOL_GRAB;
}
