/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-02-21
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/textctrl.h>

#include <memory>
#include <string>

class SplashScreenDialog : public wxDialog
{
public:

    SplashScreenDialog();
	
	virtual ~SplashScreenDialog();

	void UpdateProgress(
        float progress,
        std::string const & message);

private:

    wxGauge * mGauge;
    wxTextCtrl * mTextCtrl;    
};
