#include "profile_header.h"
#include "profile_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"

wxDECLARE_APP(App);

ProfileHeader::ProfileHeader(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE) {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.primary);
	this->SetCursor(wxCursor(wxCURSOR_HAND));

	this->badge = new ProfileBadge(this);
	this->badge->SetColours(theme.color.primary, theme.color.fill, theme.color.label);
	this->badge->SetCursor(wxCursor(wxCURSOR_HAND));

	this->nameLabel = new wxStaticText(this, wxID_ANY, "Profile", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
	wxFont font = this->nameLabel->GetFont();
	font.SetPointSize(font.GetPointSize() + 1);
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	this->nameLabel->SetFont(font);
	this->nameLabel->SetForegroundColour(theme.color.onPrimary);
	this->nameLabel->SetBackgroundColour(theme.color.primary);
	this->nameLabel->SetCursor(wxCursor(wxCURSOR_HAND));

	wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
	row->Add(this->badge, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, theme.space.sm);
	row->Add(this->nameLabel, 1, wxALIGN_CENTER_VERTICAL);
	this->SetSizer(row);

	this->BindClick(this);
	this->Load();
}

void ProfileHeader::Load() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse response = useCase.Execute();
	if (!response.ok) {
		return;
	}
	this->badge->SetInitials(wxString(response.initials));
	this->nameLabel->SetLabel(wxString(response.headerName));
	this->Layout();
}

void ProfileHeader::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.primary);
	this->nameLabel->SetForegroundColour(theme.color.onPrimary);
	this->nameLabel->SetBackgroundColour(theme.color.primary);
	this->badge->SetColours(theme.color.primary, theme.color.fill, theme.color.label);
	this->Refresh();
}

void ProfileHeader::BindClick(wxWindow* window) {
	window->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent&) { this->Open(); });
	for (wxWindow* child : window->GetChildren()) {
		this->BindClick(child);
	}
}

void ProfileHeader::Open() {
	auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
	GetUserProfileResponse response = useCase.Execute();
	if (!response.ok) {
		ShowCenteredMessage(this, "Could not load your profile.", "Profile", wxOK | wxICON_ERROR);
		return;
	}
	ProfileDialog dialog(this, response);
	if (dialog.ShowModal() == wxID_OK) {
		this->Load();
	}
}
