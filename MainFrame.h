/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini 
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "LoggingWindow.h"

#include "game.h"
#include "SettingsDialog.h"

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

	MainFrame(wxWindow* parent);
	virtual ~MainFrame();
	game gm;

private:

	//
	// Canvas
	//

	std::unique_ptr<wxGLCanvas> mMainGLCanvas;
	std::unique_ptr<wxGLContext> mMainGLCanvasContext;

	//
	// Dialogs
	//

	std::unique_ptr<wxFileDialog> mFileOpenDialog;
	std::unique_ptr<LoggingWindow> mLoggingWindow;
	std::unique_ptr<SettingsDialog> mSettingsDialog;

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
	void OnLoadShipMenuItemSelected(wxCommandEvent& event);
	void OnReloadLastShipMenuItemSelected(wxCommandEvent& event);
	void OnSmashMenuItemSelected(wxCommandEvent& event);
	void OnGrabMenuItemSelected(wxCommandEvent& event);
	void OnOpenSettingsWindowMenuItemSelected(wxCommandEvent& event);
	void OnOpenLogWindowMenuItemSelected(wxCommandEvent& event);
	void OnPlayPauseMenuItemSelected(wxCommandEvent& event);
	void OnAboutMenuItemSelected(wxCommandEvent& event);

private:

	int mFrameCount;	

	void initgl();
	void endgl();
};
