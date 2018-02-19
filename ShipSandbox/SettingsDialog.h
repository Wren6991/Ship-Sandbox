/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <GameLib/GameController.h>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/slider.h>
#include <wx/textctrl.h>

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
	
	void OnStrengthSliderScroll(wxScrollEvent & event);
	void OnBuoyancySliderScroll(wxScrollEvent & event);
	void OnWaterPressureSliderScroll(wxScrollEvent & event);
	void OnWaveHeightSliderScroll(wxScrollEvent & event);
    void OnWaterTransparencySliderScroll(wxScrollEvent & event);
	void OnSeaDepthSliderScroll(wxScrollEvent & event);
	void OnDestroyRadiusSliderScroll(wxScrollEvent & event);
	void OnQuickWaterFixCheckBoxClick(wxCommandEvent & event);
	void OnShowStressCheckBoxClick(wxCommandEvent & event);
	void OnXRayCheckBoxClick(wxCommandEvent & event);
    void OnDrawPointsOnlyCheckBoxClick(wxCommandEvent & event);

	void OnOkButton(wxCommandEvent & event);
	void OnApplyButton(wxCommandEvent & event);

private:

	// Controls

	wxSlider * mStrengthSlider;
	wxTextCtrl * mStrengthTextCtrl;
	wxSlider * mBuoyancySlider;
	wxTextCtrl * mBuoyancyTextCtrl;
	wxSlider * mWaterPressureSlider;
	wxTextCtrl * mWaterPressureTextCtrl;
	wxSlider * mWaveHeightSlider;
	wxTextCtrl * mWaveHeightTextCtrl;
    wxSlider * mWaterTransparencySlider;
    wxTextCtrl * mWaterTransparencyTextCtrl;
	wxSlider * mSeaDepthSlider;
	wxTextCtrl * mSeaDepthTextCtrl;
	wxSlider * mDestroyRadiusSlider;
	wxTextCtrl * mDestroyRadiusTextCtrl;

	wxCheckBox * mQuickWaterFixCheckBox;
	wxCheckBox* mShowStressCheckBox;
	wxCheckBox* mXRayCheckBox;
    wxCheckBox* mDrawPointsOnlyCheckBox;

	wxButton * mOkButton;
	wxButton * mCancelButton;
	wxButton * mApplyButton;

private:

	void ApplySettings();

	void ReadSettings();

	float SliderToRealValue(
		wxSlider * const slider,
		float minValue,
		float maxValue) const;

	void RealValueToSlider(
		float value,
		float minValue,
		float maxValue,
		wxSlider * slider) const;

    void OnDrawPointsOnlyChanged();

private:

	wxWindow * const mParent;
	std::shared_ptr<GameController> mGameController;

	DECLARE_EVENT_TABLE()
};
