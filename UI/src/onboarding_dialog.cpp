#include "onboarding_dialog.h"
#include "centered_message.h"
#include "theme.h"
#include "app.h"
#include "../../Application/UseCases/Profile/CompleteOnboarding/complete_onboarding_usecase.h"
#ifdef __WXOSX__
#include <objc/message.h>
#include <objc/runtime.h>
#endif

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
	this->welcomePanel = MakePage(this);
	wxStaticText* welcomeTitle = MakeTitle(this->welcomePanel, "Welcome to Merken", 14);
	wxStaticText* welcomeBody = MakeBody(this->welcomePanel, "Your companion flash-card study app.");
	wxButton* continueButton = new wxButton(this->welcomePanel, wxID_ANY, "Continue");
	continueButton->SetDefault();
	wxBoxSizer* welcome = new wxBoxSizer(wxVERTICAL);
	welcome->AddStretchSpacer(1);
	welcome->Add(welcomeTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	welcome->Add(welcomeBody, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 12);
	welcome->Add(continueButton, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 28);
	welcome->AddStretchSpacer(1);
	this->welcomePanel->SetSizer(welcome);

	this->namePanel = MakePage(this);
	wxStaticText* nameTitle = MakeTitle(this->namePanel, "What should we call you?", 4);
	this->nameCtrl = new wxTextCtrl(this->namePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(280, -1));
	wxButton* nameContinue = new wxButton(this->namePanel, wxID_ANY, "Continue");
	wxButton* nameSkip = new wxButton(this->namePanel, wxID_ANY, "Skip");
	nameContinue->SetDefault();
	wxBoxSizer* nameButtons = new wxBoxSizer(wxHORIZONTAL);
	nameButtons->Add(nameSkip, 0, wxRIGHT, 8);
	nameButtons->Add(nameContinue, 0);
	wxBoxSizer* name = new wxBoxSizer(wxVERTICAL);
	name->AddStretchSpacer(1);
	name->Add(nameTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	name->Add(this->nameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 16);
	name->Add(nameButtons, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 24);
	name->AddStretchSpacer(1);
	this->namePanel->SetSizer(name);

	this->goalPanel = MakePage(this);
	wxStaticText* goalTitle = MakeTitle(this->goalPanel, "How many cards do you want to review each day?", 3);
	wxButton* goal10 = new wxButton(this->goalPanel, 10, "10 cards");
	wxButton* goal20 = new wxButton(this->goalPanel, 20, "20 cards");
	wxButton* goal30 = new wxButton(this->goalPanel, 30, "30 cards");
	wxButton* goalSkip = new wxButton(this->goalPanel, wxID_ANY, "Skip");
	wxBoxSizer* goalChoices = new wxBoxSizer(wxHORIZONTAL);
	goalChoices->Add(goal10, 0, wxRIGHT, 8);
	goalChoices->Add(goal20, 0, wxRIGHT, 8);
	goalChoices->Add(goal30, 0);
	wxBoxSizer* goal = new wxBoxSizer(wxVERTICAL);
	goal->AddStretchSpacer(1);
	goal->Add(goalTitle, 0, wxALIGN_CENTER_HORIZONTAL);
	goal->Add(goalChoices, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);
	goal->Add(goalSkip, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 16);
	goal->AddStretchSpacer(1);
	this->goalPanel->SetSizer(goal);

	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(this->welcomePanel, 1, wxEXPAND | wxALL, 24);
	root->Add(this->namePanel, 1, wxEXPAND | wxALL, 24);
	root->Add(this->goalPanel, 1, wxEXPAND | wxALL, 24);
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
#ifdef __WXOSX__
	id window = reinterpret_cast<id>(this->GetWXWindow());
	if (window == nullptr) {
		return;
	}
	using GetMask = unsigned long (*)(id, SEL);
	using SetMask = void (*)(id, SEL, unsigned long);
	using VoidBool = void (*)(id, SEL, bool);
	static unsigned long titledMask = 0;
	const unsigned long current = reinterpret_cast<GetMask>(objc_msgSend)(window, sel_registerName("styleMask"));
	if (titledMask == 0 && current != 0) {
		titledMask = current;
	}
	const unsigned long next = titled && titledMask != 0 ? titledMask : 0UL;
	reinterpret_cast<SetMask>(objc_msgSend)(window, sel_registerName("setStyleMask:"), next);
	reinterpret_cast<VoidBool>(objc_msgSend)(window, sel_registerName("setMovableByWindowBackground:"), !titled);

	using VoidDouble = void (*)(id, SEL, double);
	using VoidId = void (*)(id, SEL, id);
	using VoidPtr = void (*)(id, SEL, void*);
	using IdFn = id (*)(id, SEL);
	using ColorFn = id (*)(id, SEL, double, double, double, double);
	const Theme& theme = Theme::Get();
	const int radius = titled ? 0 : Theme::Dip(this, theme.radius.sm);
	this->SetBackgroundColour(theme.color.window);
	id colorClass = reinterpret_cast<id>(objc_getClass("NSColor"));
	id windowColor = titled
		? reinterpret_cast<ColorFn>(objc_msgSend)(
			colorClass,
			sel_registerName("colorWithCalibratedRed:green:blue:alpha:"),
			theme.color.window.Red() / 255.0,
			theme.color.window.Green() / 255.0,
			theme.color.window.Blue() / 255.0,
			1.0
		)
		: reinterpret_cast<IdFn>(objc_msgSend)(colorClass, sel_registerName("clearColor"));
	reinterpret_cast<VoidId>(objc_msgSend)(window, sel_registerName("setBackgroundColor:"), windowColor);
	reinterpret_cast<VoidBool>(objc_msgSend)(window, sel_registerName("setOpaque:"), titled);

	id content = reinterpret_cast<IdFn>(objc_msgSend)(window, sel_registerName("contentView"));
	if (content == nullptr) {
		return;
	}
	reinterpret_cast<VoidBool>(objc_msgSend)(content, sel_registerName("setWantsLayer:"), true);
	id layer = reinterpret_cast<IdFn>(objc_msgSend)(content, sel_registerName("layer"));
	if (layer == nullptr) {
		return;
	}
	reinterpret_cast<VoidDouble>(objc_msgSend)(layer, sel_registerName("setCornerRadius:"), static_cast<double>(radius));
	reinterpret_cast<VoidBool>(objc_msgSend)(layer, sel_registerName("setMasksToBounds:"), !titled);
	if (!titled) {
		id fill = reinterpret_cast<ColorFn>(objc_msgSend)(
			colorClass,
			sel_registerName("colorWithCalibratedRed:green:blue:alpha:"),
			theme.color.window.Red() / 255.0,
			theme.color.window.Green() / 255.0,
			theme.color.window.Blue() / 255.0,
			1.0
		);
		void* cgColor = reinterpret_cast<IdFn>(objc_msgSend)(fill, sel_registerName("CGColor"));
		reinterpret_cast<VoidPtr>(objc_msgSend)(layer, sel_registerName("setBackgroundColor:"), cgColor);
	}
	using VoidFn = void (*)(id, SEL);
	reinterpret_cast<VoidFn>(objc_msgSend)(window, sel_registerName("invalidateShadow"));
#endif
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
