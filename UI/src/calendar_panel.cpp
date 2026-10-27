#include "calendar_panel.h"
#include "mainframe.h"
#include "app.h"
#include "theme.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include <thread>

wxDECLARE_APP(App); 

CalendarPanel::CalendarPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	App& app = wxGetApp();
	//============ ROOT PANEL ======================================== ROOT PANEL =====================
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->rootSizer = new wxBoxSizer(wxVERTICAL);

	//========= TOP SECTION PANEL =================================  TOP SECTION PANEL ================
    this->topPanel = new wxPanel(this);
    this->topPanel->SetBackgroundColour(Theme::Get().color.primary);

    this->topSizer = new wxBoxSizer(wxVERTICAL);

    this->header = new wxStaticText(this->topPanel, wxID_ANY, "Calendar");
    this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    this->header->SetForegroundColour(*wxWHITE);
	this->header->SetBackgroundColour(Theme::Get().color.primary);

    this->topSizer->Add(this->header, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    this->topPanel->SetSizer(this->topSizer);

	//============ MIDDLE SECTION PANEL =========================== MIDDLE SECTION PANEL ==============
    this->midPanel = new wxPanel(this);
    this->midPanel->SetBackgroundColour(wxColour(240, 240, 240)); 				// LIGHT GREY

    this->midSizer = new wxBoxSizer(wxVERTICAL);
	
	this->calendar = new wxCalendarCtrl(
		this->midPanel,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxSize(app.GetScreenWidth() * 4 / 7, app.GetScreenHeight()),
		wxCAL_SHOW_HOLIDAYS | wxCAL_MONDAY_FIRST
	);
	
	//this->midSizer->Add(this->calendar, 1, wxEXPAND | wxALL, 10);
	this->midSizer->Add(this->calendar, 0, wxALIGN_CENTER | wxALL, 10);
    this->midPanel->SetSizer(this->midSizer);

    //========== BOTTOM SECTION PANEL ============================== BOTTOM SECTION PANEL =============
    this->bottomPanel = new wxPanel(this);
	this->bottomPanel->SetBackgroundColour(Theme::Get().color.primary);

    this->bottomSizer = new wxBoxSizer(wxVERTICAL);
	
	this->backButton = new wxButton(this->bottomPanel, wxID_ANY, "Back");
	this->bottomSizer->Add(this->backButton, 0, wxALIGN_CENTER | wxALL, 10);
	
    this->bottomPanel->SetSizer(bottomSizer);
    
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(this->topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(this->midPanel,    1, wxEXPAND | wxALL, 0);
    this->rootSizer->Add(this->bottomPanel, 0, wxEXPAND | wxALL, 0);

    this->SetSizer(this->rootSizer);
    this->Layout();
	
	//========== BIND EVENT HANDLERS ============================== BIND EVENT HANDLERS ================
	this->Bind(wxEVT_PAINT, &CalendarPanel::OnPaint, this);
	this->backButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnBack, this);
	
	/* this->createEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnCreateEvent, this);				
    this->editEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnEditEvent, this);
    this->deleteEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnDeleteEvent, this);	

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
	
	this->LoadEvents(); */
}

CalendarPanel::~CalendarPanel() {		
	*this->alive = false;
}

/* void CalendarPanel::LoadEvents() {
	this->statusLabel->SetLabel("Loading events...");
	this->eventViewModel->DeleteAllItems();

	auto useCase = wxGetApp().GetInjector().create<GetEventsUseCase>();
	auto alive = this->alive;
	std::thread([this, alive, useCase]() mutable {			// SPAWN NEW THREAD
		GetEventsResponse response = useCase.Execute();
		if(wxTheApp == nullptr) { return; }
		wxTheApp->CallAfter([this, alive, response]() {		// SEND DATA BACK TO MAIN THREAD ONCE BACKGROUND FINISHES
			if(!*alive) { return; }
			this->ShowEvents(response);
		});
	}).detach();											// THREAD CLEANS UP ITSELF
}

void CalendarPanel::ShowEvents(const GetEventsResponse& response) {
	this->eventViewModel->DeleteAllItems();
	for(const EventResponse& event : response.events) {
		wxVector<wxVariant> row;
		row.push_back(wxVariant(wxString(event.title)));
		row.push_back(wxVariant(wxString(event.startTime)));
		row.push_back(wxVariant(wxString(event.endTime)));
		row.push_back(wxVariant(wxString(event.description)));
		this->eventViewModel->AppendItem(row, static_cast<wxUIntPtr>(event.calendarEventId));
	}

	if(response.events.empty()) {
		this->statusLabel->SetLabel("No events.");
	} 
	else {
		this->statusLabel->SetLabel(wxString::Format("%d events", static_cast<int>(response.events.size())));
	}
	this->Layout();
} */

void CalendarPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void CalendarPanel::OnBack(wxCommandEvent&) {
	if(auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	}
}
