#include "onboarding_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/Profile/CompleteOnboarding/complete_onboarding_usecase.h"

wxDECLARE_APP(App);

namespace {
wxPanel* MakePage(wxWindow* parent) {
	wxPanel* page = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	page->SetBackgroundColour(Theme::Get().color.window);
	return page;
}

wxStaticText* MakeTitle(wxWindow* parent, const wxString& label, int extraPoints) {
	wxStaticText* text = new wxStaticText(parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	wxFont font = text->GetFont();
	font.SetPointSize(font.GetPointSize() + extraPoints);
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	text->SetFont(font);
	text->SetForegroundColour(Theme::Get().color.label);
	text->SetBackgroundColour(Theme::Get().color.window);
	return text;
}

wxStaticText* MakeBody(wxWindow* parent, const wxString& label) {
	wxStaticText* text = new wxStaticText(parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	text->SetForegroundColour(Theme::Get().color.secondaryLabel);
	text->SetBackgroundColour(Theme::Get().color.window);
	return text;
}
}

OnboardingDialog::OnboardingDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "Merken", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
	  finished(false) {
	const int gapSm = this->FromDIP(8);
	const int gapMd = this->FromDIP(12);
	const int gapLg = this->FromDIP(16);
	const int gapXl = this->FromDIP(24);
	const int gapWelcome = this->FromDIP(28);

	this->welcomePanel = MakePage(this);
	wxStaticText* welcomeTitle = MakeTitle(this->welcomePanel, "Welcome to Merken", 14);
	wxStaticText* welcomeBody = MakeBody(this->welcomePanel, "Your companion flash-card study app.");
	wxButton* continueButton = new wxButton(this->welcomePanel, wxID_ANY, "Continue");
	ApplyButtonPadding(continueButton);
	continueButton->SetDefault();
	wxBoxSizer* welcome = new wxBoxSizer(wxVERTICAL);
	welcome->AddStretchSpacer(1);
	welcome->Add(welcomeTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	welcome->Add(welcomeBody, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, gapMd);
	welcome->Add(continueButton, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, gapWelcome);
	welcome->AddStretchSpacer(1);
	this->welcomePanel->SetSizer(welcome);

	this->namePanel = MakePage(this);
	wxStaticText* nameTitle = MakeTitle(this->namePanel, "What should we call you?", 4);
	this->nameCtrl = new wxTextCtrl(this->namePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, this->FromDIP(wxSize(280, -1)));
	wxButton* nameContinue = new wxButton(this->namePanel, wxID_ANY, "Continue");
	wxButton* nameSkip = new wxButton(this->namePanel, wxID_ANY, "Skip");
	ApplyButtonPadding(nameContinue);
	ApplyButtonPadding(nameSkip);
	nameContinue->SetDefault();
	wxBoxSizer* nameButtons = new wxBoxSizer(wxHORIZONTAL);
	nameButtons->Add(nameSkip, 0, wxRIGHT, gapSm);
	nameButtons->Add(nameContinue, 0);
	wxBoxSizer* name = new wxBoxSizer(wxVERTICAL);
	name->AddStretchSpacer(1);
	name->Add(nameTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	name->Add(this->nameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, gapLg);
	name->Add(nameButtons, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, gapXl);
	name->AddStretchSpacer(1);
	this->namePanel->SetSizer(name);

	this->goalPanel = MakePage(this);
	wxStaticText* goalTitle = MakeTitle(this->goalPanel, "How many cards do you want to review each day?", 3);
	wxButton* goal10 = new wxButton(this->goalPanel, 10, "10 cards");
	wxButton* goal20 = new wxButton(this->goalPanel, 20, "20 cards");
	wxButton* goal30 = new wxButton(this->goalPanel, 30, "30 cards");
	wxButton* goalSkip = new wxButton(this->goalPanel, wxID_ANY, "Skip");
	ApplyButtonPadding(goal10);
	ApplyButtonPadding(goal20);
	ApplyButtonPadding(goal30);
	ApplyButtonPadding(goalSkip);
	wxBoxSizer* goalChoices = new wxBoxSizer(wxHORIZONTAL);
	goalChoices->Add(goal10, 0, wxRIGHT, gapSm);
	goalChoices->Add(goal20, 0, wxRIGHT, gapSm);
	goalChoices->Add(goal30, 0);
	wxBoxSizer* goal = new wxBoxSizer(wxVERTICAL);
	goal->AddStretchSpacer(1);
	goal->Add(goalTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	goal->Add(goalChoices, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, this->FromDIP(20));
	goal->Add(goalSkip, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, gapLg);
	goal->AddStretchSpacer(1);
	this->goalPanel->SetSizer(goal);

	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(this->welcomePanel, 1, wxEXPAND | wxALL, gapXl);
	root->Add(this->namePanel, 1, wxEXPAND | wxALL, gapXl);
	root->Add(this->goalPanel, 1, wxEXPAND | wxALL, gapXl);
	this->SetSizer(root);
	this->SetMinSize(this->FromDIP(wxSize(520, 360)));
	Theme::Get().StyleDialog(this);
	this->ShowStep(Step::Welcome);

	continueButton->Bind(wxEVT_BUTTON, &OnboardingDialog::OnWelcomeContinue, this);
	nameContinue->Bind(wxEVT_BUTTON, &OnboardingDialog::OnNameContinue, this);
	nameSkip->Bind(wxEVT_BUTTON, &OnboardingDialog::OnNameSkip, this);
	goal10->Bind(wxEVT_BUTTON, &OnboardingDialog::OnGoalChosen, this);
	goal20->Bind(wxEVT_BUTTON, &OnboardingDialog::OnGoalChosen, this);
	goal30->Bind(wxEVT_BUTTON, &OnboardingDialog::OnGoalChosen, this);
	goalSkip->Bind(wxEVT_BUTTON, &OnboardingDialog::OnGoalSkip, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &OnboardingDialog::OnClose, this);
	this->Bind(wxEVT_SHOW, [this](wxShowEvent& event) {
		event.Skip();
		if (!event.IsShown()) {
			return;
		}
		const Step step = this->goalPanel->IsShown()
			? Step::Goal
			: (this->namePanel->IsShown() ? Step::Name : Step::Welcome);
		this->ApplyChrome(step);
	});
}

void OnboardingDialog::ShowStep(Step step) {
	this->welcomePanel->Show(step == Step::Welcome);
	this->namePanel->Show(step == Step::Name);
	this->goalPanel->Show(step == Step::Goal);
	const bool roomy = step == Step::Welcome || step == Step::Name;
	const wxSize size = roomy ? wxSize(800, 600) : wxSize(560, 400);
	this->ApplyChrome(step);
	this->SetSize(this->FromDIP(size));
	this->Layout();
	this->Centre();
	this->ApplyChrome(step);
}

void OnboardingDialog::ApplyChrome(Step step) {
	const bool titled = step == Step::Goal;
	this->SetTitle(titled ? "Merken" : wxString());
	this->SetBackgroundColour(Theme::Get().color.window);
}

void OnboardingDialog::Finish() {
	if (this->finished) {
		return;
	}
	auto useCase = wxGetApp().GetInjector().create<CompleteOnboardingUseCase>();
	if (!useCase.Execute(CompleteOnboardingRequest{this->displayName, this->dailyGoal})) {
		ShowCenteredMessage(this, "Could not save your profile.", "Merken", wxOK | wxICON_ERROR);
		return;
	}
	this->finished = true;
	this->EndModal(wxID_OK);
}

void OnboardingDialog::OnWelcomeContinue(wxCommandEvent&) {
	this->ShowStep(Step::Name);
}

void OnboardingDialog::OnNameContinue(wxCommandEvent&) {
	this->displayName = this->nameCtrl->GetValue().ToStdString();
	this->ShowStep(Step::Goal);
}

void OnboardingDialog::OnNameSkip(wxCommandEvent&) {
	this->displayName.clear();
	this->dailyGoal = std::nullopt;
	this->Finish();
}

void OnboardingDialog::OnGoalChosen(wxCommandEvent& event) {
	this->dailyGoal = event.GetId();
	this->Finish();
}

void OnboardingDialog::OnGoalSkip(wxCommandEvent&) {
	this->dailyGoal = std::nullopt;
	this->Finish();
}

void OnboardingDialog::OnClose(wxCloseEvent& event) {
	if (!this->finished) {
		auto useCase = wxGetApp().GetInjector().create<CompleteOnboardingUseCase>();
		if (!useCase.Execute(CompleteOnboardingRequest{this->displayName, this->dailyGoal})) {
			ShowCenteredMessage(this, "Could not save your profile.", "Merken", wxOK | wxICON_ERROR);
			event.Veto();
			return;
		}
		this->finished = true;
	}
	event.Skip();
}
