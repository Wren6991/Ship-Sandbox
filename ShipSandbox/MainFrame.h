/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini 
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "LoggingDialog.h"
#include "SettingsDialog.h"

#include <GameLib/GameController.h>

#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/timer.h>

#include <memory>

/*
 * The main window of the game's GUI.
 */
class MainFrame : public wxFrame
{
public:

	MainFrame(std::shared_ptr<GameController> gameController);

	virtual ~MainFrame();

private:

	//
	// Canvas
	//

	std::unique_ptr<wxGLCanvas> mMainGLCanvas;
	std::unique_ptr<wxGLContext> mMainGLCanvasContext;

    //
    // Controls that we're interacting with
    //

    wxMenuItem * mPauseMenuItem;

	//
	// Dialogs
	//

	std::unique_ptr<wxFileDialog> mFileOpenDialog;
	std::unique_ptr<LoggingDialog> mLoggingDialog;
	std::unique_ptr<SettingsDialog> mSettingsDialog;

    //
    // Cursors
    //

    std::unique_ptr<wxCursor> mGrabCursor;
    std::unique_ptr<wxCursor> mMoveCursor;
    std::unique_ptr<wxCursor> mSmashCursor;


	//
	// Timers
	//

	std::unique_ptr<wxTimer> mGameTimer;
	std::unique_ptr<wxTimer> mStatsRefreshTimer;

private:

	//
	// Event handlers
	//

	// App
	void OnMainFrameClose(wxCloseEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnGameTimerTrigger(wxTimerEvent& event);
	void OnStatsRefreshTimerTrigger(wxTimerEvent& event);

	// Main GL canvas
	void OnMainGLCanvasResize(wxSizeEvent& event);
	void OnMainGLCanvasLeftDown(wxMouseEvent& event);
	void OnMainGLCanvasLeftUp(wxMouseEvent& event);
	void OnMainGLCanvasRightDown(wxMouseEvent& event);
	void OnMainGLCanvasRightUp(wxMouseEvent& event);
	void OnMainGLCanvasMouseMove(wxMouseEvent& event);
	void OnMainGLCanvasMouseWheel(wxMouseEvent& event);

	// Menu
	void OnZoomInMenuItemSelected(wxCommandEvent& event);
	void OnZoomOutMenuItemSelected(wxCommandEvent& event);
	void OnPauseMenuItemSelected(wxCommandEvent& event);
	void OnLoadShipMenuItemSelected(wxCommandEvent& event);
	void OnReloadLastShipMenuItemSelected(wxCommandEvent& event);
	void OnSmashMenuItemSelected(wxCommandEvent& event);
	void OnGrabMenuItemSelected(wxCommandEvent& event);
	void OnOpenSettingsWindowMenuItemSelected(wxCommandEvent& event);
	void OnOpenLogWindowMenuItemSelected(wxCommandEvent& event);	
	void OnAboutMenuItemSelected(wxCommandEvent& event);

private:

    std::unique_ptr<wxCursor> MakeCursor(std::string const & imageFilePath, int hotspotX, int hotspotY);
    void SwitchCursor();
	void ApplyCurrentMouseTool();
	void RenderGame();

private:

	struct MouseInfo
	{
		bool ldown;
		bool rdown;
		int x;
		int y;
		int lastx;
		int lasty;

		MouseInfo()
			: ldown(false)
			, rdown(false)
			, x(0)
			, y(0)
			, lastx(0)
			, lasty(0)
		{
		}
	};
	
	MouseInfo mMouseInfo;

	enum class ToolType
	{
		Smash,
		Grab
	};

	ToolType mCurrentToolType;

	std::shared_ptr<GameController> mGameController;
	int mFrameCount;	
};
