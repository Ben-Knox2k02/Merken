#include "event_panel.h"

EventPanel::EventPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	//========== ROOT PANEL ============================================
	App& app = wxGetApp();
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
	//========== TOP PANEL =============================================
	this->topPanel = new wxPanel(this);
    this->topPanel->SetBackgroundColour(Theme::Get().color.primary);
	//this->topPanel->SetBackgroundColour(*wxBLUE);
    this->topSizer = new wxBoxSizer(wxVERTICAL);
	
	this->header = new wxStaticText(this->topPanel, wxID_ANY, "Events");
	this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	this->header->SetForegroundColour(*wxWHITE);
	this->header->SetBackgroundColour(Theme::Get().color.primary);
	//this->header->SetBackgroundColour(*wxBLUE);
	this->topSizer->Add(this->header, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
	
	this->topPanel->SetSizer(this->topSizer);
	
	//========== MID PANEL =============================================
	this->midPanel = new wxPanel(this);
    this->midPanel->SetBackgroundColour(wxColour(240, 240, 240));
    this->midSizer = new wxBoxSizer(wxVERTICAL);
	
	int sm = app.GetWindowWidth() / 16;
	int md = app.GetWindowWidth() / 14;
	int lg = app.GetWindowWidth() / 12;
	this->eventList = new wxListCtrl(this->midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	this->eventList->InsertColumn(0, "Title", wxLIST_FORMAT_LEFT, md);
	this->eventList->InsertColumn(1, "Date", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(2, "Start", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(3, "End", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(4, "Location", wxLIST_FORMAT_LEFT, md);
	this->eventList->InsertColumn(5, "Description", wxLIST_FORMAT_LEFT, lg);
	this->eventList->InsertColumn(6, "Reminder", wxLIST_FORMAT_LEFT, sm);
	this->midSizer->Add(this->eventList, 1, wxEXPAND | wxALL, 10);
	
    this->midPanel->SetSizer(this->midSizer);
	
	//========= BOTTOM PANEL ===========================================
	this->bottomPanel = new wxPanel(this);
	this->bottomPanel->SetBackgroundColour(Theme::Get().color.primary);
	//this->bottomPanel->SetBackgroundColour(*wxBLUE);
    this->bottomSizer = new wxBoxSizer(wxVERTICAL);	  
	
	this->addEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Add Event");
	this->editEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Edit Event");
	this->deleteEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Delete Event");
	
	wxBoxSizer* bottomNav = new wxBoxSizer(wxHORIZONTAL);
	bottomNav->AddStretchSpacer(1);
	bottomNav->Add(this->addEventButton, 0, wxRIGHT, 10);
	bottomNav->Add(this->editEventButton, 0, wxRIGHT, 10);
	bottomNav->Add(this->deleteEventButton, 0, wxALIGN_CENTER_VERTICAL);
	bottomNav->AddStretchSpacer(1);
		
	this->bottomSizer->Add(bottomNav, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);	
    this->bottomPanel->SetSizer(this->bottomSizer);
	
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(this->topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(this->midPanel,    1, wxEXPAND | wxALL, 0);
    this->rootSizer->Add(this->bottomPanel, 0, wxEXPAND | wxALL, 0);

    this->SetSizer(this->rootSizer);
    this->Layout();
	//this->LoadEvents();
	
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

void EventPanel::ShowEvents(const GetEventsResponse& response) {
	
}

//========== EVENT HANDLERS ========================= EVENT HANDLERS =================

void EventPanel::OnPaint(wxPaintEvent&) {
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void EventPanel::OnAddEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Add Event"); }
	
	EventDialog dialog(this);
	dialog.CentreOnParent();
	
	if(dialog.ShowModal() == wxID_OK) {
		frame->SetStatusText("Event Created");
		long row = this->eventList->GetItemCount();
		long index = this->eventList->InsertItem(row, dialog.GetTitle());
		this->eventList->SetItem(index, 1, dialog.GetDate());
		this->eventList->SetItem(index, 2, dialog.GetStart());
		this->eventList->SetItem(index, 3, dialog.GetEnd());
		this->eventList->SetItem(index, 4, dialog.GetLocation());
		this->eventList->SetItem(index, 5, dialog.GetDescription());
		this->eventList->SetItem(index, 6, dialog.GetReminder());
	}
	
	event.Skip();
}


void EventPanel::OnEditEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Event"); }
	event.Skip();
}

void EventPanel::OnDeleteEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Event"); }
	event.Skip();
}
