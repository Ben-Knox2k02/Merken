#include "profile_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "theme_preference.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Profile/UpdateUserProfile/update_user_profile_usecase.h"

wxDECLARE_APP(App);

namespace {
void AddGoal(wxChoice* choice, std::vector<int>& values, int goal) {
	values.push_back(goal);
	if (goal <= 0) {
		choice->Append("No goal");
		return;
	}
	choice->Append(wxString::Format("%d cards", goal));
}
}

ProfileDialog::ProfileDialog(wxWindow* parent, const GetUserProfileResponse& profile)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		"Edit Profile",
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY, wxString(profile.displayName));
	fields->Add(this->nameCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Note:"), 0, wxALIGN_CENTER_VERTICAL);
	this->noteCtrl = new wxTextCtrl(this, wxID_ANY, wxString(profile.note));
	fields->Add(this->noteCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Daily goal:"), 0, wxALIGN_CENTER_VERTICAL);
	this->goalChoice = new wxChoice(this, wxID_ANY);
	AddGoal(this->goalChoice, this->goalValues, 0);
	AddGoal(this->goalChoice, this->goalValues, 10);
	AddGoal(this->goalChoice, this->goalValues, 20);
	AddGoal(this->goalChoice, this->goalValues, 30);
	int selected = 0;
	if (profile.dailyGoal.has_value()) {
		const int goal = *profile.dailyGoal;
		bool found = false;
		for (size_t i = 0; i < this->goalValues.size(); ++i) {
			if (this->goalValues[i] == goal) {
				selected = static_cast<int>(i);
				found = true;
				break;
			}
		}
		if (!found && goal > 0) {
			AddGoal(this->goalChoice, this->goalValues, goal);
			selected = static_cast<int>(this->goalValues.size() - 1);
		}
	}
	this->goalChoice->SetSelection(selected);
	fields->Add(this->goalChoice, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Theme:"), 0, wxALIGN_CENTER_VERTICAL);
	this->themeChoice = new wxChoice(this, wxID_ANY);
	this->themeChoice->Append("Light");
	this->themeChoice->Append("Dark");
	this->themeChoice->SetSelection(Theme::IsDarkAppearance() ? 1 : 0);
	fields->Add(this->themeChoice, 1, wxEXPAND);
	this->keptUsesAiStudy = profile.usesAiStudy;

	wxButton* okButton = new wxButton(this, wxID_ANY, "OK");
	wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	okButton->SetDefault();
	wxBoxSizer* buttons = new wxBoxSizer(wxHORIZONTAL);
	buttons->AddStretchSpacer(1);
	buttons->Add(okButton, 0, wxRIGHT, 8);
	buttons->Add(cancelButton, 0);

	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(fields, 0, wxEXPAND | wxALL, 16);
	root->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);
	this->SetSizer(root);
	this->SetMinSize(wxSize(480, 240));
	this->SetSize(wxSize(520, 260));
	Theme::Get().StyleDialog(this);
	this->Layout();
	this->CentreOnParent();

	okButton->Bind(wxEVT_BUTTON, &ProfileDialog::OnSave, this);
}

std::optional<int> ProfileDialog::SelectedGoal() const {
	const int index = this->goalChoice->GetSelection();
	if (index < 0 || static_cast<size_t>(index) >= this->goalValues.size()) {
		return std::nullopt;
	}
	const int goal = this->goalValues[static_cast<size_t>(index)];
	if (goal <= 0) {
		return std::nullopt;
	}
	return goal;
}

void ProfileDialog::OnSave(wxCommandEvent&) {
	UpdateUserProfileRequest request;
	request.displayName = this->nameCtrl->GetValue().ToStdString();
	request.note = this->noteCtrl->GetValue().ToStdString();
	request.dailyGoal = this->SelectedGoal();
	request.usesAiStudy = this->keptUsesAiStudy;

	auto useCase = wxGetApp().GetInjector().create<UpdateUserProfileUseCase>();
	if (!useCase.Execute(request)) {
		ShowCenteredMessage(this, "Could not save your profile.", "Edit Profile", wxOK | wxICON_ERROR);
		return;
	}
	const bool dark = this->themeChoice->GetSelection() == 1;
	if (!SaveAndApplyTheme(dark)) {
		ShowCenteredMessage(this, "Could not save the theme.", "Edit Profile", wxOK | wxICON_ERROR);
		return;
	}
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ApplyTheme();
	}
	this->EndModal(wxID_OK);
}
