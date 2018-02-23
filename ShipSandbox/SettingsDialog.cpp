/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "SettingsDialog.h"

#include <GameLib/Log.h>

#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>

static constexpr int SliderTicks = 100;

const long ID_STRENGTH_SLIDER = wxNewId();
const long ID_BUOYANCY_SLIDER = wxNewId();
const long ID_WATER_PRESSURE_SLIDER = wxNewId();
const long ID_WAVE_HEIGHT_SLIDER = wxNewId();
const long ID_WATER_TRANSPARENCY_SLIDER = wxNewId();
const long ID_LIGHT_DIFFUSION_SLIDER = wxNewId();
const long ID_SEA_DEPTH_SLIDER = wxNewId();
const long ID_DESTROY_RADIUS_SLIDER = wxNewId();

const long ID_QUICK_WATER_FIX_CHECKBOX = wxNewId();
const long ID_SHOW_STRESS_CHECKBOX = wxNewId();
const long ID_XRAY_CHECKBOX = wxNewId();
const long ID_DRAW_POINTS_ONLY_CHECKBOX = wxNewId();

wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
	EVT_COMMAND_SCROLL(ID_STRENGTH_SLIDER, SettingsDialog::OnStrengthSliderScroll)
	EVT_COMMAND_SCROLL(ID_BUOYANCY_SLIDER, SettingsDialog::OnBuoyancySliderScroll)
	EVT_COMMAND_SCROLL(ID_WATER_PRESSURE_SLIDER, SettingsDialog::OnWaterPressureSliderScroll)
	EVT_COMMAND_SCROLL(ID_WAVE_HEIGHT_SLIDER, SettingsDialog::OnWaveHeightSliderScroll)
    EVT_COMMAND_SCROLL(ID_WATER_TRANSPARENCY_SLIDER, SettingsDialog::OnWaterTransparencySliderScroll)
    EVT_COMMAND_SCROLL(ID_LIGHT_DIFFUSION_SLIDER, SettingsDialog::OnLightDiffusionSliderScroll)
	EVT_COMMAND_SCROLL(ID_SEA_DEPTH_SLIDER, SettingsDialog::OnSeaDepthSliderScroll)
	EVT_COMMAND_SCROLL(ID_DESTROY_RADIUS_SLIDER, SettingsDialog::OnDestroyRadiusSliderScroll)
wxEND_EVENT_TABLE()

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
	mStrengthSlider = new wxSlider(this, ID_STRENGTH_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Strength Slider"));
	mStrengthSlider->SetTickFreq(4);
	strengthSizer->Add(mStrengthSlider, 0, wxALIGN_CENTRE);

	wxStaticText * strengthLabel = new wxStaticText(this, wxID_ANY, _("Strength Adjust"), wxDefaultPosition, wxDefaultSize, 0, _T("Strength Label"));	
	strengthSizer->Add(strengthLabel, 0, wxALIGN_CENTRE);

	mStrengthTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	strengthSizer->Add(mStrengthTextCtrl, 0, wxALIGN_CENTRE);

	strengthSizer->AddSpacer(20);

	controlsSizer->Add(strengthSizer, 0);
	
	controlsSizer->AddSpacer(20);


	// Buoyancy
	
	wxBoxSizer* buoyancySizer = new wxBoxSizer(wxVERTICAL);

	mBuoyancySlider = new wxSlider(this, ID_BUOYANCY_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Buoyancy Slider"));
	mBuoyancySlider->SetTickFreq(4);
	buoyancySizer->Add(mBuoyancySlider, 0, wxALIGN_CENTRE);

	wxStaticText * buoyancyLabel = new wxStaticText(this, wxID_ANY, _("Buoyancy Adjust"), wxDefaultPosition, wxDefaultSize, 0, _T("Buoyancy Label"));
	buoyancySizer->Add(buoyancyLabel, 0, wxALIGN_CENTRE);
	
	mBuoyancyTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	buoyancySizer->Add(mBuoyancyTextCtrl, 0, wxALIGN_CENTRE);

	buoyancySizer->AddSpacer(20);

	controlsSizer->Add(buoyancySizer, 0);
	
    controlsSizer->AddSpacer(20);


	// Water Pressure

	wxBoxSizer* waterPressureSizer = new wxBoxSizer(wxVERTICAL);

	mWaterPressureSlider = new wxSlider(this, ID_WATER_PRESSURE_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Water Pressure Slider"));
	mWaterPressureSlider->SetTickFreq(4);
	waterPressureSizer->Add(mWaterPressureSlider, 0, wxALIGN_CENTRE);

	wxStaticText * waterPressureLabel = new wxStaticText(this, wxID_ANY, _("Water Pressure Adjust"), wxDefaultPosition, wxDefaultSize, 0, _T("Water Pressure Label"));
	waterPressureSizer->Add(waterPressureLabel, 0, wxALIGN_CENTRE);

	mWaterPressureTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	waterPressureSizer->Add(mWaterPressureTextCtrl, 0, wxALIGN_CENTRE);

	waterPressureSizer->AddSpacer(20);

	controlsSizer->Add(waterPressureSizer, 0);

    controlsSizer->AddSpacer(20);


	// Wave Height

	wxBoxSizer* waveHeightSizer = new wxBoxSizer(wxVERTICAL);

	mWaveHeightSlider = new wxSlider(this, ID_WAVE_HEIGHT_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Wave Height Slider"));
	mWaveHeightSlider->SetTickFreq(4);
	waveHeightSizer->Add(mWaveHeightSlider, 0, wxALIGN_CENTRE);

	wxStaticText * waveHeightLabel = new wxStaticText(this, wxID_ANY, _("Wave Height"), wxDefaultPosition, wxDefaultSize, 0, _T("Wave Height Label"));
	waveHeightSizer->Add(waveHeightLabel, 0, wxALIGN_CENTRE);

	mWaveHeightTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	waveHeightSizer->Add(mWaveHeightTextCtrl, 0, wxALIGN_CENTRE);

	waveHeightSizer->AddSpacer(20);

	controlsSizer->Add(waveHeightSizer, 0);

    controlsSizer->AddSpacer(20);

    
    // Water Transparency

    wxBoxSizer* waterTransparencySizer = new wxBoxSizer(wxVERTICAL);

    mWaterTransparencySlider = new wxSlider(this, ID_WATER_TRANSPARENCY_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
        wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Water Transparency Slider"));
    mWaterTransparencySlider->SetTickFreq(4);
    waterTransparencySizer->Add(mWaterTransparencySlider, 0, wxALIGN_CENTRE);

    wxStaticText * waterTransparencyLabel = new wxStaticText(this, wxID_ANY, _("Water Transparency"), wxDefaultPosition, wxDefaultSize, 0, _T("Water Transparency Label"));
    waterTransparencySizer->Add(waterTransparencyLabel, 0, wxALIGN_CENTRE);

    mWaterTransparencyTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    waterTransparencySizer->Add(mWaterTransparencyTextCtrl, 0, wxALIGN_CENTRE);

    waterTransparencySizer->AddSpacer(20);

    controlsSizer->Add(waterTransparencySizer, 0);

    controlsSizer->AddSpacer(20);


    // Light Diffusion

    wxBoxSizer* lightDiffusionSizer = new wxBoxSizer(wxVERTICAL);

    mLightDiffusionSlider = new wxSlider(this, ID_LIGHT_DIFFUSION_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
        wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Light Diffusion Slider"));
    mLightDiffusionSlider->SetTickFreq(4);
    lightDiffusionSizer->Add(mLightDiffusionSlider, 0, wxALIGN_CENTRE);

    wxStaticText * lightDiffusionLabel = new wxStaticText(this, wxID_ANY, _("Light Diffusion Adjust"), wxDefaultPosition, wxDefaultSize, 0, _T("Light Diffusion Label"));
    lightDiffusionSizer->Add(lightDiffusionLabel, 0, wxALIGN_CENTRE);

    mLightDiffusionTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    lightDiffusionSizer->Add(mLightDiffusionTextCtrl, 0, wxALIGN_CENTRE);

    lightDiffusionSizer->AddSpacer(20);

    controlsSizer->Add(lightDiffusionSizer, 0);

    controlsSizer->AddSpacer(20);


	// Sea Depth

	wxBoxSizer* seaDepthSizer = new wxBoxSizer(wxVERTICAL);

	mSeaDepthSlider = new wxSlider(this, ID_SEA_DEPTH_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Sea Depth Slider"));
	mSeaDepthSlider->SetTickFreq(4);
	seaDepthSizer->Add(mSeaDepthSlider, 0, wxALIGN_CENTRE);

	wxStaticText * seaDepthLabel = new wxStaticText(this, wxID_ANY, _("Ocean Depth"), wxDefaultPosition, wxDefaultSize, 0, _T("Sea Depth Label"));
	seaDepthSizer->Add(seaDepthLabel, 0, wxALIGN_CENTRE);

	mSeaDepthTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	seaDepthSizer->Add(mSeaDepthTextCtrl, 0, wxALIGN_CENTRE);

	seaDepthSizer->AddSpacer(20);

	controlsSizer->Add(seaDepthSizer, 0);

    controlsSizer->AddSpacer(20);


	// Destroy Radius

	wxBoxSizer* destroyRadiusSizer = new wxBoxSizer(wxVERTICAL);

	mDestroyRadiusSlider = new wxSlider(this, ID_DESTROY_RADIUS_SLIDER, 50, 0, SliderTicks, wxDefaultPosition, wxSize(50, 200),
		wxSL_VERTICAL | wxSL_LEFT | wxSL_INVERSE | wxSL_AUTOTICKS, wxDefaultValidator, _T("Destroy Radius Slider"));
	mDestroyRadiusSlider->SetTickFreq(4);
	destroyRadiusSizer->Add(mDestroyRadiusSlider, 0, wxALIGN_CENTRE);

	wxStaticText * destroyRadiusLabel = new wxStaticText(this, wxID_ANY, _("Destroy Radius"), wxDefaultPosition, wxDefaultSize, 0, _T("Destroy Radius Label"));
	destroyRadiusSizer->Add(destroyRadiusLabel, 0, wxALIGN_CENTRE);

	mDestroyRadiusTextCtrl = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
	destroyRadiusSizer->Add(mDestroyRadiusTextCtrl, 0, wxALIGN_CENTRE);

	destroyRadiusSizer->AddSpacer(20);

	controlsSizer->Add(destroyRadiusSizer, 0);

    controlsSizer->AddSpacer(20);

	
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

    mDrawPointsOnlyCheckBox = new wxCheckBox(this, ID_DRAW_POINTS_ONLY_CHECKBOX, _("Draw Only Points"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("Draw Only Points Checkbox"));
    Connect(ID_DRAW_POINTS_ONLY_CHECKBOX, wxEVT_COMMAND_CHECKBOX_CLICKED, (wxObjectEventFunction)&SettingsDialog::OnDrawPointsOnlyCheckBoxClick);
    checkboxesSizer->Add(mDrawPointsOnlyCheckBox, 0, wxALL | wxALIGN_LEFT, 5);

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
		mGameController->GetMinStrengthAdjustment(),
		mGameController->GetMaxStrengthAdjustment());

	mStrengthTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnBuoyancySliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mBuoyancySlider,
		mGameController->GetMinBuoyancyAdjustment(),
		mGameController->GetMaxBuoyancyAdjustment());

	mBuoyancyTextCtrl->SetValue(std::to_string(realValue));

	// Remember we're dirty now
	mApplyButton->Enable(true);
}

void SettingsDialog::OnWaterPressureSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mWaterPressureSlider,
		mGameController->GetMinWaterPressureAdjustment(),
		mGameController->GetMaxWaterPressureAdjustment());

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

void SettingsDialog::OnWaterTransparencySliderScroll(wxScrollEvent & /*event*/)
{
    assert(!!mGameController);

    float realValue = SliderToRealValue(
        mWaterTransparencySlider,
        0.0f,
        1.0f);

    mWaterTransparencyTextCtrl->SetValue(std::to_string(realValue));

    // Remember we're dirty now
    mApplyButton->Enable(true);
}

void SettingsDialog::OnLightDiffusionSliderScroll(wxScrollEvent & /*event*/)
{
    assert(!!mGameController);

    float realValue = SliderToRealValue(
        mLightDiffusionSlider,
        0.0f,
        1.0f);

    mLightDiffusionTextCtrl->SetValue(std::to_string(realValue));

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

void SettingsDialog::OnDestroyRadiusSliderScroll(wxScrollEvent & /*event*/)
{
	assert(!!mGameController);

	float realValue = SliderToRealValue(
		mDestroyRadiusSlider,
		mGameController->GetMinDestroyRadius(),
		mGameController->GetMaxDestroyRadius());

	mDestroyRadiusTextCtrl->SetValue(std::to_string(realValue));

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

void SettingsDialog::OnDrawPointsOnlyCheckBoxClick(wxCommandEvent & /*event*/)
{
    OnDrawPointsOnlyChanged();

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

	mGameController->SetStrengthAdjustment(
		SliderToRealValue(
			mStrengthSlider,
			mGameController->GetMinStrengthAdjustment(),
			mGameController->GetMaxStrengthAdjustment()));

	mGameController->SetBuoyancyAdjustment(
		SliderToRealValue(
			mBuoyancySlider,
			mGameController->GetMinBuoyancyAdjustment(),
			mGameController->GetMaxBuoyancyAdjustment()));

	mGameController->SetWaterPressureAdjustment(
		SliderToRealValue(
			mWaterPressureSlider,
			mGameController->GetMinWaterPressureAdjustment(),
			mGameController->GetMaxWaterPressureAdjustment()));

	mGameController->SetWaveHeight(
		SliderToRealValue(
			mWaveHeightSlider,
			mGameController->GetMinWaveHeight(),
			mGameController->GetMaxWaveHeight()));

    mGameController->SetWaterTransparency(
        SliderToRealValue(
            mWaterTransparencySlider,
            0.0f,
            1.0f));

    mGameController->SetLightDiffusionAdjustment(
        SliderToRealValue(
            mLightDiffusionSlider,
            0.0f,
            1.0f));

	mGameController->SetSeaDepth(
		SliderToRealValue(
			mSeaDepthSlider,
			mGameController->GetMinSeaDepth(),
			mGameController->GetMaxSeaDepth()));

	mGameController->SetDestroyRadius(
		SliderToRealValue(
			mDestroyRadiusSlider,
			mGameController->GetMinDestroyRadius(),
			mGameController->GetMaxDestroyRadius()));

	mGameController->SetShowShipThroughWater(mQuickWaterFixCheckBox->IsChecked());

	mGameController->SetShowStress(mShowStressCheckBox->IsChecked());

	mGameController->SetUseXRayMode(mXRayCheckBox->IsChecked());

    mGameController->SetDrawPointsOnly(mDrawPointsOnlyCheckBox->IsChecked());
}

void SettingsDialog::ReadSettings()
{
	assert(!!mGameController);

	RealValueToSlider(
		mGameController->GetStrengthAdjustment(),
		mGameController->GetMinStrengthAdjustment(),
		mGameController->GetMaxStrengthAdjustment(),
		mStrengthSlider);

	mStrengthTextCtrl->SetValue(std::to_string(mGameController->GetStrengthAdjustment()));


	RealValueToSlider(
		mGameController->GetBuoyancyAdjustment(),
		mGameController->GetMinBuoyancyAdjustment(),
		mGameController->GetMaxBuoyancyAdjustment(),
		mBuoyancySlider);

	mBuoyancyTextCtrl->SetValue(std::to_string(mGameController->GetBuoyancyAdjustment()));


	RealValueToSlider(
		mGameController->GetWaterPressureAdjustment(),
		mGameController->GetMinWaterPressureAdjustment(),
		mGameController->GetMaxWaterPressureAdjustment(),
		mWaterPressureSlider);

	mWaterPressureTextCtrl->SetValue(std::to_string(mGameController->GetWaterPressureAdjustment()));


	RealValueToSlider(
		mGameController->GetWaveHeight(),
		mGameController->GetMinWaveHeight(),
		mGameController->GetMaxWaveHeight(),
		mWaveHeightSlider);

	mWaveHeightTextCtrl->SetValue(std::to_string(mGameController->GetWaveHeight()));


    RealValueToSlider(
        mGameController->GetWaterTransparency(),
        0.0f,
        1.0f,
        mWaterTransparencySlider);

    mWaterTransparencyTextCtrl->SetValue(std::to_string(mGameController->GetWaterTransparency()));


    RealValueToSlider(
        mGameController->GetLightDiffusionAdjustment(),
        0.0f,
        1.0f,
        mLightDiffusionSlider);

    mLightDiffusionTextCtrl->SetValue(std::to_string(mGameController->GetLightDiffusionAdjustment()));


	RealValueToSlider(
		mGameController->GetSeaDepth(),
		mGameController->GetMinSeaDepth(),
		mGameController->GetMaxSeaDepth(),
		mSeaDepthSlider);

	mSeaDepthTextCtrl->SetValue(std::to_string(mGameController->GetSeaDepth()));


	RealValueToSlider(
		mGameController->GetDestroyRadius(),
		mGameController->GetMinDestroyRadius(),
		mGameController->GetMaxDestroyRadius(),
		mDestroyRadiusSlider);

	mDestroyRadiusTextCtrl->SetValue(std::to_string(mGameController->GetDestroyRadius()));


	mQuickWaterFixCheckBox->SetValue(mGameController->GetShowShipThroughWater());

	mShowStressCheckBox->SetValue(mGameController->GetShowStress());

	mXRayCheckBox->SetValue(mGameController->GetUseXRayMode());

    mDrawPointsOnlyCheckBox->SetValue(mGameController->GetDrawPointsOnly());

    OnDrawPointsOnlyChanged();
}

float SettingsDialog::SliderToRealValue(
	wxSlider * const slider,
	float minValue,
	float maxValue) const
{
	int sliderValue = slider->GetValue();
	float realValue = minValue + (static_cast<float>(sliderValue) / static_cast<float>(SliderTicks)) * (maxValue - minValue);

	return realValue;
}

void SettingsDialog::RealValueToSlider(
	float value,
	float minValue,
	float maxValue,
	wxSlider * slider) const
{
    int sliderValue = static_cast<int>(roundf((value - minValue) / (maxValue - minValue) * static_cast<float>(SliderTicks)));

	slider->SetValue(sliderValue);
}

void SettingsDialog::OnDrawPointsOnlyChanged()
{
    if (mDrawPointsOnlyCheckBox->IsChecked())
    {
        mShowStressCheckBox->Enable(false);
        mXRayCheckBox->Enable(false);
    }
    else
    {
        mShowStressCheckBox->Enable(true);
        mXRayCheckBox->Enable(true);
    }
}