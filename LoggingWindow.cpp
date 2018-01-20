#include "LoggingWindow.h"

#include "Log.h"

wxBEGIN_EVENT_TABLE(LoggingWindow, wxFrame)
EVT_CLOSE(LoggingWindow::OnClose)
EVT_SHOW(LoggingWindow::OnShow)
wxEND_EVENT_TABLE()

LoggingWindow::LoggingWindow(
	wxWindow* parent, 
	wxWindowID id)
{
	// TODO: rich edit box

	Create(
		parent, 
		id, 
		_("Logging"), 
		wxDefaultPosition, 
		wxDefaultSize, 
		wxSTAY_ON_TOP | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN 
			| wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT,
		_T("id"));

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
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
				// TODO
			});
	}
	else
	{
		Logger::Instance.UnregisterListener();
	}
}

