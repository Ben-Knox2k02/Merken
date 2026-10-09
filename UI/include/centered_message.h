#ifndef CENTERED_MESSAGE_H
#define CENTERED_MESSAGE_H

#include <wx/wx.h>
#include <wx/artprov.h>

inline int ShowCenteredMessage(
	wxWindow* parent,
	const wxString& message,
	const wxString& caption,
	long style = wxOK | wxICON_INFORMATION
) {
	wxWindow* owner = wxGetTopLevelParent(parent);
	wxDialog dialog(
		owner,
		wxID_ANY,
		caption,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	);

	wxString artId = wxART_INFORMATION;
	if (style & wxICON_ERROR) { artId = wxART_ERROR; }
	else if (style & wxICON_WARNING) { artId = wxART_WARNING; }
	else if (style & wxICON_QUESTION) { artId = wxART_QUESTION; }

	wxBoxSizer* content = new wxBoxSizer(wxHORIZONTAL);
	content->Add(
		new wxStaticBitmap(&dialog, wxID_ANY, wxArtProvider::GetBitmap(artId, wxART_MESSAGE_BOX)),
		0,
		wxALIGN_CENTER_VERTICAL | wxRIGHT,
		12
	);

	wxStaticText* text = new wxStaticText(&dialog, wxID_ANY, message);
	text->Wrap(360);
	content->Add(text, 1, wxALIGN_CENTER_VERTICAL);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	if (style & wxYES_NO) {
		buttons->AddButton(new wxButton(&dialog, wxID_YES));
		buttons->AddButton(new wxButton(&dialog, wxID_NO));
	} else {
		buttons->AddButton(new wxButton(&dialog, wxID_OK));
	}
	buttons->Realize();

	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(content, 1, wxEXPAND | wxALL, 16);
	root->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	dialog.SetSizer(root);
	dialog.SetMinSize(wxSize(420, 140));
	dialog.Fit();
	dialog.Layout();
	dialog.CentreOnParent();
	return dialog.ShowModal();
}

#endif

