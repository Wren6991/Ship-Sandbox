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

#ifdef FLOATING_POINT_CHECKS
#ifdef _MSC_VER
#include <float.h>
// Enable all floating point exceptions except these
unsigned int fp_control_state = _controlfp(_EM_INEXACT | _EM_UNDERFLOW, _MCW_EM);
#else
// Have no idea how to do this on other compilers...
#endif
#endif

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
		wxMessageBox("Error during initialization: " + std::string(e.what()), wxT("Error"), wxICON_ERROR);
		return false;
	}

	MainFrame* frame = new MainFrame(std::move(gameController));
	frame->SetIcon(wxICON(AAA_SHIP_ICON));
	frame->Show();
	SetTopWindow(frame);
	
	return true;
}