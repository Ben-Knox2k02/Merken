#include "app.h"
#include "dialog.h"
#include "theme.h"

CardDialog::CardDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(0, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Front:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->frontCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
	this->frontCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->frontCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Back:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->backCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
	this->backCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->backCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Tags:"), 0, wxALIGN_CENTER_VERTICAL);
	this->tagCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->tagCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(480, 360));
	this->SetSize(wxSize(520, 400));
	Theme::Get().StyleDialog(this);
	this->Layout();
	this->CentreOnParent();
}

DeckDialog::DeckDialog(wxWindow* parent, const wxString& title)
	: wxDialog(
		wxGetTopLevelParent(parent),
		wxID_ANY,
		title,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
	) {
	wxFlexGridSizer* fields = new wxFlexGridSizer(2, 10, 12);
	fields->AddGrowableCol(1, 1);
	fields->AddGrowableRow(1, 1);

	fields->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
	this->nameCtrl = new wxTextCtrl(this, wxID_ANY);
	fields->Add(this->nameCtrl, 1, wxEXPAND);

	fields->Add(new wxStaticText(this, wxID_ANY, "Description:"), 0, wxALIGN_TOP | wxTOP, 4);
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	this->descriptionCtrl->SetMinSize(wxSize(-1, 80));
	fields->Add(this->descriptionCtrl, 1, wxEXPAND);

	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(new wxButton(this, wxID_OK));
	buttons->AddButton(new wxButton(this, wxID_CANCEL));
	buttons->Realize();

	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	this->rootSizer->Add(fields, 1, wxEXPAND | wxALL, 16);
	this->rootSizer->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 16);

	this->SetSizer(this->rootSizer);
	this->SetMinSize(wxSize(480, 320));
	this->SetSize(wxSize(520, 360));
	Theme::Get().StyleDialog(this);
	this->Layout();
	this->CentreOnParent();
}

EventDialog::EventDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Create Event", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
	App& app = wxGetApp();
	this->SetBackgroundColour(wxColour(240, 240, 240));
	
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
	this->titleText = new wxStaticText(this, wxID_ANY, "Title:");											// TITLE		
	this->titleCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->titleText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	this->rootSizer->Add(this->titleCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->dateText = new wxStaticText(this, wxID_ANY, "Date:");											// DATE
	this->dateCtrl = new wxDatePickerCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->dateText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	this->rootSizer->Add(this->dateCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->startText = new wxStaticText(this, wxID_ANY, "Start Time:");									// START TIME
	this->rootSizer->Add(this->startText, 0, wxLEFT | wxTOP | wxRIGHT, 5);	
	
	wxBoxSizer* startSizer = new wxBoxSizer(wxHORIZONTAL);
	this->startHourChoice = new wxChoice(this, wxID_ANY);
	this->startMinuteChoice = new wxChoice(this, wxID_ANY);
	
	for(int i = 0; i < 24; ++i) {
		wxString h = wxString::Format("%02d", i);
		this->startHourChoice->Append(h);
	}
	for(int i = 0; i < 60; ++i) {
		wxString m = wxString::Format("%02d", i);
		this->startMinuteChoice->Append(m);
	}
	
	this->startHourChoice->SetSelection(9);
	this->startMinuteChoice->SetSelection(0);
	
	startSizer->Add(this->startHourChoice, 1, wxRIGHT, 5);
	startSizer->Add(new wxStaticText(this, wxID_ANY, ":"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	startSizer->Add(this->startMinuteChoice, 1);
	this->rootSizer->Add(startSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->endText = new wxStaticText(this, wxID_ANY, "End Time:");										// END TIME
	this->rootSizer->Add(this->endText, 0, wxLEFT | wxTOP | wxRIGHT, 5);

	wxBoxSizer* endSizer = new wxBoxSizer(wxHORIZONTAL);
	this->endHourChoice = new wxChoice(this, wxID_ANY);
	this->endMinuteChoice = new wxChoice(this, wxID_ANY);

	for(int i = 0; i < 24; ++i) {
		wxString h = wxString::Format("%02d", i);
		this->endHourChoice->Append(h);
	}
	for(int i = 0; i < 60; ++i) {
		wxString m = wxString::Format("%02d", i);
		this->endMinuteChoice->Append(m);
	}

	this->endHourChoice->SetSelection(10);
	this->endMinuteChoice->SetSelection(0);

	endSizer->Add(this->endHourChoice, 1, wxRIGHT, 5);
	endSizer->Add(new wxStaticText(this, wxID_ANY, ":"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	endSizer->Add(this->endMinuteChoice, 1);
	this->rootSizer->Add(endSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->locationText = new wxStaticText(this, wxID_ANY, "Location:");									// LOCATION
	this->locationCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->locationText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	this->rootSizer->Add(this->locationCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->descriptionText = new wxStaticText(this, wxID_ANY, "Description:");								// DESCRIPTION
	this->descriptionCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE | wxTE_RICH2);
	this->rootSizer->Add(this->descriptionText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	this->rootSizer->Add(this->descriptionCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->reminderText = new wxStaticText(this, wxID_ANY, "Reminder:");									// REMINDER
	this->reminderCtrl = new wxTextCtrl(this, wxID_ANY);
	this->rootSizer->Add(this->reminderText, 0, wxLEFT | wxTOP | wxRIGHT, 5);
	this->rootSizer->Add(this->reminderCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
	
	this->rootSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 10);
	this->SetSizerAndFit(this->rootSizer);
	this->Fit();
	int targetWidth = app.GetWindowWidth() / 4;
	this->SetSize(targetWidth, -1);
}

wxString EventDialog::GetTitle() const { return this->titleCtrl->GetValue(); }

wxDateTime EventDialog::GetDate() const { return this->dateCtrl->GetValue(); }

wxDateTime EventDialog::GetStart() const {
    wxDateTime dt = wxDateTime::Today();
    long hour = 0, minute = 0;
    
    if(this->startHourChoice->GetSelection() != wxNOT_FOUND) {
        this->startHourChoice->GetStringSelection().ToLong(&hour);
    }
    if(this->startMinuteChoice->GetSelection() != wxNOT_FOUND) {
        this->startMinuteChoice->GetStringSelection().ToLong(&minute);
    }
    
    dt.SetHour(hour);
    dt.SetMinute(minute);
    dt.SetSecond(0);
	dt.SetMillisecond(0);
    return dt;
}

wxDateTime EventDialog::GetEnd() const {
    wxDateTime dt = wxDateTime::Today();
    long hour = 0, minute = 0;
    
    if(this->endHourChoice->GetSelection() != wxNOT_FOUND) {
        this->endHourChoice->GetStringSelection().ToLong(&hour);
    }
    if(this->endMinuteChoice->GetSelection() != wxNOT_FOUND) {
        this->endMinuteChoice->GetStringSelection().ToLong(&minute);
    }
    
    dt.SetHour(hour);
    dt.SetMinute(minute);
    dt.SetSecond(0);
	dt.SetMillisecond(0);
    return dt;
}

wxString EventDialog::GetLocation() const { return this->locationCtrl->GetValue(); }

wxString EventDialog::GetDescription() const { return this->descriptionCtrl->GetValue(); }

wxString EventDialog::GetReminder() const { return this->reminderCtrl->GetValue(); }

void EventDialog::SetTitle(const wxString& title) { this->titleCtrl->SetValue(title); }

void EventDialog::SetDate(const wxDateTime& dt) { this->dateCtrl->SetValue(dt); }

void EventDialog::SetStart(const wxDateTime& dt) {
    if(dt.IsValid()) {
        wxString hStr = wxString::Format("%02d", dt.GetHour());
        wxString mStr = wxString::Format("%02d", dt.GetMinute());
        
        if(this->startMinuteChoice->FindString(mStr) == wxNOT_FOUND) {
            this->startMinuteChoice->Append(mStr);
        }
        
        this->startHourChoice->SetStringSelection(hStr);
        this->startMinuteChoice->SetStringSelection(mStr);
    }
}

void EventDialog::SetEnd(const wxDateTime& dt) {
    if(dt.IsValid()) {
        wxString hStr = wxString::Format("%02d", dt.GetHour());
        wxString mStr = wxString::Format("%02d", dt.GetMinute());
        
        if (this->endMinuteChoice->FindString(mStr) == wxNOT_FOUND) {
            this->endMinuteChoice->Append(mStr);
        }
        
        this->endHourChoice->SetStringSelection(hStr);
        this->endMinuteChoice->SetStringSelection(mStr);
    }
}

void EventDialog::SetLocation(const wxString& location) { this->locationCtrl->SetValue(location); }

void EventDialog::SetDescription(const wxString& description) { this->descriptionCtrl->SetValue(description); }

void EventDialog::SetReminder(const wxString& reminder) {
    if(this->reminderCtrl) {
        // Note: If the reminder control is a choice dropdown instead of a text control,
        // use something like: this->reminderCtrl->SetStringSelection(reminder);
        this->reminderCtrl->SetValue(reminder);
    }
}




