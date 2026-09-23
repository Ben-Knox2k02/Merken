#include "study_panel.h"

StudyPanel::StudyPanel(wxWindow* parent, int deckID) : wxPanel(parent), deckID(deckID) {
    //============ ROOT PANEL ======================================== ROOT PANEL =====================
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->SetBackgroundColour(*wxWHITE);	
    this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
    //========= TOP SECTION PANEL =================================  TOP SECTION PANEL ================
    this->topPanel = new wxPanel(this);
    this->topPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->topSizer = new wxBoxSizer(wxVERTICAL);

    this->header = new wxStaticText(topPanel, wxID_ANY, "Study Session");
    this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    this->header->SetForegroundColour(*wxWHITE);

    this->topSizer->Add(this->header, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
    this->topPanel->SetSizer(topSizer);

    //============ MIDDLE SECTION PANEL =========================== MIDDLE SECTION PANEL ==============
    this->midPanel = new wxPanel(this);
    this->midPanel->SetBackgroundColour(wxColour(240, 240, 240)); 				// LIGHT GREY

    this->midSizer = new wxBoxSizer(wxVERTICAL);

    this->cardText = new wxStaticText(midPanel, wxID_ANY, "Card Text");
    this->answerCtrl = new wxTextCtrl(midPanel, wxID_ANY);
    this->progressText = new wxStaticText(midPanel, wxID_ANY, "Progress: 4 / 15");

    this->showAnswerButton = new wxButton(midPanel, wxID_ANY, "Show Answer");
    this->previousButton   = new wxButton(midPanel, wxID_ANY, "Previous Card");
    this->nextButton       = new wxButton(midPanel, wxID_ANY, "Next Card");

    wxBoxSizer* navSizer = new wxBoxSizer(wxHORIZONTAL);
    navSizer->Add(this->previousButton, 0, wxRIGHT, 10);
    navSizer->Add(this->nextButton, 0, wxRIGHT, 10);
    navSizer->Add(this->showAnswerButton, 0);

    this->midSizer->AddStretchSpacer(1);											// Center the entire middle block vertically
    this->midSizer->Add(this->cardText,     0, wxALIGN_CENTER | wxALL, 10);
    this->midSizer->Add(this->answerCtrl,   0, wxALIGN_CENTER | wxALL, 10);
    this->midSizer->Add(navSizer,           0, wxALIGN_CENTER | wxALL, 10);
    this->midSizer->Add(this->progressText, 0, wxALIGN_CENTER | wxALL, 10);
    this->midSizer->AddStretchSpacer(1);

    this->midPanel->SetSizer(this->midSizer);

    //========== BOTTOM SECTION PANEL ============================== BOTTOM SECTION PANEL =============
    this->bottomPanel = new wxPanel(this);
    //this->bottomPanel->SetBackgroundColour(wxColour(220, 220, 255)); // light blue
	this->bottomPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->bottomSizer = new wxBoxSizer(wxVERTICAL);

    this->backButton   = new wxButton(bottomPanel, wxID_ANY, "Back");
    this->resetButton  = new wxButton(bottomPanel, wxID_ANY, "Reset");
    this->changeButton = new wxButton(bottomPanel, wxID_ANY, "Change Deck");

    wxBoxSizer* bottomNav = new wxBoxSizer(wxHORIZONTAL);
    bottomNav->Add(backButton,  0, wxRIGHT, 10);
    bottomNav->Add(resetButton, 0, wxRIGHT, 10);
    bottomNav->Add(changeButton,0);

    this->bottomSizer->Add(bottomNav, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
    this->bottomPanel->SetSizer(bottomSizer);
    
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(midPanel,    1, wxEXPAND | wxALL, 0);
    this->rootSizer->Add(bottomPanel, 0, wxEXPAND | wxALL, 0);

    this->SetSizer(rootSizer);
    this->Layout();
	
	//========== BIND EVENT HANDLERS ============================== BIND EVENT HANDLERS ================
	this->Bind(wxEVT_PAINT, &StudyPanel::OnPaint, this);
	
    this->previousButton->Bind(wxEVT_BUTTON, &StudyPanel::OnPrevious, this);				
    this->nextButton->Bind(wxEVT_BUTTON, &StudyPanel::OnNext, this);
    this->showAnswerButton->Bind(wxEVT_BUTTON, &StudyPanel::OnShowAnswer, this);
	this->backButton->Bind(wxEVT_BUTTON, &StudyPanel::OnBack, this);
	this->resetButton->Bind(wxEVT_BUTTON, &StudyPanel::OnReset, this);
	this->changeButton->Bind(wxEVT_BUTTON, &StudyPanel::OnChange, this);
}

void StudyPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void StudyPanel::OnPrevious(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Previous Card"); }
	event.Skip();
}

void StudyPanel::OnNext(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Next Card"); }
	event.Skip();
}

void StudyPanel::OnShowAnswer(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Show Answer"); }
	event.Skip();
}

void StudyPanel::OnBack(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Back to Card List"); }
	event.Skip();
}

void StudyPanel::OnReset(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Reset"); }
	event.Skip();
}

void StudyPanel::OnChange(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("Change Deck"); }
	event.Skip();
}
