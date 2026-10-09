#include "ai_panel.h"

AIPanel::AIPanel(wxWindow* parent) : wxPanel(parent) {
	//============ ROOT PANEL ======================================== ROOT PANEL =====================
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->SetBackgroundColour(*wxWHITE);	
    this->rootSizer = new wxBoxSizer(wxVERTICAL);
	
    //========= TOP SECTION PANEL =================================  TOP SECTION PANEL ================
    this->topPanel = new wxPanel(this);
    this->topPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->topSizer = new wxBoxSizer(wxVERTICAL);

    this->header = new wxStaticText(this->topPanel, wxID_ANY, "AI");
    this->header->SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    this->header->SetForegroundColour(*wxWHITE);

    this->topSizer->Add(this->header, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    this->topPanel->SetSizer(this->topSizer);

    //========== BOTTOM SECTION PANEL ============================== BOTTOM SECTION PANEL =============
    this->bottomPanel = new wxPanel(this);
	this->bottomPanel->SetBackgroundColour(wxColour(240, 240, 240));

    this->bottomSizer = new wxBoxSizer(wxVERTICAL);
	
	this->convoCtrl = new wxTextCtrl(this->bottomPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);	
	this->promptCtrl = new wxTextCtrl(this->bottomPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	this->promptCtrl->SetHint("Ask Gemini to generate cards, explain concepts, or create quizzes...");
	this->submitButton = new wxButton(this->bottomPanel, wxID_ANY, "Submit");
	
	wxBoxSizer* inputSizer = new wxBoxSizer(wxHORIZONTAL);
	inputSizer->Add(this->promptCtrl, 1, wxEXPAND | wxRIGHT, 10);
	inputSizer->Add(this->submitButton, 0);
	
	this->bottomSizer->Add(this->convoCtrl, 1, wxEXPAND | wxALL, 10);
	this->bottomSizer->Add(inputSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    this->bottomPanel->SetSizer(this->bottomSizer);
    
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(this->topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(this->bottomPanel, 1, wxEXPAND | wxALL, 0);

    this->SetSizer(this->rootSizer);
    this->Layout();
	
	//========== BIND EVENT HANDLERS ============================== BIND EVENT HANDLERS ================
	this->Bind(wxEVT_PAINT, &AIPanel::OnPaint, this);
	
	this->promptCtrl->Bind(wxEVT_TEXT_ENTER, &AIPanel::OnSubmit, this);
	this->submitButton->Bind(wxEVT_BUTTON, &AIPanel::OnSubmit, this);
	
}

void AIPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}

void AIPanel::OnSubmit(wxCommandEvent& event) {
	wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
	if(frame) { frame->SetStatusText("ASK GEMINI"); }
	event.Skip();
}
