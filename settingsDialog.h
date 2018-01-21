/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include <memory>

class SettingsDialog : public wxDialog
{
public:

	SettingsDialog(wxWindow* parent);
	virtual ~SettingsDialog();

	std::unique_ptr<wxCheckBox> mChkQuickFix;
	std::unique_ptr<wxSlider> mSldBuoyancy;
	wxStaticText* StaticText2;
	wxCheckBox* chkShowStress;
	wxSlider* sldStrength;
	wxStaticText* StaticText1;
	wxStaticText* StaticText3;
	wxSlider* sldWaveHeight;
	wxStaticText* StaticText5;
	wxSlider* sldSeaDepth;
	wxStaticText* StaticText4;
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

	wxWindow *mParent;

	DECLARE_EVENT_TABLE()
};
