/***************************************************************************************
 * Original Author:		Luke Wren (wren6991@gmail.com)
 * Created:				2013-04-30
 * Modified By:			Gabriele Giuseppini
 * Copyright:			Luke Wren (http://github.com/Wren6991), 
 *						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
 ***************************************************************************************/

//
// The main application. This journey begins from here.
//

#include "MainFrame.h"

#include <GameLib/GameController.h>
#include <GameLib/GameException.h>

#include <wx/app.h>
#include <wx/msgdlg.h>

class MainApp : public wxApp
{
public:
	virtual bool OnInit() override;
};

IMPLEMENT_APP(MainApp);

bool MainApp::OnInit()
{
	wxInitAllImageHandlers();

	// Create Game controller
	// TODO: splash screen and game controller progress
	std::unique_ptr<GameController> gameController;
	try
	{
		gameController = GameController::Create();
	}
	catch (GameException const & e)
	{
		wxMessageBox(L"Error during initialization: " + e.GetErrorMessage(), wxT("Error"), wxICON_ERROR);
		return false;
	}

	MainFrame* frame = new MainFrame(std::move(gameController));
	frame->SetIcon(wxICON(SHIP_ICON));
	frame->Show();
	SetTopWindow(frame);
	
	return true;
}