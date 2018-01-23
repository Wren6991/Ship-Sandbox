/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include "GameController.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/slider.h>

#include <memory>

class SettingsDialog : public wxDialog
{
public:

	SettingsDialog(
		wxWindow * parent,
		std::shared_ptr<GameController> gameController);

	virtual ~SettingsDialog();

	void Open();

private:
	
	void OnGenericSliderScroll(wxScrollEvent & event);
	void OnOkButton(wxCommandEvent & event);
	void OnApplyButton(wxCommandEvent & event);

private:

	// Controls
	wxButton * mOkButton;
	wxButton * mCancelButton;
	wxButton * mApplyButton;

// TODOOLD
	wxCheckBox * mChkQuickFix;
	wxSlider * mSldBuoyancy;
	wxCheckBox* chkShowStress;
	wxSlider* sldStrength;
	wxSlider* sldWaveHeight;
	wxSlider* sldSeaDepth;
	wxSlider* sldWaterPressure;
	wxCheckBox* chkXRay;
	//*)

protected:

	//(*Identifiers(settingsDialog)
	static const long ID_STATICTEXT1;
	static const long ID_SLIDER1;
	static const long ID_STATICTEXT2;
	static const long ID_SLIDER2;
	static const long ID_STATICTEXT3;
	static const long ID_SLIDER4;
	static const long ID_STATICTEXT4;
	static const long ID_SLIDER5;
	static const long ID_STATICTEXT5;
	static const long ID_SLIDER3;
	static const long ID_CHECKBOX2;
	static const long ID_CHECKBOX1;
	static const long ID_CHECKBOX3;
	//*)

private:

	//(*Handlers(settingsDialog)
	void OnSlider1CmdScroll(wxScrollEvent& event);
	void OnCheckBox1Click(wxCommandEvent& event);
	//*)

	wxWindow * const mParent;
	std::shared_ptr<GameController> mGameController;

	DECLARE_EVENT_TABLE()
};
