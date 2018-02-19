/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "MainFrame.h"
#include "Version.h"

#include <GameLib/GameException.h>
#include <GameLib/Log.h>

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/string.h>

#include <cassert>
#include <map>
#include <sstream>

namespace /* anonymous */ {

	std::string GetWindowTitle()
	{
		return std::string("Ship Sandbox ") + GetVersionInfo(VersionFormat::Short);
	}
}

const long ID_MAIN_CANVAS = wxNewId();

const long ID_LOAD_SHIP_MENUITEM = wxNewId();
const long ID_RELOAD_LAST_SHIP_MENUITEM = wxNewId();
const long ID_QUIT_MENUITEM = wxNewId();

const long ID_ZOOM_IN_MENUITEM = wxNewId();
const long ID_ZOOM_OUT_MENUITEM = wxNewId();
const long ID_PAUSE_MENUITEM = wxNewId();
const long ID_RESET_VIEW_MENUITEM = wxNewId();

const long ID_SMASH_MENUITEM = wxNewId();
const long ID_GRAB_MENUITEM = wxNewId();

const long ID_OPEN_SETTINGS_WINDOW_MENUITEM = wxNewId();
const long ID_OPEN_LOG_WINDOW_MENUITEM = wxNewId();

const long ID_ABOUT_MENUITEM = wxNewId();

const long ID_GAME_TIMER = wxNewId();
const long ID_STATS_REFRESH_TIMER = wxNewId();

static constexpr int CursorStep = 30;
static constexpr int PowerBarThickness = 2;

MainFrame::MainFrame()
	: mMouseInfo()
	, mCurrentToolType(ToolType::Smash)
	, mGameController()
	, mFrameCount(0u)
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

	wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
	mainPanel->Bind(wxEVT_CHAR_HOOK, (wxObjectEventFunction)&MainFrame::OnKeyDown, this);

	wxBoxSizer * mainFrameSizer = new wxBoxSizer(wxHORIZONTAL);

	Connect(this->GetId(), wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainFrame::OnMainFrameClose);
	Connect(this->GetId(), wxEVT_PAINT, (wxObjectEventFunction)&MainFrame::OnPaint);


	//
	// Build main GL canvas
	//
	
	int mainGLCanvasAttributes[] = 
	{
		WX_GL_RGBA,
		WX_GL_DOUBLEBUFFER,
        WX_GL_DEPTH_SIZE,      16,
        WX_GL_STENCIL_SIZE,    0,

        // We want to use OpenGL 3.3, Core Profile        
        // TBD: Not now, my laptop does not support OpenGL 3 :-(
        // WX_GL_CORE_PROFILE,
        // WX_GL_MAJOR_VERSION,    3,
        // WX_GL_MINOR_VERSION,    3,

		0, 0 
	};

	mMainGLCanvas = std::make_unique<wxGLCanvas>(
		mainPanel, 
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
		0);					// Border	

	// Take context for this canvas
	mMainGLCanvasContext = std::make_unique<wxGLContext>(mMainGLCanvas.get());
	mMainGLCanvasContext->SetCurrent(*mMainGLCanvas);

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


	// Control

	wxMenu * controlsMenu = new wxMenu();

	wxMenuItem * zoomInMenuItem = new wxMenuItem(controlsMenu, ID_ZOOM_IN_MENUITEM, _("Zoom In\t+"), wxEmptyString, wxITEM_NORMAL);
	controlsMenu->Append(zoomInMenuItem);
	Connect(ID_ZOOM_IN_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnZoomInMenuItemSelected);

	wxMenuItem * zoomOutMenuItem = new wxMenuItem(controlsMenu, ID_ZOOM_OUT_MENUITEM, _("Zoom Out\t-"), wxEmptyString, wxITEM_NORMAL);
	controlsMenu->Append(zoomOutMenuItem);
	Connect(ID_ZOOM_OUT_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnZoomOutMenuItemSelected);

    mPauseMenuItem = new wxMenuItem(controlsMenu, ID_PAUSE_MENUITEM, _("Pause\tP"), wxEmptyString, wxITEM_CHECK);
	controlsMenu->Append(mPauseMenuItem);
    mPauseMenuItem->Check(false);
	Connect(ID_PAUSE_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnPauseMenuItemSelected);

    controlsMenu->Append(new wxMenuItem(controlsMenu, wxID_SEPARATOR));

    wxMenuItem * resetViewMenuItem = new wxMenuItem(controlsMenu, ID_RESET_VIEW_MENUITEM, _("Reset View\tESC"), wxEmptyString, wxITEM_NORMAL);
    controlsMenu->Append(resetViewMenuItem);
    Connect(ID_RESET_VIEW_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnResetViewMenuItemSelected);

	mainMenuBar->Append(controlsMenu, _("Controls"));


	// Tools

	wxMenu * toolsMenu = new wxMenu();

	wxMenuItem * smashMenuItem = new wxMenuItem(toolsMenu, ID_SMASH_MENUITEM, _("Smash\tS"), wxEmptyString, wxITEM_RADIO);		
	toolsMenu->Append(smashMenuItem);		
	Connect(ID_SMASH_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnSmashMenuItemSelected);

	wxMenuItem * grabMenuItem = new wxMenuItem(toolsMenu, ID_GRAB_MENUITEM, _("Grab\tG"), wxEmptyString, wxITEM_RADIO);	
	toolsMenu->Append(grabMenuItem);	
	Connect(ID_GRAB_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnGrabMenuItemSelected);

	toolsMenu->Check(mCurrentToolType == ToolType::Smash ? ID_SMASH_MENUITEM : ID_GRAB_MENUITEM, true);

	mainMenuBar->Append(toolsMenu, _("Tools"));

	
	// Options

	wxMenu * optionsMenu = new wxMenu();

	wxMenuItem * openSettingsWindowMenuItem = new wxMenuItem(optionsMenu, ID_OPEN_SETTINGS_WINDOW_MENUITEM, _("Open Settings Window\tCtrl+S"), wxEmptyString, wxITEM_NORMAL);	
	optionsMenu->Append(openSettingsWindowMenuItem);
	Connect(ID_OPEN_SETTINGS_WINDOW_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnOpenSettingsWindowMenuItemSelected);

	wxMenuItem * openLogWindowMenuItem = new wxMenuItem(optionsMenu, ID_OPEN_LOG_WINDOW_MENUITEM, _("Open Log Window\tCtrl+L"), wxEmptyString, wxITEM_NORMAL);
	optionsMenu->Append(openLogWindowMenuItem);
	Connect(ID_OPEN_LOG_WINDOW_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnOpenLogWindowMenuItemSelected);
	
	mainMenuBar->Append(optionsMenu, _("Options"));


	// Help

	wxMenu * helpMenu = new wxMenu();

	wxMenuItem * aboutMenuItem = new wxMenuItem(helpMenu, ID_ABOUT_MENUITEM, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
	helpMenu->Append(aboutMenuItem);
	Connect(ID_ABOUT_MENUITEM, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainFrame::OnAboutMenuItemSelected);

	mainMenuBar->Append(helpMenu, _("Help"));

	SetMenuBar(mainMenuBar);

    //
    // Create Game controller
    //

    try
    {
        mGameController = GameController::Create();
    }
    catch (GameException const & e)
    {
        wxMessageBox("Error during initialization: " + std::string(e.what()), wxT("Error"), wxICON_ERROR);

        Close();
    }



	//
	// Finalize frame
	//

	mainPanel->SetSizerAndFit(mainFrameSizer);
	
	Maximize();
	Centre();


    //
    // Prepare cursors
    //

    mGrabCursors = MakeCursors("Data/Resources/DragCursor.png", 15, 15);
    mSmashCursors = MakeCursors("Data/Resources/SmashCursor.png", 1, 16);
    mMoveCursor = MakeCursor("Data/Resources/MoveCursor.png", 15, 15);

    SwitchCursor();


	//
	// Initialize timers
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
    if (!!mGameTimer)
	    mGameTimer->Stop();
    if (!!mStatsRefreshTimer)
	    mStatsRefreshTimer->Stop();

	Destroy();
}

void MainFrame::OnQuit(wxCommandEvent & /*event*/)
{
	Close();
}

void MainFrame::OnPaint(wxPaintEvent& event)
{
	RenderGame();

	event.Skip();
}

void MainFrame::OnKeyDown(wxKeyEvent & event)
{
    if (event.GetKeyCode() == WXK_LEFT)
    {
        // Left
        mGameController->Pan(vec2f(20.0f, 0.0f));
    }
    else if (event.GetKeyCode() == WXK_UP)
    {
        // Up
        mGameController->Pan(vec2f(00.0f, 20.0f));
    }
    else if (event.GetKeyCode() == WXK_RIGHT)
    {
        // Right
        mGameController->Pan(vec2f(-20.0f, 0.0f));
    }
    else if (event.GetKeyCode() == WXK_DOWN)
    {
        // Down
        mGameController->Pan(vec2f(0.0f, -20.0f));
    }
    else if (event.GetKeyCode() == WXK_PAGEUP)
    {
        // Ambient light up
        float newAmbientLight = std::min(1.0f, mGameController->GetAmbientLightIntensity() * 1.05f);
        mGameController->SetAmbientLightIntensity(newAmbientLight);
    }
    else if (event.GetKeyCode() == WXK_PAGEDOWN)
    {
        // Ambient light down
        float newAmbientLight = mGameController->GetAmbientLightIntensity() * 0.95f;
        mGameController->SetAmbientLightIntensity(newAmbientLight);
    }
    else if (event.GetKeyCode() == static_cast<int>(' '))
    {
        //
        // Get point stats
        //

        assert(!!mGameController);

        vec2f screenCoordinates(mMouseInfo.x, mMouseInfo.y);
        vec2f worldCoordinates = mGameController->ScreenToWorld(screenCoordinates);

        Physics::Point const * point = mGameController->GetNearestPointAt(screenCoordinates);
        if (nullptr != point)
        {
            // TODO: write directly onto window
            LogMessage("@ ", worldCoordinates.toString(), ": point @ ", point->GetPosition().toString(), "; Light=", point->GetLight());
        }
        else
        {
            // TODO: write directly onto window
            LogMessage("@ ", worldCoordinates.toString(), ": no points");
        }
    }
	
	event.Skip();
}

void MainFrame::OnGameTimerTrigger(wxTimerEvent & /*event*/)
{
    if (!!mGameController)
    {
        // Make the timer for the next step start now
        mGameTimer->Start(0, true);

        // Update the tool
        UpdateTool();

        // Do a simulation step
        if (!IsPaused())
        {
            mGameController->DoStep();
        }

        // Render
        RenderGame();
        ++mFrameCount;
    }
}

void MainFrame::OnStatsRefreshTimerTrigger(wxTimerEvent & /*event*/)
{
	std::wostringstream ss;
	ss << GetWindowTitle();
	ss << "  FPS: " << mFrameCount;

	SetTitle(ss.str());

	mFrameCount = 0u;
}

//
// Main canvas event handlers
//

void MainFrame::OnMainGLCanvasResize(wxSizeEvent & event)
{
    if (!!mGameController)
    {
        mGameController->SetCanvasSize(
            event.GetSize().GetX(),
            event.GetSize().GetY());
    }
}

void MainFrame::OnMainGLCanvasLeftDown(wxMouseEvent & /*event*/)
{
    // Remember the mouse button is down
    mMouseInfo.ldown = true;

    // Initialize the tool
    mToolState.Initialize(mMouseInfo.x, mMouseInfo.y);

    // Change cursor
    SwitchCursor();
}

void MainFrame::OnMainGLCanvasLeftUp(wxMouseEvent & /*event*/)
{
	// Remember the mouse button is not down anymore
	mMouseInfo.ldown = false;

    // Change cursor
    SwitchCursor();
}

void MainFrame::OnMainGLCanvasRightDown(wxMouseEvent & /*event*/)
{
    // Remember the mouse button is down
	mMouseInfo.rdown = true;

    SwitchCursor();
}

void MainFrame::OnMainGLCanvasRightUp(wxMouseEvent & /*event*/)
{
    // Remember the mouse button is not down anymore
	mMouseInfo.rdown = false;

    SwitchCursor();
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
		mGameController->PanImmediate(screenOffset);
	}
}

void MainFrame::OnMainGLCanvasMouseWheel(wxMouseEvent& event)
{
	assert(!!mGameController);

	mGameController->AdjustZoom(powf(1.002f, event.GetWheelRotation()));
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
			L"JPG and PNG files(*.jpg; *.png) | *.jpg; *.png",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST, 
			wxDefaultPosition, 
			wxDefaultSize, 
			_T("File Open Dialog"));
	}

	assert(!!mFileOpenDialog);

	if (mFileOpenDialog->ShowModal() == wxID_OK)
	{
		std::string filename = mFileOpenDialog->GetPath().ToStdString();

		assert(!!mGameController);
		mGameController->ResetAndLoadShip(filename);
	}
}

void MainFrame::OnReloadLastShipMenuItemSelected(wxCommandEvent & /*event*/)
{
	assert(!!mGameController);
	mGameController->ReloadLastShip();
}

void MainFrame::OnPauseMenuItemSelected(wxCommandEvent & /*event*/)
{
	// Nothing to do
}

void MainFrame::OnResetViewMenuItemSelected(wxCommandEvent & /*event*/)
{
    assert(!!mGameController);
    mGameController->ResetPan();
    mGameController->ResetZoom();
}

void MainFrame::OnZoomInMenuItemSelected(wxCommandEvent & /*event*/)
{
	assert(!!mGameController);

	mGameController->AdjustZoom(1.05f);
}

void MainFrame::OnZoomOutMenuItemSelected(wxCommandEvent & /*event*/)
{
	assert(!!mGameController);

	mGameController->AdjustZoom(0.95f);
}

void MainFrame::OnSmashMenuItemSelected(wxCommandEvent & /*event*/)
{
	mCurrentToolType = ToolType::Smash;

    SwitchCursor();
}

void MainFrame::OnGrabMenuItemSelected(wxCommandEvent & /*event*/)
{
	mCurrentToolType = ToolType::Grab;

    SwitchCursor();
}

void MainFrame::OnOpenSettingsWindowMenuItemSelected(wxCommandEvent & /*event*/)
{
	if (!mSettingsDialog)
	{
		mSettingsDialog = std::make_unique<SettingsDialog>(
			this,
			mGameController);
	}

	mSettingsDialog->Open();
}

void MainFrame::OnOpenLogWindowMenuItemSelected(wxCommandEvent & /*event*/)
{
	if (!mLoggingDialog)
	{
		mLoggingDialog = std::make_unique<LoggingDialog>(this);
	}

	mLoggingDialog->Open();
}

void MainFrame::OnAboutMenuItemSelected(wxCommandEvent & /*event*/)
{
	wxMessageBox(GetVersionInfo(VersionFormat::Long), L"Welcome to...");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::unique_ptr<wxCursor>> MainFrame::MakeCursors(std::string const & imageFilePath, int hotspotX, int hotspotY)
{
    wxBitmap* bmp = new wxBitmap(imageFilePath, wxBITMAP_TYPE_PNG);
    if (nullptr == bmp)
    {
        throw GameException("Cannot load resource '" + imageFilePath + "'");
    }

    wxImage img = bmp->ConvertToImage();
    int const imageWidth = img.GetWidth();
    int const imageHeight = img.GetHeight();

    // Set hotspots
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotspotX);
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotspotY);

    std::vector<std::unique_ptr<wxCursor>> cursors;

    // Create base
    cursors.emplace_back(std::make_unique<wxCursor>(img));

    // Create cursors for all the strengths we want to have on the power base
    static_assert(CursorStep > 0, "CursorStep is at least 1");
    static_assert(PowerBarThickness > 0, "PowerBarThickness is at least 1");

    unsigned char * data = img.GetData();
    unsigned char * alpha = img.GetAlpha();
        
    for (int c = 1; c <= CursorStep; ++c)
    {
        // Draw vertical line on the right, height = f(c),  0 < height <= imageHeight
        int powerHeight = static_cast<int>(floorf(
            static_cast<float>(imageHeight) * static_cast<float>(c) / static_cast<float>(CursorStep)
        ));

        // Start from top
        for (int y = imageHeight - powerHeight; y < imageHeight; ++y)
        {
            int index = imageWidth - PowerBarThickness - 1 + (imageWidth * y);

            // Thickness
            for (int t = 0; t < PowerBarThickness; ++t, ++index)
            {
                // Red:   ff3300
                // Green: 00ff00
                alpha[index] = 0xff;
                data[index * 3] = (c == CursorStep) ? 0x00 : 0xFF;
                data[index * 3 + 1] = (c == CursorStep) ? 0xFF : 0x33;
                data[index * 3 + 2] = (c == CursorStep) ? 0x00 : 0x00;
            }
        }

        cursors.emplace_back(std::make_unique<wxCursor>(img));
    }

    delete (bmp);

    return cursors;
}

std::unique_ptr<wxCursor> MainFrame::MakeCursor(std::string const & imageFilePath, int hotspotX, int hotspotY)
{
    wxBitmap* bmp = new wxBitmap(imageFilePath, wxBITMAP_TYPE_PNG);
    if (nullptr == bmp)
    {
        throw GameException("Cannot load resource '" + imageFilePath + "'");
    }

    wxImage img = bmp->ConvertToImage();

    // Set hotspots
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotspotX);
    img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotspotY);

    return std::make_unique<wxCursor>(img);
}

void MainFrame::SwitchCursor()
{
    // Set base cursor
    if (mMouseInfo.rdown)
    {
        this->SetCursor(*mMoveCursor);
    }
    else
    {
        switch (mCurrentToolType)
        {
            case ToolType::Grab:
            {
                this->SetCursor(*(mGrabCursors[0]));
                break;
            }

            case ToolType::Smash:
            {
                this->SetCursor(*(mSmashCursors[0]));
                break;
            }
        }
    }
}

void MainFrame::SetCursorStrength(float strength, float minStrength, float maxStrength)
{
    assert(mMouseInfo.ldown);

    std::vector<std::unique_ptr<wxCursor>> const * cursors;
    switch (mCurrentToolType)
    {
        case ToolType::Grab:
        {
            cursors = &mGrabCursors;
            break;
        }

        case ToolType::Smash:
        {
            cursors = &mSmashCursors;
            break;
        }

        default:
        {
            cursors = nullptr;
            break;
        }
    }

    assert(nullptr != cursors);

    // Calculate cursor index (cursor 0 is the base, we don't use it here)
    size_t const numberOfCursors = (cursors->size() - 1);
    size_t cursorIndex = 1u + static_cast<size_t>(floorf(
        (strength - minStrength) / (maxStrength - minStrength) * static_cast<float>(numberOfCursors - 1)
        ));

    switch (mCurrentToolType)
    {
        case ToolType::Grab:
        {
            this->SetCursor(*(mGrabCursors[cursorIndex]));
            break;
        }

        case ToolType::Smash:
        {
            this->SetCursor(*(mSmashCursors[cursorIndex]));
            break;
        }
    }
}

void MainFrame::UpdateTool()
{
    // We apply the tool only if the left mouse button is down
    if (mMouseInfo.ldown)
    {
        auto now = std::chrono::steady_clock::now();

        // Accumulate total time iff we haven't moved since last time
        if (mToolState.PreviousMouseX == mMouseInfo.x
            && mToolState.PreviousMouseY == mMouseInfo.y)
        {
            mToolState.CumulatedTime += std::chrono::duration_cast<std::chrono::microseconds>(
                now - mToolState.PreviousTimestamp);
        }
        else
        {
            // We've moved, don't accumulate but use time
            // that was built up so far
        }

        // Remember new position and timestamp
        mToolState.PreviousMouseX = mMouseInfo.x;
        mToolState.PreviousMouseY = mMouseInfo.y;
        mToolState.PreviousTimestamp = now;

	    // Apply current tool
        assert(!!mGameController);
	    switch (mCurrentToolType)
	    {
		    case ToolType::Grab:
		    {
                // Calculate strength multiplier
                // 0-500ms      = 1.0
                // 10000ms-+INF = 10.0

                float millisecondsElapsed = static_cast<float>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(mToolState.CumulatedTime).count());
                float strengthMultiplier = 1.0f + 9.0f * std::min(1.0f, millisecondsElapsed / 10000.0f);

                // Draw
			    mGameController->DrawTo(vec2(mMouseInfo.x, mMouseInfo.y), strengthMultiplier);

                // Modulate cursor
                SetCursorStrength(strengthMultiplier, 1.0f, 10.0f);

			    break;
		    }

		    case ToolType::Smash:
		    {
                // Calculate radius multiplier
                // 0-500ms      = 1.0
                // 10000ms-+INF = 10.0

                float millisecondsElapsed = static_cast<float>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(mToolState.CumulatedTime).count());
                float radiusMultiplier = 1.0f + 9.0f * std::min(1.0f, millisecondsElapsed / 10000.0f);

                // Destroy
			    mGameController->DestroyAt(vec2(mMouseInfo.x, mMouseInfo.y), radiusMultiplier);

                // Modulate cursor
                SetCursorStrength(radiusMultiplier, 1.0f, 10.0f);

			    break;
		    }
	    }
    }
}

bool MainFrame::IsPaused()
{
    return mPauseMenuItem->IsChecked();
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

void MainFrame::RenderGame()
{
    if (!!mGameController)
    {
        // Render
        mGameController->Render();

        // Flush all the draw operations and flip the back buffer onto the screen.	
        mMainGLCanvas->SwapBuffers();
        mMainGLCanvas->Refresh();
    }
}



