/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "SettingsDialog.h"

#include "Log.h"

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>

const long ID_STRENGTH_SLIDER = wxNewId();
const long ID_BUOYANCY_SLIDER = wxNewId();


BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
END_EVENT_TABLE()


SettingsDialog::SettingsDialog(
	wxWindow* parent,
	std::shared_ptr<GameController> gameController)
	: mParent(parent)
	, mGameController(std::move(gameController))
{
	Create(
		mParent, 
		wxID_ANY, 
		_("Settings"), 
		wxDefaultPosition, 
		wxSize(400, 200),
		wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX | wxFRAME_SHAPED, 
		_T("Settings Window"));


	//
	// Lay out the controls
	//

	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->AddSpacer(10);
	
	// Controls

	wxBoxSizer* controlsSizer = new wxBoxSizer(wxHORIZONTAL);

	controlsSizer->AddSpacer(20);
	

	// Strength

	wxBoxSizer* strengthSizer = new wxBoxSizer(wxVERTICAL);

	wxSlider * strengthSlider = new wxSlider(this, ID_STRENGTH_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_LABELS | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Strength Slider"));
	Connect(ID_STRENGTH_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnGenericSliderScroll);
	Connect(ID_STRENGTH_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnGenericSliderScroll);
	
	strengthSizer->Add(strengthSlider, 0, wxALIGN_CENTRE);

	wxStaticText * strengthLabel = new wxStaticText(this, wxID_ANY, _("Strength"), wxDefaultPosition, wxDefaultSize, 0, _T("Strength Label"));
	
	strengthSizer->Add(strengthLabel, 0, wxALIGN_CENTRE);
	
	strengthSizer->AddSpacer(20);

	controlsSizer->Add(strengthSizer, 0);
	
	controlsSizer->AddSpacer(40);

	// Buoyancy
	
	wxBoxSizer* buoyancySizer = new wxBoxSizer(wxVERTICAL);

	wxSlider * buoyancySlider = new wxSlider(this, ID_BUOYANCY_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200), 
		wxSL_VERTICAL | wxSL_LEFT | wxSL_LABELS | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Buoyancy Slider"));
	Connect(ID_BUOYANCY_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnGenericSliderScroll);
	Connect(ID_BUOYANCY_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnGenericSliderScroll);

	buoyancySizer->Add(buoyancySlider, 0, wxALIGN_CENTRE);

	wxStaticText * buoyancyLabel = new wxStaticText(this, wxID_ANY, _("Buoyancy"), wxDefaultPosition, wxDefaultSize, 0, _T("Buoyancy Label"));
	buoyancySizer->Add(buoyancyLabel, 0, wxALIGN_CENTRE);
	
	buoyancySizer->AddSpacer(20);

	controlsSizer->Add(buoyancySizer, 0);
	
	// TODO: add spacer (40)

	// TODO: others


	controlsSizer->AddSpacer(20);

	mainSizer->Add(controlsSizer);

	mainSizer->AddSpacer(20);


	// Buttons

	wxBoxSizer * buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonsSizer->AddSpacer(20);

	mOkButton = new wxButton(this, wxID_OK);
	Connect(wxID_OK, wxEVT_BUTTON, (wxObjectEventFunction)&SettingsDialog::OnOkButton);
	buttonsSizer->Add(mOkButton);

	buttonsSizer->AddSpacer(20);

	mCancelButton = new wxButton(this, wxID_CANCEL);
	buttonsSizer->Add(mCancelButton);

	buttonsSizer->AddSpacer(20);

	mApplyButton = new wxButton(this, wxID_APPLY);
	mApplyButton->Enable(false);
	Connect(wxID_APPLY, wxEVT_BUTTON, (wxObjectEventFunction)&SettingsDialog::OnApplyButton);
	buttonsSizer->Add(mApplyButton);
	
	buttonsSizer->AddSpacer(20);

	mainSizer->Add(buttonsSizer);
	
	mainSizer->AddSpacer(20);


	// TODOHERE

	/*
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Buoyancy"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer1->Add(StaticText2, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	sldBuoyancy = new wxSlider(this, ID_SLIDER2, 4, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER2"));
	BoxSizer1->Add(sldBuoyancy, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Wave Height"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer1->Add(StaticText3, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	sldWaveHeight = new wxSlider(this, ID_SLIDER4, 2, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER4"));
	BoxSizer1->Add(sldWaveHeight, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Water Pressure"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer1->Add(StaticText4, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	sldWaterPressure = new wxSlider(this, ID_SLIDER5, 5, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER5"));
	BoxSizer1->Add(sldWaterPressure, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Ocean Depth"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	BoxSizer1->Add(StaticText5, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	sldSeaDepth = new wxSlider(this, ID_SLIDER3, 150, 50, 1000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER3"));
	sldSeaDepth->SetTickFreq(50);
	BoxSizer1->Add(sldSeaDepth, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	chkShowStress = new wxCheckBox(this, ID_CHECKBOX2, _("Highlight Stress"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	chkShowStress->SetValue(false);
	BoxSizer1->Add(chkShowStress, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	mChkQuickFix = new wxCheckBox(this, ID_CHECKBOX1, _("Quick Water Fix"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	mChkQuickFix->SetValue(false);
	BoxSizer1->Add(chkQuickFix, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	chkXRay = new wxCheckBox(this, ID_CHECKBOX3, _("X-Ray Mode"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	chkXRay->SetValue(false);
	BoxSizer1->Add(chkXRay, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);

	Connect(ID_SLIDER2, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER2, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER4, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER4, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER5, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER5, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER3, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER3, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_CHECKBOX2, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnCheckBox1Click);
	Connect(ID_CHECKBOX1, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnCheckBox1Click);
	Connect(ID_CHECKBOX3, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnCheckBox1Click);
	*/



	//
	// Finalize dialog
	//

	SetSizerAndFit(mainSizer);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::Open()
{
	assert(!!mGameController);

	// TODO: populate controls with controller values

	// We're not dirty
	mApplyButton->Enable(false);

	this->Show();
}

void SettingsDialog::OnGenericSliderScroll(wxScrollEvent & /*event*/)
{
	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnOkButton(wxCommandEvent & /*event*/)
{
	// TODO: read values from controls and set into controller
	assert(!!mGameController);

	// Close ourselves
	this->Close();
}

void SettingsDialog::OnApplyButton(wxCommandEvent & /*event*/)
{
	// TODO: read values from controls and set into controller
	assert(!!mGameController);

	// We're not dirty anymore
	mApplyButton->Enable(false);
}

// TODOOLD
void SettingsDialog::OnSlider1CmdScroll(wxScrollEvent& event)
{
	
	/* TODO
	MainFrame *frame = dynamic_cast<MainFrame *>(mParent);
	frame->gm.strength = (sldStrength->GetValue() ? sldStrength->GetValue() : 0.5) * 0.005;
	frame->gm.buoyancy = mSldBuoyancy->GetValue();
	frame->gm.waveheight = sldWaveHeight->GetValue() * 0.5;
	frame->gm.waterpressure = sldWaterPressure->GetValue() * 0.06;
	frame->gm.seadepth = sldSeaDepth->GetValue();
	frame->gm.showstress = chkShowStress->GetValue();
	frame->gm.quickwaterfix = mChkQuickFix->GetValue();
	frame->gm.xraymode = chkXRay->GetValue();
	frame->gm.assertSettings();
	*/
	LogDebug("SettingsDialog::Slider moved");
}

void SettingsDialog::OnCheckBox1Click(wxCommandEvent& event)
{
	wxScrollEvent evt;
	OnSlider1CmdScroll(evt);
}