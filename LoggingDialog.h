/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <wx/dialog.h>
#include <wx/textctrl.h>

#include <memory>

class LoggingWindow : public wxDialog
{
public:

	LoggingWindow(wxWindow* parent);
	
	virtual ~LoggingWindow();

	void Open();

private:

	void OnClose(wxCloseEvent& event);

private:

	wxWindow * const mParent;

	wxTextCtrl * mTextCtrl;

	DECLARE_EVENT_TABLE()
};
