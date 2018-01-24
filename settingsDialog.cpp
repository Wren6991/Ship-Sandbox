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
const long ID_WATER_PRESSURE_SLIDER = wxNewId();
const long ID_WAVE_HEIGHT_SLIDER = wxNewId();
const long ID_SEA_DEPTH_SLIDER = wxNewId();

const long ID_QUICK_WATER_FIX_CHECKBOX = wxNewId();
const long ID_SHOW_STRESS_CHECKBOX = wxNewId();
const long ID_XRAY_CHECKBOX = wxNewId();

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
	// Lay out the dialog
	//

	wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

	mainSizer->AddSpacer(10);
	

	// Controls

	wxBoxSizer* controlsSizer = new wxBoxSizer(wxHORIZONTAL);

	controlsSizer->AddSpacer(20);
	

	// Strength

	wxBoxSizer* strengthSizer = new wxBoxSizer(wxVERTICAL);

	mStrengthSlider = new wxSlider(this, ID_STRENGTH_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Strength Slider"));
	Connect(ID_STRENGTH_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnStrengthSliderScroll);
	Connect(ID_STRENGTH_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnStrengthSliderScroll);	
	strengthSizer->Add(mStrengthSlider, 0, wxALIGN_CENTRE);

	wxStaticText * strengthLabel = new wxStaticText(this, wxID_ANY, _("Strength"), wxDefaultPosition, wxDefaultSize, 0, _T("Strength Label"));	
	strengthSizer->Add(strengthLabel, 0, wxALIGN_CENTRE);

	mStrengthTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	strengthSizer->Add(mStrengthTextCtrl, 0, wxALIGN_CENTRE);

	strengthSizer->AddSpacer(20);

	controlsSizer->Add(strengthSizer, 0);
	
	controlsSizer->AddSpacer(40);


	// Buoyancy
	
	wxBoxSizer* buoyancySizer = new wxBoxSizer(wxVERTICAL);

	mBuoyancySlider = new wxSlider(this, ID_BUOYANCY_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200), 
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Buoyancy Slider"));
	Connect(ID_BUOYANCY_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnBuoyancySliderScroll);
	Connect(ID_BUOYANCY_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnBuoyancySliderScroll);
	buoyancySizer->Add(mBuoyancySlider, 0, wxALIGN_CENTRE);

	wxStaticText * buoyancyLabel = new wxStaticText(this, wxID_ANY, _("Buoyancy"), wxDefaultPosition, wxDefaultSize, 0, _T("Buoyancy Label"));
	buoyancySizer->Add(buoyancyLabel, 0, wxALIGN_CENTRE);
	
	mBuoyancyTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	buoyancySizer->Add(mBuoyancyTextCtrl, 0, wxALIGN_CENTRE);

	buoyancySizer->AddSpacer(20);

	controlsSizer->Add(buoyancySizer, 0);
	
	controlsSizer->AddSpacer(40);


	// Water Pressure

	wxBoxSizer* waterPressureSizer = new wxBoxSizer(wxVERTICAL);

	mWaterPressureSlider = new wxSlider(this, ID_WATER_PRESSURE_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Water Pressure Slider"));
	Connect(ID_WATER_PRESSURE_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnWaterPressureSliderScroll);
	Connect(ID_WATER_PRESSURE_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnWaterPressureSliderScroll);
	waterPressureSizer->Add(mWaterPressureSlider, 0, wxALIGN_CENTRE);

	wxStaticText * waterPressureLabel = new wxStaticText(this, wxID_ANY, _("Water Pressure"), wxDefaultPosition, wxDefaultSize, 0, _T("Water Pressure Label"));
	waterPressureSizer->Add(waterPressureLabel, 0, wxALIGN_CENTRE);

	mWaterPressureTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	waterPressureSizer->Add(mWaterPressureTextCtrl, 0, wxALIGN_CENTRE);

	waterPressureSizer->AddSpacer(20);

	controlsSizer->Add(waterPressureSizer, 0);

	controlsSizer->AddSpacer(40);


	// Wave Height

	wxBoxSizer* waveHeightSizer = new wxBoxSizer(wxVERTICAL);

	mWaveHeightSlider = new wxSlider(this, ID_WAVE_HEIGHT_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Wave Height Slider"));
	Connect(ID_WAVE_HEIGHT_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnWaveHeightSliderScroll);
	Connect(ID_WAVE_HEIGHT_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnWaveHeightSliderScroll);
	waveHeightSizer->Add(mWaveHeightSlider, 0, wxALIGN_CENTRE);

	wxStaticText * waveHeightLabel = new wxStaticText(this, wxID_ANY, _("Wave Height"), wxDefaultPosition, wxDefaultSize, 0, _T("Wave Height Label"));
	waveHeightSizer->Add(waveHeightLabel, 0, wxALIGN_CENTRE);

	mWaveHeightTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	waveHeightSizer->Add(mWaveHeightTextCtrl, 0, wxALIGN_CENTRE);

	waveHeightSizer->AddSpacer(20);

	controlsSizer->Add(waveHeightSizer, 0);

	controlsSizer->AddSpacer(40);


	// Sea Depth

	wxBoxSizer* seaDepthSizer = new wxBoxSizer(wxVERTICAL);

	mSeaDepthSlider = new wxSlider(this, ID_SEA_DEPTH_SLIDER, 50, 0, 100, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Sea Depth Slider"));
	Connect(ID_SEA_DEPTH_SLIDER, wxEVT_SCROLL_TOP | wxEVT_SCROLL_BOTTOM | wxEVT_SCROLL_LINEUP | wxEVT_SCROLL_LINEDOWN | wxEVT_SCROLL_PAGEUP | wxEVT_SCROLL_PAGEDOWN | wxEVT_SCROLL_THUMBTRACK | wxEVT_SCROLL_THUMBRELEASE | wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&SettingsDialog::OnSeaDepthSliderScroll);
	Connect(ID_SEA_DEPTH_SLIDER, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&SettingsDialog::OnSeaDepthSliderScroll);
	seaDepthSizer->Add(mSeaDepthSlider, 0, wxALIGN_CENTRE);

	wxStaticText * seaDepthLabel = new wxStaticText(this, wxID_ANY, _("Ocean Depth"), wxDefaultPosition, wxDefaultSize, 0, _T("Sea Depth Label"));
	seaDepthSizer->Add(seaDepthLabel, 0, wxALIGN_CENTRE);

	mSeaDepthTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	seaDepthSizer->Add(mSeaDepthTextCtrl, 0, wxALIGN_CENTRE);

	seaDepthSizer->AddSpacer(20);

	controlsSizer->Add(seaDepthSizer, 0);

	controlsSizer->AddSpacer(40);

	
	// Check boxes

	wxStaticBoxSizer* checkboxesSizer = new wxStaticBoxSizer(wxVERTICAL, this);
	
	mQuickWaterFixCheckBox = new wxCheckBox(this, ID_QUICK_WATER_FIX_CHECKBOX, _("See Ship Through Water"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("Quick Water Fix Checkbox"));
	Connect(ID_QUICK_WATER_FIX_CHECKBOX, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnQuickWaterFixCheckBoxClick);
	checkboxesSizer->Add(mQuickWaterFixCheckBox, 0, wxALL | wxALIGN_LEFT, 5);

	mShowStressCheckBox = new wxCheckBox(this, ID_SHOW_STRESS_CHECKBOX, _("Show Stress"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("Show Stress Checkbox"));
	Connect(ID_SHOW_STRESS_CHECKBOX, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnShowStressCheckBoxClick);
	checkboxesSizer->Add(mShowStressCheckBox, 0, wxALL | wxALIGN_LEFT, 5);

	mXRayCheckBox = new wxCheckBox(this, ID_XRAY_CHECKBOX, _("X-Ray Mode"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("Use XRay Checkbox"));
	Connect(ID_XRAY_CHECKBOX, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnXRayCheckBoxClick);
	checkboxesSizer->Add(mXRayCheckBox, 0, wxALL | wxALIGN_LEFT, 5);

	controlsSizer->Add(checkboxesSizer, 0);



	controlsSizer->AddSpacer(20);

	mainSizer->Add(controlsSizer);

	mainSizer->AddSpacer(20);


	// Buttons

	wxBoxSizer * buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonsSizer->AddSpacer(20);

	mOkButton = new wxButton(this, wxID_OK);
	Connect(wxID_OK, wxEVT_BUTTON, (wxObjectEventFunction)&SettingsDialog::OnOkButton);
	buttonsSizer->Add(mOkButton, 0);

	buttonsSizer->AddSpacer(20);

	mCancelButton = new wxButton(this, wxID_CANCEL);
	buttonsSizer->Add(mCancelButton, 0);

	buttonsSizer->AddSpacer(20);

	mApplyButton = new wxButton(this, wxID_APPLY);
	mApplyButton->Enable(false);
	Connect(wxID_APPLY, wxEVT_BUTTON, (wxObjectEventFunction)&SettingsDialog::OnApplyButton);
	buttonsSizer->Add(mApplyButton, 0);
	
	buttonsSizer->AddSpacer(20);

	mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT);
	
	mainSizer->AddSpacer(20);



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

	// Populate ourselves with current settings
	ReadSettings();

	// We're not dirty
	mApplyButton->Enable(false);

	this->Show();
}

void SettingsDialog::OnStrengthSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mStrengthSlider,
		mGameController->GetMinStrength(),
		mGameController->GetMaxStrength());

	mStrengthTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnBuoyancySliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mBuoyancySlider,
		mGameController->GetMinBuoyancy(),
		mGameController->GetMaxBuoyancy());

	mBuoyancyTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnWaterPressureSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mWaterPressureSlider,
		mGameController->GetMinWaterPressure(),
		mGameController->GetMaxWaterPressure());

	mWaterPressureTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnWaveHeightSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mWaveHeightSlider,
		mGameController->GetMinWaveHeight(),
		mGameController->GetMaxWaveHeight());

	mWaveHeightTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnSeaDepthSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mSeaDepthSlider,
		mGameController->GetMinSeaDepth(),
		mGameController->GetMaxSeaDepth());

	mSeaDepthTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnQuickWaterFixCheckBoxClick(wxCommandEvent & /*event*/)
{
	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnShowStressCheckBoxClick(wxCommandEvent & /*event*/)
{
	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnXRayCheckBoxClick(wxCommandEvent & /*event*/)
{
	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnOkButton(wxCommandEvent & /*event*/)
{
	assert(!!mGameController);

	// Write settings back to controller
	ApplySettings();

	// Close ourselves
	this->Close();
}

void SettingsDialog::OnApplyButton(wxCommandEvent & /*event*/)
{
	assert(!!mGameController);

	// Write settings back to controller
	ApplySettings();

	// We're not dirty anymore
	mApplyButton->Enable(false);
}

void SettingsDialog::ApplySettings()
{
	assert(!!mGameController);

	mGameController->SetStrength(
		SliderToRealValue(
			mStrengthSlider,
			mGameController->GetMinStrength(),
			mGameController->GetMaxStrength()));

	mGameController->SetBuoyancy(
		SliderToRealValue(
			mBuoyancySlider,
			mGameController->GetMinBuoyancy(),
			mGameController->GetMaxBuoyancy()));

	mGameController->SetWaterPressure(
		SliderToRealValue(
			mWaterPressureSlider,
			mGameController->GetMinWaterPressure(),
			mGameController->GetMaxWaterPressure()));

	mGameController->SetWaveHeight(
		SliderToRealValue(
			mWaveHeightSlider,
			mGameController->GetMinWaveHeight(),
			mGameController->GetMaxWaveHeight()));

	mGameController->SetSeaDepth(
		SliderToRealValue(
			mSeaDepthSlider,
			mGameController->GetMinSeaDepth(),
			mGameController->GetMaxSeaDepth()));

	mGameController->SetDoQuickWaterFix(mQuickWaterFixCheckBox->IsChecked());

	mGameController->SetDoShowStress(mShowStressCheckBox->IsChecked());

	mGameController->SetDoUseXRayMode(mXRayCheckBox->IsChecked());
}

void SettingsDialog::ReadSettings()
{
	assert(!!mGameController);

	RealValueToSlider(
		mGameController->GetStrength(),
		mGameController->GetMinStrength(),
		mGameController->GetMaxStrength(),
		mStrengthSlider);

	mStrengthTextCtrl->SetValue(std::to_string(mGameController->GetStrength()));

	RealValueToSlider(
		mGameController->GetBuoyancy(),
		mGameController->GetMinBuoyancy(),
		mGameController->GetMaxBuoyancy(),
		mBuoyancySlider);

	mBuoyancyTextCtrl->SetValue(std::to_string(mGameController->GetBuoyancy()));

	RealValueToSlider(
		mGameController->GetWaterPressure(),
		mGameController->GetMinWaterPressure(),
		mGameController->GetMaxWaterPressure(),
		mWaterPressureSlider);

	mWaterPressureTextCtrl->SetValue(std::to_string(mGameController->GetWaterPressure()));

	RealValueToSlider(
		mGameController->GetWaveHeight(),
		mGameController->GetMinWaveHeight(),
		mGameController->GetMaxWaveHeight(),
		mWaveHeightSlider);

	mWaveHeightTextCtrl->SetValue(std::to_string(mGameController->GetWaveHeight()));

	RealValueToSlider(
		mGameController->GetSeaDepth(),
		mGameController->GetMinSeaDepth(),
		mGameController->GetMaxSeaDepth(),
		mSeaDepthSlider);

	mSeaDepthTextCtrl->SetValue(std::to_string(mGameController->GetSeaDepth()));

	mQuickWaterFixCheckBox->SetValue(mGameController->GetDoQuickWaterFix());

	mShowStressCheckBox->SetValue(mGameController->GetDoShowStress());

	mXRayCheckBox->SetValue(mGameController->GetDoUseXRayMode());
}

float SettingsDialog::SliderToRealValue(
	wxSlider * const slider,
	float minValue,
	float maxValue) const
{
	int sliderValue = slider->GetValue();
	float realValue = minValue + (static_cast<float>(sliderValue) / 100.0f) * (maxValue - minValue);

	return realValue;
}

void SettingsDialog::RealValueToSlider(
	float value,
	float minValue,
	float maxValue,
	wxSlider * slider) const
{
	int sliderValue = static_cast<int>(roundf((value - minValue) / (maxValue - minValue) * 100.0f));

	slider->SetValue(sliderValue);
}
