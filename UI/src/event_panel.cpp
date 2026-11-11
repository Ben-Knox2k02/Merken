#include "event_panel.h"
#include "../../Application/UseCases/Calendar/CreateEvent/create_event_usecase.h"
#include "../../Application/UseCases/Calendar/CreateEvent/create_event_request.h"
#include "../../Application/UseCases/Calendar/CreateEvent/create_event_response.h"
#include "../../Application/UseCases/Calendar/GetEvents/get_events_usecase.h"
#include "../../Application/UseCases/Calendar/UpdateEvent/update_event_request.h"
#include "../../Application/UseCases/Calendar/UpdateEvent/update_event_usecase.h"
#include "../../Application/UseCases/Calendar/DeleteEvent/delete_event_request.h"
#include "../../Application/UseCases/Calendar/DeleteEvent/delete_event_usecase.h"

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
	
	int xs = app.GetWindowWidth() / 20;	
	int sm = app.GetWindowWidth() / 15;
	int md = app.GetWindowWidth() / 10;
	int lg = app.GetWindowWidth() / 8;
	int xl = app.GetWindowWidth() / 7;
	this->eventList = new wxListCtrl(this->midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	this->eventList->InsertColumn(0, "Title", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(1, "Date", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(2, "Start", wxLIST_FORMAT_LEFT, xs);
	this->eventList->InsertColumn(3, "End", wxLIST_FORMAT_LEFT, xs);
	this->eventList->InsertColumn(4, "Location", wxLIST_FORMAT_LEFT, sm);
	this->eventList->InsertColumn(5, "Description", wxLIST_FORMAT_LEFT, sm);
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
	if(!this->currentDate.IsValid()) { this->currentDate = wxDateTime::Now(); }
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
	auto useCase = wxGetApp().GetInjector().create<GetEventsUseCase>();
	auto alive = this->alive;

	std::thread([this, alive, useCase]() mutable {
		GetEventsResponse response = useCase.Execute();
		
		if(wxTheApp == nullptr) { return; }
		
		wxTheApp->CallAfter([this, alive, response]() {
			if(!*alive) { return; }
			this->ShowEvents(response);
		});
	}).detach();
}

void EventPanel::LoadEventsForDate(const wxDateTime& date) {
	this->currentDate = date;
	this->LoadEvents();
}

void EventPanel::ShowEvents(const GetEventsResponse& response) {
	this->eventList->DeleteAllItems();
	
	for(const auto& ev : response.events) {
		long row = this->eventList->GetItemCount();
		long index = this->eventList->InsertItem(row, ev.title);
		this->eventList->SetItemData(index, ev.calendarEventId);
		this->eventList->SetItem(index, 1, ev.date.ToIso());
		this->eventList->SetItem(index, 2, ev.startTime);
		this->eventList->SetItem(index, 3, ev.endTime);
		this->eventList->SetItem(index, 4, ev.location);
		this->eventList->SetItem(index, 5, ev.description);
		this->eventList->SetItem(index, 6, wxString::Format("%d minutes", ev.reminderMinutes));
	}
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
	dialog.SetTitle("Create Event");
	
	if(this->currentDate.IsValid()) {
		dialog.SetDate(this->currentDate);
	}
	
	dialog.CentreOnParent();
	
	if(dialog.ShowModal() == wxID_OK) {
		CreateEventRequest request;
		request.title = dialog.GetEventTitle().ToStdString();
		request.location = dialog.GetLocation().ToStdString();
		request.description = dialog.GetDescription().ToStdString();
		
		wxDateTime wxDt = dialog.GetDate();
		auto dateOpt = Date::Create(wxDt.GetYear(), wxDt.GetMonth() + 1, wxDt.GetDay());
		if(dateOpt.has_value()) {
			request.date = *dateOpt;
		}
		
		request.startTime = dialog.GetStart().Format("%H:%M").ToStdString();
		request.endTime = dialog.GetEnd().Format("%H:%M").ToStdString(); 
		
		wxDateTime startTime = dialog.GetStart();
		wxDateTime reminderTime = dialog.GetReminder();
		wxTimeSpan diff = startTime - reminderTime;
		long totalMinutes = diff.GetMinutes();
		long wrappedMinutes = (totalMinutes % 1440 + 1440) % 1440;
		request.reminderMinutes = static_cast<int>(wrappedMinutes);

		auto createUseCase = wxGetApp().GetInjector().create<CreateEventUseCase>();
		auto alive = this->alive;
		
		std::thread([this, alive, createUseCase, request, frame]() mutable {	// Run the use case asynchronously so it doesn't freeze/crash the GUI
			try {
				CreateEventResponse response = createUseCase.Execute(request);

				if(wxTheApp == nullptr) { return; }
				
				wxTheApp->CallAfter([this, alive, response, frame]() {		// Safely pass data back to the main UI thread
					if (!*alive) { return; }
					if(frame) { frame->SetStatusText("Event Created Successfully"); }
					
					long row = this->eventList->GetItemCount();
					long index = this->eventList->InsertItem(row, response.title);
					this->eventList->SetItem(index, 1, response.date.ToIso());
					this->eventList->SetItem(index, 2, response.startTime);
					this->eventList->SetItem(index, 3, response.endTime);
					this->eventList->SetItem(index, 4, response.location);
					this->eventList->SetItem(index, 5, response.description);
					this->eventList->SetItem(index, 6, wxString::Format("%d hours before", response.reminderMinutes / 60));
				});
			} 
			catch(const std::exception& e) {
				if(wxTheApp == nullptr) { return; }
				wxTheApp->CallAfter([e]() {
					wxMessageBox(wxString::Format("Failed to create event: %s", e.what()), "Error", wxOK | wxICON_ERROR);
				});
			}
		}).detach();
	}
	
	event.Skip();
}

void EventPanel::OnEditEvent(wxCommandEvent& event) {
	long selected = this->eventList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (selected == -1) {
		wxMessageBox("Please select an event to edit.", "Edit Event", wxOK | wxICON_WARNING, this);
		event.Skip();
		return;
	}
	int eventId = static_cast<int>(this->eventList->GetItemData(selected));

	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Event"); }

	EventDialog dialog(this);
	dialog.SetTitle("Edit Event");
	
	dialog.SetEventTitle(this->eventList->GetItemText(selected, 0));
	dialog.SetLocation(this->eventList->GetItemText(selected, 4));
	dialog.SetDescription(this->eventList->GetItemText(selected, 5));
	
	wxDateTime eventDate;
	if(eventDate.ParseFormat(this->eventList->GetItemText(selected, 1), "%m/%d/%Y")) {
		dialog.SetDate(eventDate);
	}

	wxDateTime startTime, endTime;
	if(startTime.ParseTime(this->eventList->GetItemText(selected, 2))) {
		dialog.SetStart(startTime);
	}
	if(endTime.ParseTime(this->eventList->GetItemText(selected, 3))) {
		dialog.SetEnd(endTime);
	}
	
	wxDateTime reminderDate;
	if(reminderDate.ParseTime(this->eventList->GetItemText(selected, 6))) {
		dialog.SetReminder(reminderDate);
	}

	dialog.CentreOnParent();
	
	if(dialog.ShowModal() == wxID_OK) {	
		UpdateEventRequest request;
		request.calendarEventId = eventId;
		request.title = dialog.GetEventTitle().ToStdString();
		request.location = dialog.GetLocation().ToStdString();
		request.description = dialog.GetDescription().ToStdString();
		
		wxDateTime wxDt = dialog.GetDate();
		auto dateOpt = Date::Create(wxDt.GetYear(), wxDt.GetMonth() + 1, wxDt.GetDay());
		if(dateOpt.has_value()) {
			request.date = *dateOpt;
		}
		
		request.startTime = dialog.GetStart().Format("%H:%M").ToStdString();
		request.endTime = dialog.GetEnd().Format("%H:%M").ToStdString();
		
		wxDateTime startTime = dialog.GetStart();
		wxDateTime reminderTime = dialog.GetReminder();
		wxTimeSpan diff = startTime - reminderTime;
		long totalMinutes = diff.GetMinutes();
		long wrappedMinutes = (totalMinutes % 1440 + 1440) % 1440;
		request.reminderMinutes = static_cast<int>(wrappedMinutes);

		auto updateUseCase = wxGetApp().GetInjector().create<UpdateEventUseCase>();
		auto alive = this->alive;

		// Capture the request by value so the background thread can use its updated values for the UI
		std::thread([this, alive, updateUseCase, selected, frame, request]() mutable {
			try {
				updateUseCase.Execute(request);
				
				if(wxTheApp == nullptr) { return; }
				wxTheApp->CallAfter([this, alive, selected, frame, request]() {
					if(!*alive) { return; }
					if(frame) { frame->SetStatusText("Event Updated"); }
					
					this->eventList->SetItem(selected, 0, request.title);
					this->eventList->SetItem(selected, 1, request.date.ToIso());
					this->eventList->SetItem(selected, 2, request.startTime);
					this->eventList->SetItem(selected, 3, request.endTime);
					this->eventList->SetItem(selected, 4, request.location);
					this->eventList->SetItem(selected, 5, request.description);
					this->eventList->SetItem(selected, 6, wxString::Format("%d hours before", request.reminderMinutes / 60));
				});
			} 
			catch(const std::exception& e) {
				if(wxTheApp == nullptr) { return; }
				wxTheApp->CallAfter([e]() {
					wxMessageBox(wxString::Format("Failed to update event: %s", e.what()), "Error", wxOK | wxICON_ERROR);
				});
			}
		}).detach();
	}
	
	event.Skip();
}

void EventPanel::OnDeleteEvent(wxCommandEvent& event) {
	long selected = this->eventList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(selected == -1) {
		wxMessageBox("Please select an event to delete.", "Delete Event", wxOK | wxICON_WARNING, this);
		event.Skip();
		return;
	}
	int eventId = static_cast<int>(this->eventList->GetItemData(selected));

	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Event"); }

	int confirmed = wxMessageBox(
		"Are you sure you want to delete this event?",
		"Delete Event",
		wxYES_NO | wxICON_QUESTION,
		this
	);

	if(confirmed == wxYES) {
		DeleteEventRequest request;
		request.id = eventId;

		auto deleteUseCase = wxGetApp().GetInjector().create<DeleteEventUseCase>();
		auto alive = this->alive;

		std::thread([this, alive, deleteUseCase, selected, frame, request]() mutable {
			try {
				deleteUseCase.Execute(request);

				if(wxTheApp == nullptr) { return; }
				wxTheApp->CallAfter([this, alive, selected, frame]() {
					if(!*alive) { return; }
					this->eventList->DeleteItem(selected);
					if(frame) { frame->SetStatusText("Event Deleted Successfully"); }
				});
			} 
			catch(const std::exception& e) {
				if(wxTheApp == nullptr) { return; }
				wxTheApp->CallAfter([e]() {
					wxMessageBox(wxString::Format("Failed to delete event: %s", e.what()), "Error", wxOK | wxICON_ERROR);
				});
			}
		}).detach();
	}
	
	event.Skip();
}
