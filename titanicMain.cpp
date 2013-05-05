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
const int directions[8][2] = {
{ 1,  0},
{ 1, -1},
{ 0, -1},
{-1, -1},
{-1,  0},
{-1,  1},
{ 0,  1},
{ 1,  1}
};

enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(titanicFrame)
const long titanicFrame::ID_GLCANVAS1 = wxNewId();
const long titanicFrame::ID_MENUITEM1 = wxNewId();
const long titanicFrame::idMenuQuit = wxNewId();
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

    Create(parent, id, _("Pac0\'s Ship Sandbox"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
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
    Menu3 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu3, mnuShow, _("Show\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(Menu3, ID_MENUITEM2, _("Play/Pause\tCtrl+P"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuBar1->Append(Menu3, _("Options"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_RADIO);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    Timer1.SetOwner(this, ID_TIMER1);
    Timer1.Start(20, false);
    dlgOpen = new wxFileDialog(this, _("Select Ship Image"), wxEmptyString, wxEmptyString, _("(*.png)|*.png"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    GLCanvas1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1Paint,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1LeftDown,0,this);
    GLCanvas1->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1LeftUp,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1RightDown,0,this);
    GLCanvas1->Connect(wxEVT_RIGHT_UP,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1RightUp,0,this);
    GLCanvas1->Connect(wxEVT_MOTION,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1MouseMove,0,this);
    GLCanvas1->Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1MouseWheel,0,this);
    GLCanvas1->Connect(wxEVT_SIZE,(wxObjectEventFunction)&titanicFrame::OnGLCanvas1Resize,0,this);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemLoadSelected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnQuit);
    Connect(mnuShow,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemOptionsSelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnMenuItemPlayPauseSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&titanicFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&titanicFrame::OnTimer1Trigger);
    //*)

    buoyancy = 4.0;
    strength = 0.01;
    waveheight = 1.0;
    waterpressure = 0.5;
    seadepth = 150;
    showstress = false;

    running = true;

    settings = new settingsDialog(this);

    GLContext1 = new wxGLContext(GLCanvas1);

    wld = new phys::world;

    loadShip("ship.png");

    zoomsize = 30;
    camx = 0;
    camy = 0;

    Timer1.Start();
    canvaswidth = 200;
    canvasheight = 200;
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

void titanicFrame::loadShip(std::string filename)
{
    // RGB channels contain separate information:
    // R: Strength (higher = more)
    // G: Empty or not (white background has high G so is ignored, all materials have low G)
    // B: Hull or not (blue has high G, is not hull; black has high G, is hull)
    // black: weak hull; blue: weak internal; red: strong hull; magenta: strong internal
    // Can vary shades of red for varying strengths and colours

    wxImage shipimage(filename, wxBITMAP_TYPE_PNG);
    phys::ship *shp = new phys::ship(wld);

    std::map<int,  std::map <int, phys::point*> > points;

    for (int x = 0; x < shipimage.GetWidth(); x++)
    {
        for (int y = 0; y < shipimage.GetHeight(); y++)
        {
            if (shipimage.GetGreen(x, shipimage.GetHeight() - y - 1) < 128)
            {
                bool isHull = shipimage.GetBlue(x, shipimage.GetHeight() - y - 1) < 128;
                points[x][y] = new phys::point(wld, vec2(x - shipimage.GetWidth()/2, y), isHull? 1500 : 1000, isHull? 0 : 1);  // no buoyancy if it's a hull section
                shp->points.insert(points[x][y]);
            }
            else
            {
                points[x][y] = 0;
            }
        }
    }

    // Points have been generated, so fill in all the beams between them.
    // If beam joins two hull nodes, it is a hull beam.
    // If a non-hull node has empty space on one of its four sides, it is automatically leaking.

    for (int x = 0; x < shipimage.GetWidth(); x++)
    {
        for (int y = 0; y < shipimage.GetHeight(); y++)
        {
            phys::point *a = points[x][y];
            if (!a)
                continue;
            // First four directions out of 8: from 0 deg (+x) through to 135 deg (-x +y) - this covers each pair of points in each direction
            for (int i = 0; i < 4; i++)
            {
                phys::point *b = points[x + directions[i][0]][y + directions[i][1]];
                if (b)
                {
                    bool pointIsHull = shipimage.GetBlue(x, shipimage.GetHeight() - y - 1) < 128;
                    bool isHull = pointIsHull && shipimage.GetBlue(x + directions[i][0], shipimage.GetHeight() - y - directions[i][1] - 1) < 128;
                    double strength = 1.0 + (shipimage.GetRed(x, shipimage.GetHeight() - y - 1) / 255.0);
                    shp->springs.insert(new phys::spring(wld, a, b, isHull, -1, strength));
                    if (!isHull)
                    {
                        shp->adjacentnodes[a].insert(b);
                        shp->adjacentnodes[b].insert(a);
                    }
                    if (!(pointIsHull || (points[x+1][y] && points[x][y+1] && points[x-1][y] && points[x][y-1])))   // check for gaps next to non-hull areas:
                    {
                        points[x][y]->isLeaking = true;
                    }
                }
            }
        }
    }
}

void titanicFrame::assertSettings()
{
    wld->buoyancy = buoyancy;
    wld->strength = strength;
    wld->waterpressure = waterpressure;
    wld->waveheight = waveheight;
    wld->seadepth = seadepth;
    wld->showstress = showstress;
}

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
    // Set the context, clear the canvas and set up all the matrices.
    GLContext1->SetCurrent(*GLCanvas1);

    glViewport(0, 0, canvaswidth, canvasheight);
    glClearColor(0.529, 0.808, 0.980, 1); //(cornflower blue)
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float halfheight = zoomsize;
    float halfwidth = (float)canvaswidth / canvasheight * halfheight;
    glFrustum(-halfwidth, halfwidth, -halfheight, halfheight, 1, 1000);
    glTranslatef(-camx, -camy, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);

    glPointSize(5.f);
    glLineWidth(3.f);
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

void titanicFrame::OnGLCanvas1Paint(wxPaintEvent& event)
{

}

void titanicFrame::OnGLCanvas1Resize(wxSizeEvent& event)
{
    canvaswidth = event.GetSize().GetX();
    canvasheight = event.GetSize().GetY();
    GLCanvas1->Refresh();
}

vec2 titanicFrame::screen2world(vec2 pos)
{
    float height = zoomsize * 2.0;
    float width = (float)canvaswidth / canvasheight * height;
    return vec2((pos.x / canvaswidth - 0.5) * width + camx,
                (pos.y / canvasheight - 0.5) * -height + camy);
}

void titanicFrame::OnTimer1Trigger(wxTimerEvent& event)
{
    // Main timing event!
    initgl();
    float halfheight = zoomsize;
    float halfwidth = (float)canvaswidth / canvasheight * halfheight;
    if (mouse.ldown)
    {
        wld->destroyAt(screen2world(vec2(mouse.x, mouse.y)));
    }
    if (running)
        wld->update(0.02);
    wld->render(camx - halfwidth, camx + halfwidth, camy - halfheight, camx + halfheight);
    endgl();
}

void titanicFrame::OnMenuItemLoadSelected(wxCommandEvent& event)
{
    if (dlgOpen->ShowModal() == wxID_OK)
    {
        std::string filename = dlgOpen->GetPath().ToStdString();
        delete wld;
        wld = new phys::world;
        assertSettings();
        loadShip(filename);
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
    mouse.ldown = true;
}

void titanicFrame::OnGLCanvas1LeftUp(wxMouseEvent& event)
{
    mouse.ldown = false;
}

void titanicFrame::OnGLCanvas1RightDown(wxMouseEvent& event)
{
    mouse.rdown = true;
}

void titanicFrame::OnGLCanvas1RightUp(wxMouseEvent& event)
{
    mouse.rdown = false;
}

void titanicFrame::OnGLCanvas1MouseMove(wxMouseEvent& event)
{
    int oldx = mouse.x;
    int oldy = mouse.y;
    mouse.x = event.GetX();
    mouse.y = event.GetY();
    if (mouse.rdown)
    {
        vec2 difference = screen2world(vec2(mouse.x, mouse.y)) - screen2world(vec2(oldx, oldy));
        camx -= difference.x;
        camy -= difference.y;
    }
}

void titanicFrame::OnMenuItemOptionsSelected(wxCommandEvent& event)
{
    settings->Show();
}

void titanicFrame::OnGLCanvas1MouseWheel(wxMouseEvent& event)
{
    zoomsize *= pow(0.998, event.GetWheelRotation());
}

void titanicFrame::OnMenuItemPlayPauseSelected(wxCommandEvent& event)
{
    running = !running;
}
