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
		_T("id"));

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	//
	// Create RichText control
	//

	mRichTextCtrl = std::make_unique<wxRichTextCtrl>(
		this, 
		wxID_ANY, 
		wxEmptyString, 
		wxDefaultPosition,
		wxSize(200, 200), 
		wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxRE_READONLY);

	wxFont font(10, wxTELETYPE, wxNORMAL, wxNORMAL);
	mRichTextCtrl->SetFont(font);
}

LoggingWindow::~LoggingWindow()
{
}

void LoggingWindow::OnClose(wxCloseEvent& event)
{
	this->Show(false);
}

void LoggingWindow::OnShow(wxShowEvent& event)
{
	if (event.IsShown())
	{
		Logger::Instance.RegisterListener(
			[this](std::wstring const & message)
			{
				assert(!!this->mRichTextCtrl);
				this->mRichTextCtrl->WriteText(message);
			});
	}
	else
	{
		Logger::Instance.UnregisterListener();
		this->mRichTextCtrl->Clear();
	}
}

