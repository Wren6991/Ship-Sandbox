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

#include "game.h"
#include "settingsDialog.h"

class titanicFrame: public wxFrame
{
public:

    titanicFrame(wxWindow* wld,wxWindowID id = -1);
    virtual ~titanicFrame();
    game gm;

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
    void OnMenuItemSmashSelected(wxCommandEvent& event);
    void OnMenuItemGrabSelected(wxCommandEvent& event);
    void OnTimer2Trigger(wxTimerEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnMenuReloadSelected(wxCommandEvent& event);
    //*)

    //(*Identifiers(titanicFrame)
    static const long ID_GLCANVAS1;
    static const long ID_MENUITEM1;
    static const long ID_MENUITEM5;
    static const long idMenuQuit;
    static const long ID_MENUITEM3;
    static const long ID_MENUITEM4;
    static const long mnuShow;
    static const long ID_MENUITEM2;
    static const long idMenuAbout;
    static const long ID_TIMER1;
    static const long ID_TIMER2;
    //*)

    //(*Declarations(titanicFrame)
    wxMenuItem* MenuItem5;
    wxMenu* Menu3;
    wxMenuItem* MenuItem4;
    wxGLCanvas* GLCanvas1;
    wxFileDialog* dlgOpen;
    wxMenuItem* MenuItemGrab;
    wxMenuItem* MenuItem3;
    wxTimer Timer2;
    wxMenuItem* MenuItem6;
    wxMenuItem* MenuItemSmash;
    wxTimer Timer1;
    wxMenu* Menu4;
    //*)

    int frameCount;
    settingsDialog *settings;

    wxGLContext* GLContext1;

    void initgl();
    void endgl();


    DECLARE_EVENT_TABLE()
};

#endif // TITANICMAIN_H
