#include "calendar_panel.h"

CalendarPanel::CalendarPanel(wxWindow* parent)
	: wxPanel(parent),
	  alive(std::make_shared<bool>(true)) {
	//============ ROOT PANEL ======================================== ROOT PANEL =====================
	App& app = wxGetApp();
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
	this->calendar->Bind(wxEVT_CALENDAR_SEL_CHANGED, &CalendarPanel::OnDateSelected, this);
	this->backButton->Bind(wxEVT_BUTTON, &CalendarPanel::OnBack, this);
}

CalendarPanel::~CalendarPanel() {		
	*this->alive = false;
}

void CalendarPanel::OnPaint(wxPaintEvent& event) {	
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void CalendarPanel::OnDateSelected(wxCalendarEvent& event) {
	wxDateTime selectedDate = this->calendar->GetDate();
	wxString dateString = selectedDate.Format("%m/%d/%Y");
	
	if(auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->SetSelectedDate(selectedDate);
		frame->SetStatusText("Selected Date: " + dateString);
	}
	
	event.Skip();
}

void CalendarPanel::OnBack(wxCommandEvent& event) {
	/*if(auto* frame = dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
		frame->ShowCardList();
	} */
	event.Skip();
}
