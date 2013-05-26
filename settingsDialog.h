#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//(*Headers(settingsDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/dialog.h>
//*)

class settingsDialog: public wxDialog
{
	public:

		settingsDialog(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~settingsDialog();

		//(*Declarations(settingsDialog)
		wxCheckBox* chkQuickFix;
		wxSlider* sldBuoyancy;
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

		wxWindow *parent;

		DECLARE_EVENT_TABLE()
};

#endif
