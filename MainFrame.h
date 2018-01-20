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
#include "settingsDialog.h"

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

	MainFrame(wxWindow* parent, wxWindowID id = -1);
	virtual ~MainFrame();
	game gm;

private:

	std::unique_ptr<LoggingWindow> mLoggingWindow;

private:

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
	void OnMenuItemShowLoggingWindowSelected(wxCommandEvent& event);
	void OnGLCanvas1MouseWheel(wxMouseEvent& event);
	void OnMenuItemPlayPauseSelected(wxCommandEvent& event);
	void OnMenuItemSmashSelected(wxCommandEvent& event);
	void OnMenuItemGrabSelected(wxCommandEvent& event);
	void OnTimer2Trigger(wxTimerEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnMenuReloadSelected(wxCommandEvent& event);
	
private:

	static const long ID_GLCANVAS1;
	static const long ID_MENUITEM1;
	static const long ID_MENUITEM5;
	static const long idMenuQuit;
	static const long ID_MENUITEM3;
	static const long ID_MENUITEM4;
	static const long mnuShow;
	static const long mnuShowLoggingWindow;
	static const long ID_MENUITEM2;
	static const long idMenuAbout;
	static const long ID_TIMER1;
	static const long ID_TIMER2;

	wxMenuItem* mMenuItem5;
	wxMenu* mMenu3;
	wxMenuItem* mMenuItem4;
	wxMenuItem* mMenuItemShowLoggingWindow;
	wxGLCanvas* mGLCanvas1;
	wxFileDialog* mDlgOpen;
	wxMenuItem* mMenuItemGrab;
	wxMenuItem* mMenuItem3;
	wxTimer mTimer2;
	wxMenuItem* mMenuItem6;
	wxMenuItem* mMenuItemSmash;
	wxTimer mTimer1;
	wxMenu* mMenu4;

	int mFrameCount;
	settingsDialog *mSettings;	

	wxGLContext* mGLContext1;

	void initgl();
	void endgl();

	DECLARE_EVENT_TABLE()
};
