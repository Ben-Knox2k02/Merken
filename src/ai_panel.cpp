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

    this->topSizer->Add(this->header, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
    this->topPanel->SetSizer(this->topSizer);

    //============ MIDDLE SECTION PANEL =========================== MIDDLE SECTION PANEL ==============
    this->midPanel = new wxPanel(this);
    this->midPanel->SetBackgroundColour(wxColour(240, 240, 240)); 				// LIGHT GREY

    this->midSizer = new wxBoxSizer(wxVERTICAL);

    this->midPanel->SetSizer(this->midSizer);

    //========== BOTTOM SECTION PANEL ============================== BOTTOM SECTION PANEL =============
    this->bottomPanel = new wxPanel(this);
    //this->bottomPanel->SetBackgroundColour(wxColour(220, 220, 255)); // light blue
	this->bottomPanel->SetBackgroundColour(wxColour(*wxBLUE));

    this->bottomSizer = new wxBoxSizer(wxVERTICAL);

    this->bottomPanel->SetSizer(this->bottomSizer);
    
	//========== ADD PANELS TO ROOT SIZER ======================== ADD PANELS TO ROOT SIZER ============ 
    this->rootSizer->Add(this->topPanel,    0, wxEXPAND | wxALL, 0);			
    this->rootSizer->Add(this->midPanel,    1, wxEXPAND | wxALL, 0);
    this->rootSizer->Add(this->bottomPanel, 0, wxEXPAND | wxALL, 0);

    this->SetSizer(this->rootSizer);
    this->Layout();
	
	//========== BIND EVENT HANDLERS ============================== BIND EVENT HANDLERS ================
	this->Bind(wxEVT_PAINT, &AIPanel::OnPaint, this);
	
}

void AIPanel::OnPaint(wxPaintEvent& event) {				// PAINT METHOD FOR DOUBLE BUFFERING
	wxAutoBufferedPaintDC dc(this);
	dc.SetBrush(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
}
