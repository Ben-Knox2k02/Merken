#include "calendar_panel.h"

CalendarPanel::CalendarPanel(wxWindow* parent) : wxPanel(parent) {
	//============ ROOT PANEL ======================================== ROOT PANEL =====================
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->SetBackgroundColour(*wxWHITE);	
    this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
    //========= TOP SECTION PANEL =================================  TOP SECTION PANEL ================
    this->topPanel = new wxPanel(this);
    this->topPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->topSizer = new wxBoxSizer(wxVERTICAL);

    this->header = new wxStaticText(this->topPanel, wxID_ANY, "Calendar");
    this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    this->header->SetForegroundColour(*wxWHITE);

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
		wxDefaultSize,
		wxCAL_SHOW_HOLIDAYS | wxCAL_MONDAY_FIRST
	);
	
	this->midSizer->Add(this->calendar, 1, wxEXPAND | wxALL, 10);
    this->midPanel->SetSizer(this->midSizer);

    //========== BOTTOM SECTION PANEL ============================== BOTTOM SECTION PANEL =============
    this->bottomPanel = new wxPanel(this);
	this->bottomPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->bottomSizer = new wxBoxSizer(wxVERTICAL);
	
	this->createEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Create Event");	
	this->editEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Edit Event");
	this->deleteEventButton = new wxButton(this->bottomPanel, wxID_ANY, "Delete Event");
	
	wxBoxSizer* bottomNav = new wxBoxSizer(wxHORIZONTAL);
    bottomNav->Add(createEventButton,  0, wxRIGHT, 20);
    bottomNav->Add(editEventButton, 0, wxRIGHT, 20);
    bottomNav->Add(deleteEventButton, 0);

    this->bottomSizer->Add(bottomNav, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    this->bottomPanel->SetSizer(bottomSizer);
    
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(this->topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(this->midPanel,    1, wxEXPAND | wxALL, 0);
    this->rootSizer->Add(this->bottomPanel, 0, wxEXPAND | wxALL, 0);

    this->SetSizer(this->rootSizer);
    this->Layout();
	
	//========== BIND EVENT HANDLERS ============================== BIND EVENT HANDLERS ================
	this->Bind(wxEVT_PAINT, &CalendarPanel::OnPaint, this);
	
	this->createEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnCreateEvent, this);				
    this->editEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnEditEvent, this);
    this->deleteEventButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnDeleteEvent, this);
	
}

void CalendarPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void CalendarPanel::OnCreateEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Create Event"); }
	
	EventDialog dialog(this);
	dialog.CentreOnParent();
	
	if(dialog.ShowModal() == wxID_OK) {
		
	}
	
	event.Skip();
}

void CalendarPanel::OnEditEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Edit Event"); }
	event.Skip();
}

void CalendarPanel::OnDeleteEvent(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Delete Event"); }
	event.Skip();
}
