/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <wx/frame.h>

class LoggingWindow : public wxFrame
{
public:

	LoggingWindow(wxWindow* parent, wxWindowID id = -1);
	virtual ~LoggingWindow();


private:

	void OnClose(wxCloseEvent& event);
	void OnShow(wxShowEvent& event);

private:


	wxWindow *parent;

	DECLARE_EVENT_TABLE()
};
