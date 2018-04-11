/***************************************************************************************
* Original Author:		Gabriele Giuseppini
* Created:				2018-03-12
* Copyright:			Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "AboutDialog.h"

#include "Version.h"

#include <wx/generic/statbmpg.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

wxBEGIN_EVENT_TABLE(AboutDialog, wxDialog)
	EVT_CLOSE(AboutDialog::OnClose)
wxEND_EVENT_TABLE()

AboutDialog::AboutDialog(
    wxWindow * parent,
    ResourceLoader const & resourceLoader)
	: mParent(parent)
{
	Create(
		mParent,
		wxID_ANY,
		_("About " + GetVersionInfo(VersionFormat::Medium)),
		wxDefaultPosition, 
		wxSize(760, 450),
		wxCAPTION | wxCLOSE_BOX | wxFRAME_SHAPED | wxSTAY_ON_TOP,
		_T("About Window"));

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->AddSpacer(5);

    //
    // Title
    //

    wxStaticText * titleLabel = new wxStaticText(this, wxID_ANY, _(""));
    titleLabel->SetLabelText(GetVersionInfo(VersionFormat::Medium));
    titleLabel->SetFont(wxFont(wxFontInfo(14).Family(wxFONTFAMILY_MODERN)));
    mainSizer->Add(titleLabel, 0, wxALIGN_CENTRE);

    mainSizer->AddSpacer(1);

    wxStaticText * title2Label = new wxStaticText(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
    title2Label->SetLabelText("Original Concept (c) Luke Wren 2013\n(c) Gabriele Giuseppini 2018\nThis version licensed to Mattia");
    mainSizer->Add(title2Label, 0, wxALIGN_CENTRE);

    mainSizer->AddSpacer(5);


    //
    // Image
    //

    wxBitmap* bmp = new wxBitmap(resourceLoader.GetArtFilepath("splash_screen").string(), wxBITMAP_TYPE_PNG);

    wxStaticBitmap * stBmp = new wxStaticBitmap(this, wxID_ANY, *bmp, wxDefaultPosition, wxSize(400, 150), wxBORDER_SIMPLE);
    stBmp->SetScaleMode(wxStaticBitmap::Scale_AspectFill);

    mainSizer->Add(stBmp, 0, wxALIGN_CENTER);

    mainSizer->AddSpacer(5);


    //
    // Credits
    //

    wxStaticText * creditsTitleLabel = new wxStaticText(this, wxID_ANY, _("Credits:"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
    creditsTitleLabel->SetFont(wxFont(wxFontInfo(14).Family(wxFONTFAMILY_MODERN)));
    mainSizer->Add(creditsTitleLabel, 0, wxALIGN_CENTRE);

    mainSizer->AddSpacer(2);

    std::vector<std::pair<std::string, std::string>> credits
    {
        {"Covert art:", "Dimitar Katsarov - https://www.artstation.com/stukata/profile" },
        {"Textures:", "Tune 'Prototstar' Katerungroch"},
        {"wxWidgets:", "Copyright (c) 1998-2005 Julian Smart, Robert Roebling et al - https://www.wxwidgets.org/"},
        {"SFML:", "Copyright (c) Laurent Gomila - https://www.sfml-dev.org/"},
        {"DevIL:", "Denton Woods et al - http://openil.sourceforge.net/" },
        {"picojson:", "Copyright 2009-2010 Cybozu Labs, Inc.; Copyright 2011 - 2014 Kazuho Oku - https://github.com/kazuho/picojson"},
        { "OpenGL tutorial:", "Joey de Vries - https://learnopengl.com/" }
    };

    wxFlexGridSizer * creditsSizer = new wxFlexGridSizer(4, 0, 2);
    for (auto const & credit : credits)
    {
        creditsSizer->AddSpacer(5);

        wxStaticText * credits1Label = new wxStaticText(this, wxID_ANY, credit.first, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        creditsSizer->Add(credits1Label, 0, wxALIGN_LEFT);

        creditsSizer->AddSpacer(5);

        wxStaticText * credits2Label = new wxStaticText(this, wxID_ANY, credit.second, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        creditsSizer->Add(credits2Label, 1, wxALIGN_LEFT);
    }

    mainSizer->Add(creditsSizer);


    //
    // Finalize
    //

    SetSizer(mainSizer);

    Centre(wxCENTER_ON_SCREEN | wxBOTH);
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::OnClose(wxCloseEvent & event)
{
	event.Skip();
}

