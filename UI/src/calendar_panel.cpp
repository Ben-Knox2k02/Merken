#include "calendar_panel.h"
#include "mainframe.h"
#include "app.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include <thread>

wxDECLARE_APP(App);

CalendarPanel::CalendarPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	this->rootSizer = new wxBoxSizer(wxVERTICAL);

	this->header = new wxStaticText(this, wxID_ANY, "Calendar");
	this->header->SetFont(this->header->GetFont().Bold());
	this->rootSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	this->statusLabel = new wxStaticText(this, wxID_ANY, "Loading events…");
	this->rootSizer->Add(this->statusLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 4);

	this->eventList = new wxDataViewCtrl(this, wxID_ANY);
	this->eventList->AppendTextColumn("Title", 0, wxDATAVIEW_CELL_INERT, 180, wxALIGN_LEFT);
	this->eventList->AppendTextColumn("Start", 1, wxDATAVIEW_CELL_INERT, 160, wxALIGN_LEFT);
	this->eventList->AppendTextColumn("End", 2, wxDATAVIEW_CELL_INERT, 160, wxALIGN_LEFT);
	this->eventList->AppendTextColumn("Description", 3, wxDATAVIEW_CELL_INERT, 240, wxALIGN_LEFT);

	this->eventViewModel = new wxDataViewListStore();
	this->eventList->AssociateModel(this->eventViewModel);
	this->eventViewModel->DecRef();
	this->rootSizer->Add(this->eventList, 1, wxEXPAND | wxALL, 10);

	this->backButton = new wxButton(this, wxID_ANY, "Back");
	this->rootSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 10);

	this->SetSizer(this->rootSizer);
	this->backButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnBack, this);
	this->LoadEvents();
}

CalendarPanel::~CalendarPanel() {
	*this->alive = false;
}

void CalendarPanel::LoadEvents() {
	this->statusLabel->SetLabel("Loading events…");
	this->eventViewModel->DeleteAllItems();

	auto useCase = wxGetApp().GetInjector().create<GetEventsUseCase>();
	auto alive = this->alive;
	std::thread([this, alive, useCase]() mutable {
		GetEventsResponse response = useCase.Execute();
		if (wxTheApp == nullptr) { return; }
		wxTheApp->CallAfter([this, alive, response]() {
			if (!*alive) { return; }
			this->ShowEvents(response);
		});
	}).detach();
}

void CalendarPanel::ShowEvents(const GetEventsResponse& response) {
	this->eventViewModel->DeleteAllItems();
	for (const EventResponse& event : response.events) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(wxString(event.title)));
		row.push_back(wxVariant(wxString(event.startTime)));
		row.push_back(wxVariant(wxString(event.endTime)));
		row.push_back(wxVariant(wxString(event.description)));
		this->eventViewModel->AppendItem(row, static_cast<wxUIntPtr>(event.calendarEventId));
	}

	if (response.events.empty()) {
		this->statusLabel->SetLabel("No events.");
	} else {
		this->statusLabel->SetLabel(wxString::Format("%d events", static_cast<int>(response.events.size())));
	}
	this->Layout();
}

void CalendarPanel::OnBack(wxCommandEvent&) {
	if (auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
