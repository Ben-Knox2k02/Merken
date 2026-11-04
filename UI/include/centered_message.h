#ifndef CENTERED_MESSAGE_H
#define CENTERED_MESSAGE_H

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/display.h>
#include "theme.h"

inline void PlaceClearOf(wxDialog& dialog, wxWindow* avoid, wxWindow* avoidAlso) {
	if (avoid == nullptr) {
		dialog.CentreOnParent();
		return;
	}

	wxRect target = avoid->GetScreenRect();
	if (avoidAlso != nullptr && avoidAlso->IsShown()) {
		target.Union(avoidAlso->GetScreenRect());
	}
	const wxSize box = dialog.GetSize();
	const int gap = avoid->FromDIP(16);
	const wxRect screen = wxDisplay(avoid).GetClientArea();

	auto clampPoint = [&](wxPoint pos) {
		const int maxX = screen.GetRight() - box.GetWidth();
		const int maxY = screen.GetBottom() - box.GetHeight();
		if (pos.x < screen.GetLeft()) { pos.x = screen.GetLeft(); }
		if (pos.y < screen.GetTop()) { pos.y = screen.GetTop(); }
		if (pos.x > maxX) { pos.x = maxX; }
		if (pos.y > maxY) { pos.y = maxY; }
		return pos;
	};
	auto overlap = [&](wxPoint pos) {
		const wxRect placed(pos, box);
		const wxRect shared = placed.Intersect(target);
		if (shared.IsEmpty()) {
			return 0;
		}
		return shared.GetWidth() * shared.GetHeight();
	};

	const wxPoint candidates[] = {
		clampPoint(wxPoint(target.GetLeft(), target.GetTop() - box.GetHeight() - gap)),
		clampPoint(wxPoint(target.GetLeft(), target.GetBottom() + gap)),
		clampPoint(wxPoint(target.GetRight() + gap, target.GetTop())),
		clampPoint(wxPoint(target.GetLeft() - box.GetWidth() - gap, target.GetTop()))
	};
	wxPoint best = candidates[0];
	int bestOverlap = overlap(best);
	for (const wxPoint& candidate : candidates) {
		const int area = overlap(candidate);
		if (area < bestOverlap) {
			best = candidate;
			bestOverlap = area;
		}
	}
	dialog.SetPosition(best);
}

inline void SizeAsThirdOfMainWindow(
	wxDialog* dialog,
	int widthNum = 1,
	int widthDen = 1,
	int heightNum = 1,
	int heightDen = 1
) {
	wxWindow* mainWindow = dialog->GetParent();
	if (mainWindow == nullptr || widthDen < 1 || heightDen < 1) {
		return;
	}
	const wxSize mainSize = mainWindow->GetSize();
	const int width = (mainSize.GetWidth() / 3) * widthNum / widthDen;
	const int height = (mainSize.GetHeight() / 3) * heightNum / heightDen;
	if (width < 1 || height < 1) {
		return;
	}
	const wxSize size(width, height);
	dialog->SetMinSize(size);
	dialog->SetSize(size);
	dialog->Layout();
	dialog->CentreOnParent();
}

inline int ShowCenteredMessage(
	wxWindow* parent,
	const wxString& message,
	const wxString& caption,
	long style = wxOK | wxICON_INFORMATION,
	wxWindow* avoid = nullptr,
	wxWindow* avoidAlso = nullptr,
	bool thirdOfMain = false
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

	wxStaticBitmap* icon = new wxStaticBitmap(
		&dialog,
		wxID_ANY,
		wxArtProvider::GetBitmap(artId, wxART_MESSAGE_BOX)
	);
	wxBoxSizer* content = new wxBoxSizer(wxHORIZONTAL);
	content->Add(icon, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

	wxStaticText* text = new wxStaticText(&dialog, wxID_ANY, message);
	content->Add(text, 1, wxALIGN_CENTER_VERTICAL);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	if (style & wxYES_NO) {
		wxButton* yesButton = new wxButton(&dialog, wxID_YES);
		wxButton* noButton = new wxButton(&dialog, wxID_NO);
		ApplyButtonPadding(yesButton);
		ApplyButtonPadding(noButton);
		buttons->AddButton(yesButton);
		buttons->AddButton(noButton);
		dialog.SetAffirmativeId(wxID_YES);
		dialog.SetEscapeId(wxID_NO);
	} else {
		wxButton* okButton = new wxButton(&dialog, wxID_OK);
		ApplyButtonPadding(okButton);
		buttons->AddButton(okButton);
	}
	buttons->Realize();

	dialog.Bind(wxEVT_BUTTON, [&dialog](wxCommandEvent& event) {
		const int id = event.GetId();
		if (id == wxID_OK || id == wxID_CANCEL || id == wxID_YES || id == wxID_NO) {
			dialog.EndModal(id);
			return;
		}
		event.Skip();
	});

	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(content, 1, wxEXPAND | wxALL, 16);
	root->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	dialog.SetSizer(root);
	Theme::Get().StyleDialog(&dialog);
	if (thirdOfMain) {
		SizeAsThirdOfMainWindow(&dialog, 2, 3, 1, 2);
		const int wrapWidth = dialog.GetClientSize().GetWidth()
			- dialog.FromDIP(16) * 2
			- dialog.FromDIP(12)
			- icon->GetBestSize().GetWidth();
		if (wrapWidth > 0) {
			text->Wrap(wrapWidth);
		}
		dialog.Layout();
		return dialog.ShowModal();
	}
	text->Wrap(360);
	dialog.SetMinSize(wxSize(420, 140));
	dialog.Fit();
	dialog.Layout();
	PlaceClearOf(dialog, avoid, avoidAlso);
	return dialog.ShowModal();
}

#endif

