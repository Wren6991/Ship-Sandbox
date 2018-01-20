/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/

#include "MainFrame.h"

#include "phys.h"

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/string.h>

#include <map>
#include <sstream>

namespace /* anonymous */ {

	enum wxbuildinfoformat {
		short_f, long_f
	};

}

#define VERSION "2.0"

// TODO: see this one
wxString wxbuildinfo(wxbuildinfoformat format)
{
	return "Ship Sandbox Alpha v" VERSION "\n(c) Luke Wren 2013\n(c) Gabriele Giuseppini 2017\nLicensed to Francis Racicot";
}

const long MainFrame::ID_GLCANVAS1 = wxNewId();
const long MainFrame::ID_MENUITEM1 = wxNewId();
const long MainFrame::ID_MENUITEM5 = wxNewId();
const long MainFrame::idMenuQuit = wxNewId();
const long MainFrame::ID_MENUITEM3 = wxNewId();
const long MainFrame::ID_MENUITEM4 = wxNewId();
const long MainFrame::mnuShow = wxNewId();
const long MainFrame::mnuShowLoggingWindow = wxNewId();
const long MainFrame::ID_MENUITEM2 = wxNewId();
const long MainFrame::idMenuAbout = wxNewId();
const long MainFrame::ID_TIMER1 = wxNewId();
const long MainFrame::ID_TIMER2 = wxNewId();

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
END_EVENT_TABLE()

MainFrame::MainFrame(
	wxWindow* parent, 
	wxWindowID id)
{
	wxMenuItem* MenuItem2;
	wxMenuItem* MenuItem1;
	wxMenu* Menu1;
	wxBoxSizer* BoxSizer1;
	wxMenuBar* MenuBar1;
	wxMenu* Menu2;

	Create(parent, id, _("Ship Sandbox Alpha " VERSION), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	int GLCanvasAttributes_1[8] = {
		WX_GL_RGBA,
		WX_GL_DOUBLEBUFFER,
		WX_GL_DEPTH_SIZE,      16,
		WX_GL_STENCIL_SIZE,    0,
		0, 0 };
	mGLCanvas1 = new wxGLCanvas(
		this,
		ID_GLCANVAS1,
		GLCanvasAttributes_1,
		wxDefaultPosition,
		wxSize(640, 480),
		0L,
		_T("ID_GLCANVAS1"));	

	BoxSizer1->Add(mGLCanvas1, 1, wxALL | wxEXPAND /*| wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL*/, 5);
	SetSizer(BoxSizer1);
	MenuBar1 = new wxMenuBar();
	Menu1 = new wxMenu();
	mMenuItem3 = new wxMenuItem(Menu1, ID_MENUITEM1, _("Load Ship\tCtrl+O"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(mMenuItem3);
	mMenuItem6 = new wxMenuItem(Menu1, ID_MENUITEM5, _("Reload Last Ship\tCtrl+R"), wxEmptyString, wxITEM_NORMAL);
	Menu1->Append(mMenuItem6);
	MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
	Menu1->Append(MenuItem1);
	MenuBar1->Append(Menu1, _("&File"));
	mMenu4 = new wxMenu();
	mMenuItemSmash = new wxMenuItem(mMenu4, ID_MENUITEM3, _("Smash"), wxEmptyString, wxITEM_RADIO);
	mMenu4->Append(mMenuItemSmash);
	mMenuItemGrab = new wxMenuItem(mMenu4, ID_MENUITEM4, _("Grab"), wxEmptyString, wxITEM_RADIO);
	mMenu4->Append(mMenuItemGrab);
	MenuBar1->Append(mMenu4, _("Tools"));
	mMenu3 = new wxMenu();
	mMenuItem4 = new wxMenuItem(mMenu3, mnuShow, _("Show\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);
	mMenu3->Append(mMenuItem4);

	mMenuItemShowLoggingWindow = new wxMenuItem(mMenu3, mnuShowLoggingWindow, _("Show Logging\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
	mMenu3->Append(mMenuItemShowLoggingWindow);

	mMenuItem5 = new wxMenuItem(mMenu3, ID_MENUITEM2, _("Play/Pause\tCtrl+P"), wxEmptyString, wxITEM_NORMAL);
	mMenu3->Append(mMenuItem5);
	MenuBar1->Append(mMenu3, _("Options"));
	Menu2 = new wxMenu();
	MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
	Menu2->Append(MenuItem2);
	MenuBar1->Append(Menu2, _("Help"));
	SetMenuBar(MenuBar1);
	mTimer1.SetOwner(this, ID_TIMER1);	
	mDlgOpen = new wxFileDialog(this, _("Select Ship Image"), wxEmptyString, wxEmptyString, _("(*.png)|*.png"), wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
	mTimer2.SetOwner(this, ID_TIMER2);	
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	mGLCanvas1->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&MainFrame::OnGLCanvas1LeftDown, 0, this);
	mGLCanvas1->Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&MainFrame::OnGLCanvas1LeftUp, 0, this);
	mGLCanvas1->Connect(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&MainFrame::OnGLCanvas1RightDown, 0, this);
	mGLCanvas1->Connect(wxEVT_RIGHT_UP, (wxObjectEventFunction)&MainFrame::OnGLCanvas1RightUp, 0, this);
	mGLCanvas1->Connect(wxEVT_MOTION, (wxObjectEventFunction)&MainFrame::OnGLCanvas1MouseMove, 0, this);
	mGLCanvas1->Connect(wxEVT_MOUSEWHEEL, (wxObjectEventFunction)&MainFrame::OnGLCanvas1MouseWheel, 0, this);
	mGLCanvas1->Connect(wxEVT_SIZE, (wxObjectEventFunction)&MainFrame::OnGLCanvas1Resize, 0, this);
	Connect(ID_MENUITEM1, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemLoadSelected);
	Connect(ID_MENUITEM5, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuReloadSelected);
	Connect(idMenuQuit, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnQuit);
	Connect(ID_MENUITEM3, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemSmashSelected);
	Connect(ID_MENUITEM4, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemGrabSelected);
	Connect(mnuShow, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemOptionsSelected);
	Connect(mnuShowLoggingWindow, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemShowLoggingWindowSelected);
	Connect(ID_MENUITEM2, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnMenuItemPlayPauseSelected);
	Connect(idMenuAbout, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnAbout);
	Connect(ID_TIMER1, wxEVT_TIMER, (wxObjectEventFunction)&MainFrame::OnTimer1Trigger);
	Connect(ID_TIMER2, wxEVT_TIMER, (wxObjectEventFunction)&MainFrame::OnTimer2Trigger);
	Connect(wxID_ANY, wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainFrame::OnClose);
	//*)
	
	mSettings = new settingsDialog(this);

	mGLContext1 = new wxGLContext(mGLCanvas1);

	gm.loadShip(L"ship.png");

	gm.zoomsize = 30;
	gm.running = true;

	gm.camx = 0;
	gm.camy = 0;

	mFrameCount = 0;
	gm.canvaswidth = 200;
	gm.canvasheight = 200;

	mTimer1.Start(20, false);
	mTimer2.Start(1000, false);
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxString msg = wxbuildinfo(long_f);
	wxMessageBox(msg, _("Welcome to..."));
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

void MainFrame::initgl()
{
	// Set the context, clear the gm.canvas and set up all the matrices.
	mGLContext1->SetCurrent(*mGLCanvas1);

	glViewport(0, 0, gm.canvaswidth, gm.canvasheight);
	glClearColor(0.529, 0.808, 0.980, 1); //(cornflower blue)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(0.15f * gm.canvasheight / gm.zoomsize);
	glLineWidth(0.1f * gm.canvasheight / gm.zoomsize);
	glColor3f(0, 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MainFrame::endgl()
{
	// Flush all the draw operations and flip the back buffer onto the screen.
	glFlush();
	mGLCanvas1->SwapBuffers();
	mGLCanvas1->Refresh();
}

void MainFrame::OnGLCanvas1Resize(wxSizeEvent& event)
{
	gm.canvaswidth = event.GetSize().GetX();
	gm.canvasheight = event.GetSize().GetY();
	mGLCanvas1->Refresh();
}



void MainFrame::OnTimer1Trigger(wxTimerEvent& event)
{
	// Main timing event!
	mFrameCount++;
	initgl();
	gm.update();
	gm.render();
	endgl();
}

void MainFrame::OnMenuItemLoadSelected(wxCommandEvent& event)
{
	if (mDlgOpen->ShowModal() == wxID_OK)
	{
		std::wstring filename = mDlgOpen->GetPath().ToStdWstring();
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

void MainFrame::OnGLCanvas1LeftDown(wxMouseEvent& event)
{
	gm.mouse.ldown = true;
}

void MainFrame::OnGLCanvas1LeftUp(wxMouseEvent& event)
{
	gm.mouse.ldown = false;
}

void MainFrame::OnGLCanvas1RightDown(wxMouseEvent& event)
{
	gm.mouse.rdown = true;
}

void MainFrame::OnGLCanvas1RightUp(wxMouseEvent& event)
{
	gm.mouse.rdown = false;
}

void MainFrame::OnGLCanvas1MouseMove(wxMouseEvent& event)
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

void MainFrame::OnMenuItemOptionsSelected(wxCommandEvent& event)
{
	mSettings->Show();
}

void MainFrame::OnMenuItemShowLoggingWindowSelected(wxCommandEvent& event)
{
	if (!mLoggingWindow)
	{
		mLoggingWindow = std::make_unique<LoggingWindow>(this);
	}

	mLoggingWindow->Show();
}

void MainFrame::OnGLCanvas1MouseWheel(wxMouseEvent& event)
{
	gm.zoomsize *= pow(0.998, event.GetWheelRotation());
}

void MainFrame::OnMenuItemPlayPauseSelected(wxCommandEvent& event)
{
	gm.running = !gm.running;
}

void MainFrame::OnMenuItemSmashSelected(wxCommandEvent& event)
{
	gm.tool = game::TOOL_SMASH;
}

void MainFrame::OnMenuItemGrabSelected(wxCommandEvent& event)
{
	gm.tool = game::TOOL_GRAB;
}

void MainFrame::OnTimer2Trigger(wxTimerEvent& event)
{
	std::ostringstream ss;
	ss << "Ship Sandbox Alpha " VERSION;
	ss << "  FPS: " << mFrameCount;

	SetTitle(ss.str());
	
	mFrameCount = 0;
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	Destroy();
}

void MainFrame::OnMenuReloadSelected(wxCommandEvent& event)
{
	delete gm.wld;
	gm.wld = new phys::world;
	gm.assertSettings();
	gm.loadShip(gm.lastFilename);
}