/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "MainFrame.h"

#include "Log.h"
#include "phys.h"
#include "Version.h"

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/string.h>

#include <map>
#include <sstream>

namespace /* anonymous */ {

	std::wstring GetWindowTitle()
	{
		return std::wstring(L"Ship Sandbox ") + GetVersionInfo(VersionFormat::Short);
	}
}

const long ID_MAIN_CANVAS = wxNewId();
const long ID_LOAD_SHIP_MENUITEM = wxNewId();
const long ID_RELOAD_LAST_SHIP_MENUITEM = wxNewId();
const long ID_QUIT_MENUITEM = wxNewId();
const long ID_SMASH_MENUITEM = wxNewId();
const long ID_GRAB_MENUITEM = wxNewId();
const long ID_OPEN_SETTINGS_WINDOW_MENUITEM = wxNewId();
const long ID_OPEN_LOG_WINDOW_MENUITEM = wxNewId();
const long ID_PLAY_PAUSE_MENUITEM = wxNewId();
const long ID_ABOUT_MENUITEM = wxNewId();
const long ID_GAME_TIMER = wxNewId();
const long ID_STATS_REFRESH_TIMER = wxNewId();


//BEGIN_EVENT_TABLE(MainFrame, wxFrame)
//END_EVENT_TABLE()


MainFrame::MainFrame(wxWindow* parent)
{
	Create(
		parent, 
		wxID_ANY,
		GetWindowTitle(),		
		wxDefaultPosition, 
		wxDefaultSize, 
		wxDEFAULT_FRAME_STYLE, 
		_T("Main Frame"));

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	wxBoxSizer * mainFrameSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainFrameSizer);

	Connect(this->GetId(), wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainFrame::OnMainFrameClose);


	//
	// Build main GL canvas
	//
	
	int mainGLCanvasAttributes[8] = 
	{
		WX_GL_RGBA,
		WX_GL_DOUBLEBUFFER,
		WX_GL_DEPTH_SIZE,      16,
		WX_GL_STENCIL_SIZE,    0,
		0, 0 
	};

	mMainGLCanvas = std::make_unique<wxGLCanvas>(
		this,
		ID_MAIN_CANVAS,
		mainGLCanvasAttributes,
		wxDefaultPosition,
		wxSize(640, 480),
		0L,
		_T("Main GL Canvas"));	

	mMainGLCanvas->Connect(wxEVT_SIZE, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasResize, 0, this);
	mMainGLCanvas->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasLeftDown, 0, this);
	mMainGLCanvas->Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasLeftUp, 0, this);
	mMainGLCanvas->Connect(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasRightDown, 0, this);
	mMainGLCanvas->Connect(wxEVT_RIGHT_UP, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasRightUp, 0, this);
	mMainGLCanvas->Connect(wxEVT_MOTION, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasMouseMove, 0, this);
	mMainGLCanvas->Connect(wxEVT_MOUSEWHEEL, (wxObjectEventFunction)&MainFrame::OnMainGLCanvasMouseWheel, 0, this);
	
	mainFrameSizer->Add(
		mMainGLCanvas.get(),
		1,					// Proportion
		wxALL | wxEXPAND,	// Flags
		5);					// Border	

	// Take context for this canvas
	mMainGLCanvasContext = std::make_unique<wxGLContext>(mMainGLCanvas.get());


	//
	// Build menu
	//

	wxMenuBar * mainMenuBar = new wxMenuBar();
		
	// File

	wxMenu * fileMenu = new wxMenu();
	
	wxMenuItem * loadShipMenuItem = new wxMenuItem(fileMenu, ID_LOAD_SHIP_MENUITEM, _("Load Ship\tCtrl+O"), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(loadShipMenuItem);
	Connect(ID_LOAD_SHIP_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnLoadShipMenuItemSelected);

	wxMenuItem * reloadLastShipMenuItem = new wxMenuItem(fileMenu, ID_RELOAD_LAST_SHIP_MENUITEM, _("Reload Last Ship\tCtrl+R"), wxEmptyString, wxITEM_NORMAL);
	fileMenu->Append(reloadLastShipMenuItem);
	Connect(ID_RELOAD_LAST_SHIP_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnReloadLastShipMenuItemSelected);

	wxMenuItem* quitMenuItem = new wxMenuItem(fileMenu, ID_QUIT_MENUITEM, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
	fileMenu->Append(quitMenuItem);
	Connect(ID_QUIT_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnQuit);

	mainMenuBar->Append(fileMenu, _("&File"));

	// Tools

	wxMenu * toolsMenu = new wxMenu();

	wxMenuItem * smashMenuItem = new wxMenuItem(toolsMenu, ID_SMASH_MENUITEM, _("Smash"), wxEmptyString, wxITEM_RADIO);
	toolsMenu->Append(smashMenuItem);
	Connect(ID_SMASH_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnSmashMenuItemSelected);

	wxMenuItem * grabMenuItem = new wxMenuItem(toolsMenu, ID_GRAB_MENUITEM, _("Grab"), wxEmptyString, wxITEM_RADIO);
	toolsMenu->Append(grabMenuItem);
	Connect(ID_GRAB_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnGrabMenuItemSelected);

	mainMenuBar->Append(toolsMenu, _("Tools"));

	
	// Options

	wxMenu * optionsMenu = new wxMenu();

	wxMenuItem * openSettingsWindowMenuItem = new wxMenuItem(optionsMenu, ID_OPEN_SETTINGS_WINDOW_MENUITEM, _("Open Settings Window\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);	
	optionsMenu->Append(openSettingsWindowMenuItem);
	Connect(ID_OPEN_SETTINGS_WINDOW_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnOpenSettingsWindowMenuItemSelected);

	wxMenuItem * openLogWindowMenuItem = new wxMenuItem(optionsMenu, ID_OPEN_LOG_WINDOW_MENUITEM, _("Show Log Window\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
	optionsMenu->Append(openLogWindowMenuItem);
	Connect(ID_OPEN_LOG_WINDOW_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnOpenLogWindowMenuItemSelected);

	wxMenuItem * playPauseMenuItem = new wxMenuItem(optionsMenu, ID_PLAY_PAUSE_MENUITEM, _("Play/Pause\tCtrl+P"), wxEmptyString, wxITEM_NORMAL);
	optionsMenu->Append(playPauseMenuItem);
	Connect(ID_PLAY_PAUSE_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnPlayPauseMenuItemSelected);
	
	mainMenuBar->Append(optionsMenu, _("Options"));

	// Help

	wxMenu * helpMenu = new wxMenu();

	wxMenuItem * aboutMenuItem = new wxMenuItem(helpMenu, ID_ABOUT_MENUITEM, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
	helpMenu->Append(aboutMenuItem);
	Connect(ID_ABOUT_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnAboutMenuItemSelected);

	mainMenuBar->Append(helpMenu, _("Help"));

	SetMenuBar(mainMenuBar);

	// Finalize frame
	mainFrameSizer->Fit(this);
	mainFrameSizer->SetSizeHints(this);


	// TODOHERE


	gm.loadShip(L"ship.png");

	gm.zoomsize = 30;
	gm.running = true;

	gm.camx = 0;
	gm.camy = 0;

	mFrameCount = 0;
	gm.canvaswidth = 200;
	gm.canvasheight = 200;


	//
	// Timers
	//
	
	mGameTimer = std::make_unique<wxTimer>(this, ID_GAME_TIMER);
	Connect(ID_GAME_TIMER, wxEVT_TIMER, (wxObjectEventFunction)&MainFrame::OnGameTimerTrigger);	
	mGameTimer->Start(0, true);

	mStatsRefreshTimer = std::make_unique<wxTimer>(this, ID_STATS_REFRESH_TIMER);
	Connect(ID_STATS_REFRESH_TIMER, wxEVT_TIMER, (wxObjectEventFunction)&MainFrame::OnStatsRefreshTimerTrigger);	
	mStatsRefreshTimer->Start(1000, false);
}

MainFrame::~MainFrame()
{
}

//
// App event handlers
//

void MainFrame::OnMainFrameClose(wxCloseEvent & /*event*/)
{
	mGameTimer->Stop();
	mStatsRefreshTimer->Stop();

	Destroy();
}

void MainFrame::OnQuit(wxCommandEvent & /*event*/)
{
	Close();
}

void MainFrame::OnGameTimerTrigger(wxTimerEvent & /*event*/)
{
	// Main timing event!
	mFrameCount++;
	initgl();
	gm.update();
	gm.render();
	endgl();

	mGameTimer->Start(0, true);
}

void MainFrame::OnStatsRefreshTimerTrigger(wxTimerEvent & /*event*/)
{
	std::wostringstream ss;
	ss << GetWindowTitle();
	ss << "  FPS: " << mFrameCount;

	SetTitle(ss.str());

	mFrameCount = 0;
}

//
// Main canvas event handlers
//

void MainFrame::OnMainGLCanvasResize(wxSizeEvent & event)
{
	gm.canvaswidth = event.GetSize().GetX();
	gm.canvasheight = event.GetSize().GetY();
	mMainGLCanvas->Refresh();
}

void MainFrame::OnMainGLCanvasLeftDown(wxMouseEvent & /*event*/)
{
	gm.mouse.ldown = true;
}

void MainFrame::OnMainGLCanvasLeftUp(wxMouseEvent & /*event*/)
{
	gm.mouse.ldown = false;
}

void MainFrame::OnMainGLCanvasRightDown(wxMouseEvent & /*event*/)
{
	gm.mouse.rdown = true;
}

void MainFrame::OnMainGLCanvasRightUp(wxMouseEvent & /*event*/)
{
	gm.mouse.rdown = false;
}

void MainFrame::OnMainGLCanvasMouseMove(wxMouseEvent& event)
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

void MainFrame::OnMainGLCanvasMouseWheel(wxMouseEvent& event)
{
	gm.zoomsize *= pow(0.998, event.GetWheelRotation());
}


//
// Menu event handlers
//

void MainFrame::OnLoadShipMenuItemSelected(wxCommandEvent & /*event*/)
{
	if (!mFileOpenDialog)
	{
		mFileOpenDialog = std::make_unique<wxFileDialog>(
			this, 
			L"Select Ship", 
			wxEmptyString, 
			wxEmptyString, 
			L"(*.png)|*.png", 
			wxFD_OPEN | wxFD_FILE_MUST_EXIST, 
			wxDefaultPosition, 
			wxDefaultSize, 
			_T("File Open Dialog"));
	}

	assert(!!mFileOpenDialog);

	if (mFileOpenDialog->ShowModal() == wxID_OK)
	{
		std::wstring filename = mFileOpenDialog->GetPath().ToStdWstring();

		delete gm.wld;
		gm.wld = new phys::world;
		gm.assertSettings();
		gm.loadShip(filename);
	}
}

void MainFrame::OnReloadLastShipMenuItemSelected(wxCommandEvent & /*event*/)
{
	delete gm.wld;
	gm.wld = new phys::world;
	gm.assertSettings();
	gm.loadShip(gm.lastFilename);
}

void MainFrame::OnSmashMenuItemSelected(wxCommandEvent & /*event*/)
{
	gm.tool = game::TOOL_SMASH;
}

void MainFrame::OnGrabMenuItemSelected(wxCommandEvent & /*event*/)
{
	gm.tool = game::TOOL_GRAB;
}

void MainFrame::OnOpenSettingsWindowMenuItemSelected(wxCommandEvent & /*event*/)
{
	if (!mSettingsDialog)
	{
		mSettingsDialog = std::make_unique<SettingsDialog>(this);
	}

	mSettingsDialog->Show();
}

void MainFrame::OnOpenLogWindowMenuItemSelected(wxCommandEvent & /*event*/)
{
	if (!mLoggingWindow)
	{
		mLoggingWindow = std::make_unique<LoggingWindow>(this);
	}

	mLoggingWindow->Show();
}

void MainFrame::OnPlayPauseMenuItemSelected(wxCommandEvent & /*event*/)
{
	gm.running = !gm.running;
}

void MainFrame::OnAboutMenuItemSelected(wxCommandEvent & /*event*/)
{
	wxMessageBox(GetVersionInfo(VersionFormat::Long), L"Welcome to...");
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

	// TODO: see if all of this can be done once and for all
	mMainGLCanvasContext->SetCurrent(*mMainGLCanvas);

	// Clear canvas (blue)
	glViewport(0, 0, gm.canvaswidth, gm.canvasheight);
	glClearColor(0.529f, 0.808f, 0.980f, 1.0f); //(cornflower blue)
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
	mMainGLCanvas->SwapBuffers();
	mMainGLCanvas->Refresh();
}



