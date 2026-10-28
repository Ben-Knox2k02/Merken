#include "event_panel.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include <wx/dcbuffer.h>
#include <thread>

EventPanel::EventPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetDoubleBuffered(true);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Calendar");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->statusLabel = new wxStaticText(this, wxID_ANY, "Loading events...");
	this->rootSizer->Add(this->statusLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	this->eventList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

	// Insert columns (wxListCtrl uses InsertColumn instead of AppendTextColumn)
	this->eventList->InsertColumn(0, "Title", wxLIST_FORMAT_LEFT, 180);
	this->eventList->InsertColumn(1, "Start", wxLIST_FORMAT_LEFT, 160);
	this->eventList->InsertColumn(2, "End", wxLIST_FORMAT_LEFT, 160);
	this->eventList->InsertColumn(3, "Description", wxLIST_FORMAT_LEFT, 240);

	this->rootSizer->Add(this->eventList, 1, wxEXPAND | wxALL, 10);

	this->backButton = new wxButton(this, wxID_ANY, "Back");
	this->rootSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 10);

	this->SetSizer(this->rootSizer);
	this->Bind(wxEVT_PAINT, &EventPanel::OnPaint, this);
	this->backButton->Bind(wxEVT_BUTTON, &EventPanel::OnBack, this);
	this->LoadEvents();
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

void EventPanel::OnBack(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
