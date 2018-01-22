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

#include <cassert>
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

MainFrame::MainFrame(std::shared_ptr<GameController> gameController)
	: mMouseInfo()
	, mToolType(ToolType::Smash)
	, mGameController(std::move(gameController))
	, mFrameCount(0)
{
	Create(
		nullptr, 
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

	// TODO: select the right one based off mToolType

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


	//
	// Finalize frame
	//

	mainFrameSizer->Fit(this);
	mainFrameSizer->SetSizeHints(this);


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
	assert(!!mGameController);

	// Main timing event!
	mFrameCount++;
	initgl();
	mGameController->DoStep();
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
	LogDebug("MainFrame::OnMainGLCanvasResize");

	assert(!!mGameController);

	mGameController->SetCanvasSize(
		event.GetSize().GetX(),
		event.GetSize().GetY());

	mMainGLCanvas->Refresh();
}

void MainFrame::OnMainGLCanvasLeftDown(wxMouseEvent & /*event*/)
{
	assert(!!mGameController);

	// Action depends on current tool
	switch (mToolType)	
	{
		case ToolType::Grab:
		{
			mGameController->DrawAt(vec2(mMouseInfo.x, mMouseInfo.y));
			break;
		}

		case ToolType::Smash:
		{
			mGameController->DestroyAt(vec2(mMouseInfo.x, mMouseInfo.y));
			break;
		}

	}
	
	// Remember the mouse button is down
	mMouseInfo.ldown = true;
}

void MainFrame::OnMainGLCanvasLeftUp(wxMouseEvent & /*event*/)
{
	// Remember the mouse button is not down anymore
	mMouseInfo.ldown = false;
}

void MainFrame::OnMainGLCanvasRightDown(wxMouseEvent & /*event*/)
{
	mMouseInfo.rdown = true;
}

void MainFrame::OnMainGLCanvasRightUp(wxMouseEvent & /*event*/)
{
	mMouseInfo.rdown = false;
}

void MainFrame::OnMainGLCanvasMouseMove(wxMouseEvent& event)
{
	assert(!!mGameController);

	int oldx = mMouseInfo.x;
	int oldy = mMouseInfo.y;
	mMouseInfo.x = event.GetX();
	mMouseInfo.y = event.GetY();
	if (mMouseInfo.rdown)
	{
		// Pan
		vec2 screenOffset = vec2(mMouseInfo.x, mMouseInfo.y) - vec2(oldx, oldy);
		mGameController->Pan(screenOffset);
	}
}

void MainFrame::OnMainGLCanvasMouseWheel(wxMouseEvent& event)
{
	assert(!!mGameController);

	mGameController->AdjustZoom(powf(0.998f, event.GetWheelRotation()));
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

		assert(!!mGameController);

		mGameController->Reset(filename);
	}
}

void MainFrame::OnReloadLastShipMenuItemSelected(wxCommandEvent & /*event*/)
{
	/* TODO
	delete gm.wld;
	gm.wld = new phys::world;
	gm.assertSettings();
	gm.loadShip(gm.lastFilename);
	*/
}

void MainFrame::OnSmashMenuItemSelected(wxCommandEvent & /*event*/)
{
	mToolType = ToolType::Smash;
}

void MainFrame::OnGrabMenuItemSelected(wxCommandEvent & /*event*/)
{
	mToolType = ToolType::Grab;
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
	assert(!!mGameController);

	// TODO: use state of menu item selection
	mGameController->SetRunningState(true);
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
	// Set the context
	// TODO: see if all of this can be done once and for all
	mMainGLCanvasContext->SetCurrent(*mMainGLCanvas);
}

void MainFrame::endgl()
{
	// Flush all the draw operations and flip the back buffer onto the screen.
	glFlush();
	mMainGLCanvas->SwapBuffers();
	mMainGLCanvas->Refresh();
}



