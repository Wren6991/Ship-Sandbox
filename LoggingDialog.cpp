/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-01-19
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "LoggingWindow.h"

#include "Log.h"

#include <cassert>

wxBEGIN_EVENT_TABLE(LoggingWindow, wxFrame)
	EVT_CLOSE(LoggingWindow::OnClose)
	EVT_SHOW(LoggingWindow::OnShow)
wxEND_EVENT_TABLE()

LoggingWindow::LoggingWindow(wxWindow * parent)
	: mParent(parent)
{
	Create(
		parent, 
		-1, 
		_("Logging"), 
		wxDefaultPosition, 
		wxDefaultSize, 
		wxSTAY_ON_TOP | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN 
			| wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT,
		_T("Logging Window"));

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//
	// Create Text control
	//

	mTextCtrl = std::make_unique<wxTextCtrl>(
		this, 
		wxID_ANY, 
		wxEmptyString, 
		wxDefaultPosition,
		wxSize(200, 200), 
		wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxVSCROLL | wxHSCROLL | wxBORDER_NONE);

	wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	mTextCtrl->SetFont(font);
}

LoggingWindow::~LoggingWindow()
{
}

void LoggingWindow::OnClose(wxCloseEvent & /*event*/)
{
	this->Show(false);
}

void LoggingWindow::OnShow(wxShowEvent & event)
{
	if (event.IsShown())
	{
		Logger::Instance.RegisterListener(
			[this](std::wstring const & message)
			{
				assert(!!this->mTextCtrl);
				this->mTextCtrl->WriteText(message);
			});
	}
	else
	{
		Logger::Instance.UnregisterListener();

		// Be nice, clear the control
		assert(!!this->mTextCtrl);
		this->mTextCtrl->Clear();
	}
}

