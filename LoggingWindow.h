/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <wx/frame.h>
#include <wx/richtext/richtextctrl.h>

#include <memory>

class LoggingWindow : public wxFrame
{
public:

	LoggingWindow(wxWindow* parent);
	virtual ~LoggingWindow();


private:

	void OnClose(wxCloseEvent& event);
	void OnShow(wxShowEvent& event);

private:

	wxWindow const * const mParent;

	std::unique_ptr<wxRichTextCtrl> mRichTextCtrl;

	DECLARE_EVENT_TABLE()
};
