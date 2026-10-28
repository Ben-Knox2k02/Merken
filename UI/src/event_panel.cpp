#include "event_panel.h"

EventPanel::EventPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Events");
	this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->header->SetForegroundColour(*wxBLACK);
	this->header->SetBackgroundColour(wxColour(240, 240, 240));
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->eventList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

	this->eventList->InsertColumn(0, "Title", wxLIST_FORMAT_LEFT, 180);
	this->eventList->InsertColumn(1, "Start", wxLIST_FORMAT_LEFT, 160);
	this->eventList->InsertColumn(2, "End", wxLIST_FORMAT_LEFT, 160);
	this->eventList->InsertColumn(3, "Description", wxLIST_FORMAT_LEFT, 240);

	this->rootSizer->Add(this->eventList, 1, wxEXPAND | wxALL, 10);

	this->addEventButton = new wxButton(this, wxID_ANY, "Add Event");
	this->editEventButton = new wxButton(this, wxID_ANY, "Edit Event");
	this->deleteEventButton = new wxButton(this, wxID_ANY, "Delete Event");
	
	wxBoxSizer* bottomNav = new wxBoxSizer(wxHORIZONTAL);

	bottomNav->AddStretchSpacer(1);
	bottomNav->Add(this->addEventButton, 0, wxRIGHT, 10);
	bottomNav->Add(this->editEventButton, 0, wxRIGHT, 10);
	bottomNav->Add(this->deleteEventButton, 0, wxALIGN_CENTER_VERTICAL);
	bottomNav->AddStretchSpacer(1);
	
	this->rootSizer->Add(bottomNav, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->SetSizer(this->rootSizer);
	this->LoadEvents();
	
	//=========== BIND EVENT HANDLERS ===================== BIND EVENT HANDLERS ==========
	this->Bind(wxEVT_PAINT, &EventPanel::OnPaint, this);
	this->addEventButton->Bind(wxEVT_BUTTON, &EventPanel::OnAddEvent, this);
	this->editEventButton->Bind(wxEVT_BUTTON, &EventPanel::OnEditEvent, this);
	this->deleteEventButton->Bind(wxEVT_BUTTON, &EventPanel::OnDeleteEvent, this);
}

EventPanel::~EventPanel() {
	*this->alive = false;
}

void EventPanel::LoadEvents() {
	
}

void EventPanel::ShowEvents(const GetEventsResponse& response) {
	
}

void EventPanel::OnPaint(wxPaintEvent&) {
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void EventPanel::OnAddEvent(wxCommandEvent& event) {
	
	event.Skip();
}


void EventPanel::OnEditEvent(wxCommandEvent& event) {
	
	event.Skip();
}

void EventPanel::OnDeleteEvent(wxCommandEvent& event) {
	
	event.Skip();
}
