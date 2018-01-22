/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "SettingsDialog.h"

#include "Log.h"
#include "MainFrame.h"

#include <wx/intl.h>
#include <wx/string.h>

const long SettingsDialog::ID_STATICTEXT1 = wxNewId();
const long SettingsDialog::ID_SLIDER1 = wxNewId();
const long SettingsDialog::ID_STATICTEXT2 = wxNewId();
const long SettingsDialog::ID_SLIDER2 = wxNewId();
const long SettingsDialog::ID_STATICTEXT3 = wxNewId();
const long SettingsDialog::ID_SLIDER4 = wxNewId();
const long SettingsDialog::ID_STATICTEXT4 = wxNewId();
const long SettingsDialog::ID_SLIDER5 = wxNewId();
const long SettingsDialog::ID_STATICTEXT5 = wxNewId();
const long SettingsDialog::ID_SLIDER3 = wxNewId();
const long SettingsDialog::ID_CHECKBOX2 = wxNewId();
const long SettingsDialog::ID_CHECKBOX1 = wxNewId();
const long SettingsDialog::ID_CHECKBOX3 = wxNewId();

BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent)
{
	mParent = parent;
	//(*Initialize(settingsDialog)
	wxBoxSizer* BoxSizer1;

	Create(parent, wxID_ANY, _("Pac0\'s Ship Sandbox"), wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLOSE_BOX | wxMINIMIZE_BOX | wxFRAME_SHAPED, _T("wxID_ANY"));
	SetMinSize(wxSize(200, 300));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	/*
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Strength"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	sldStrength = new wxSlider(this, ID_SLIDER1, 5, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER1"));
	BoxSizer1->Add(sldStrength, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
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
	*/
	sldSeaDepth = new wxSlider(this, ID_SLIDER3, 150, 50, 1000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER3"));
	sldSeaDepth->SetTickFreq(50);
	//BoxSizer1->Add(sldSeaDepth, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	chkShowStress = new wxCheckBox(this, ID_CHECKBOX2, _("Highlight Stress"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	chkShowStress->SetValue(false);
	//BoxSizer1->Add(chkShowStress, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	mChkQuickFix = std::make_unique<wxCheckBox>(this, ID_CHECKBOX1, _("Quick Water Fix"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	mChkQuickFix->SetValue(false);
	//BoxSizer1->Add(chkQuickFix, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	chkXRay = new wxCheckBox(this, ID_CHECKBOX3, _("X-Ray Mode"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	chkXRay->SetValue(false);
	//BoxSizer1->Add(chkXRay, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	//SetSizer(BoxSizer1);
	//BoxSizer1->Fit(this);
	//BoxSizer1->SetSizeHints(this);

	Connect(ID_SLIDER1, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
	Connect(ID_SLIDER1, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSlider1CmdScroll);
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
	//*)
}

SettingsDialog::~SettingsDialog()
{
}


void SettingsDialog::OnSlider1CmdScroll(wxScrollEvent& event)
{
	MainFrame *frame = dynamic_cast<MainFrame *>(mParent);
	/* TODO
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