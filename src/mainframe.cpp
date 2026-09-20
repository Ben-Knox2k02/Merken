#include "mainframe.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "card_list_panel.h"
#include "deck_panel.h"
#include "study_panel.h"

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
      app(wxGetApp())
{
	//=========== INITIALIZATION ===================================== INITIALIZATION ================================
	this->SetIcon(wxIcon("resources/app.ico", wxBITMAP_TYPE_ICO));
	
	this->menuBar = new wxMenuBar;										// INIT MENU BAR
	 
	this->fileMenu = new wxMenu;										// FILE
	this->fileMenu->Append(wxID_NEW, "&New\tCtrl+N");		
	this->fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O");
	this->fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "&Exit\tAlt+F4");
	 
	this->editMenu = new wxMenu;										// EDIT						
	this->editMenu->Append(wxID_CUT, "&Cut\tCtrl+X");
	this->editMenu->Append(wxID_COPY, "&Copy\tCtrl+C");
	this->editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V");
	
	this->searchMenu = new wxMenu;										// SEARCH
	this->searchMenu->Append(wxID_FIND, "&Find\tCtrl+F");
	this->searchMenu->Append(wxID_REPLACE, "&Replace\tCtrl+H");
	 
	this->viewMenu = new wxMenu;										// VIEW
	this->viewMenu->Append(ID_CARDS, "Cards");
	this->viewMenu->Append(ID_STUDY, "Study");
	this->viewMenu->Append(ID_CALENDAR, "Calendar");
	this->viewMenu->Append(ID_AI, "AI");
	
	this->toolsMenu = new wxMenu;										// TOOLS
	this->toolsMenu->Append(ID_SETTINGS, "Settings");
	
	this->helpMenu = new wxMenu;										// HELP		
	this->helpMenu->Append(wxID_ABOUT, "About");
	 
	this->menuBar->Append(fileMenu, "File");							// ATTACH MENUS
	this->menuBar->Append(editMenu, "Edit");
	this->menuBar->Append(searchMenu, "Search");
	this->menuBar->Append(viewMenu, "View");
	this->menuBar->Append(toolsMenu, "Tools");
	this->menuBar->Append(helpMenu, "Help");
	
	this->shortcuts[0].Set(wxACCEL_CTRL, (int)'N', wxID_NEW);			// KEYBOARD SHORTCUTS
	this->shortcuts[1].Set(wxACCEL_CTRL, (int)'O', wxID_OPEN);
	this->shortcuts[2].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
	
	this->shortcuts[3].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
	this->shortcuts[4].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
	this->shortcuts[5].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
	
	this->shortcuts[6].Set(wxACCEL_CTRL, (int)'F', wxID_FIND);
	this->shortcuts[7].Set(wxACCEL_CTRL, (int)'H', wxID_REPLACE);
	
	wxAcceleratorTable accel(8, this->shortcuts);
	this->SetAcceleratorTable(accel);
	 
	this->SetMenuBar(this->menuBar);									// ATTACH MENU BAR / STATUS BAR
	this->CreateStatusBar();
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	
	this->deckPanel = new DeckPanel(this);
	
	this->activePanel = new wxPanel(this, wxID_ANY);
	this->activePanel->SetBackgroundColour(*wxWHITE);
	//this->SwapCurrentPanel(new CardListPanel(this->activePanel, 0));
	this->SwapCurrentPanel(new StudyPanel(this->activePanel, 0));
	
	this->rootSizer->Add(deckPanel, 0, wxEXPAND | wxALL, 0);
	this->rootSizer->Add(activePanel, 1, wxEXPAND | wxALL, 0);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS =================================
	
	this->Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);					// FILE
	this->Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnCut, this, wxID_CUT);					// EDIT
	this->Bind(wxEVT_MENU, &MainFrame::OnCopy, this, wxID_COPY);
	this->Bind(wxEVT_MENU, &MainFrame::OnPaste, this, wxID_PASTE);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnFind, this, wxID_FIND);					// SEARCH
	this->Bind(wxEVT_MENU, &MainFrame::OnReplace, this, wxID_REPLACE);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnCards, this, ID_CARDS);					// VIEW
	this->Bind(wxEVT_MENU, &MainFrame::OnStudy, this, ID_STUDY);
	this->Bind(wxEVT_MENU, &MainFrame::OnCalendar, this, ID_CALENDAR);
	this->Bind(wxEVT_MENU, &MainFrame::OnAI, this, ID_AI);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnSettings, this, ID_SETTINGS);			// TOOLS
	
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);				// HELP
		
	this->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyEvent, this);						// KEY EVENTS
					
	this->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);				// MOUSE EVENTS
	this->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);
	
	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);					// WINDOW EVENTS
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
}

void MainFrame::SwapCurrentPanel(wxPanel* newPanel) {
	if(this->currentPanel != NULL) { 
		this->currentPanel->Destroy();
		this->currentPanel = NULL;
	}
	
	this->activePanel->SetSizer(NULL);
	//this->activePanel->DestroyChildren();
	
	this->currentPanel = newPanel;
	this->currentPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->currentPanel->SetBackgroundColour(*wxWHITE);
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->currentPanel, 1, wxEXPAND);
	
	this->activePanel->SetSizer(sizer);
	this->activePanel->Layout();
	
	this->currentPanel->SetFocus();
}

void MainFrame::OnNew(wxCommandEvent& event) {						// FILE
	wxLogStatus("NEW");
	event.Skip();
}

void MainFrame::OnOpen(wxCommandEvent& event) {
	wxLogStatus("OPEN");
	event.Skip();
}

void MainFrame::OnSave(wxCommandEvent& event) {
	wxLogStatus("SAVE");
	event.Skip();
}

void MainFrame::OnExit(wxCommandEvent& event) {
	wxLogStatus("EXIT");
	Close(true);
}

void MainFrame::OnCut(wxCommandEvent& event) {						// EDIT
	wxLogStatus("CUT");
	event.Skip();
}

void MainFrame::OnCopy(wxCommandEvent& event) {
	wxLogStatus("COPY");
	event.Skip();
}

void MainFrame::OnPaste(wxCommandEvent& event) {
	wxLogStatus("PASTE");
	event.Skip();
}

void MainFrame::OnFind(wxCommandEvent& event) {						// SEARCH
	wxLogStatus("FIND");
	event.Skip();
}

void MainFrame::OnReplace(wxCommandEvent& event) {	
	wxLogStatus("REPLACE");
	event.Skip();
}

void MainFrame::OnCards(wxCommandEvent& event) {					// VIEW
	wxLogStatus("CARDS");
	this->SwapCurrentPanel(new CardListPanel(this->activePanel, 0));
	event.Skip();
}

void MainFrame::OnStudy(wxCommandEvent& event) {
	wxLogStatus("STUDY");
	this->SwapCurrentPanel(new StudyPanel(this->activePanel, 0));
	event.Skip();
}

void MainFrame::OnCalendar(wxCommandEvent& event) {
	wxLogStatus("CALENDAR");
	event.Skip();
}

void MainFrame::OnAI(wxCommandEvent& event) {
	wxLogStatus("AI");
	event.Skip();
}

void MainFrame::OnSettings(wxCommandEvent& event) {					// TOOLS
	wxLogStatus("SETTINGS");
	event.Skip();
}

void MainFrame::OnAbout(wxCommandEvent& event) {					// HELP
	wxLogStatus("ABOUT");
	event.Skip();
}

void MainFrame::OnMouseEvent(wxMouseEvent& event) {					// I/O
	if(event.LeftDown()) {
		//wxLogStatus("LMB DOWN");
	}
	if(event.LeftUp()) {
		//wxLogStatus("LMB UP");
	}
	if(event.RightDown()) {
		//wxLogStatus("RMB DOWN");
	}
	if(event.RightUp()) {
		//wxLogStatus("RMB UP");
	}
	if(event.Dragging()) {
		//wxLogStatus("DRAGGING");
	}
	if(event.Moving()) {
		//wxLogStatus("MOVING");
	}
	if(event.GetWheelRotation() != 0) {
		//wxLogStatus("SCROLL WHEEL");
	}
	event.Skip();
}

void MainFrame::OnKeyEvent(wxKeyEvent& event) {						
	if(event.GetKeyCode() == WXK_TAB) {
		wxWindow* window = (wxWindow*)event.GetEventObject();
		window->Navigate();
	}
	
	wxChar keyChar = event.GetUnicodeKey();
	if(keyChar == WXK_NONE) {
		int keyCode = event.GetKeyCode();
		wxLogStatus("Key event %c", keyCode);
	}
	else { wxLogStatus("Key event %c", keyChar); }
	event.Skip();
}

void MainFrame::OnWindowResized(wxSizeEvent& event) {				// WINDOW
	wxLogStatus("WINDOW RESIZED");
	wxSize size = event.GetSize();
	
	this->app.SetWindowWidth(size.GetWidth());
	this->app.SetWindowHeight(size.GetHeight());
	this->app.Print();
	
	event.Skip();
}

void MainFrame::OnWindowClosed(wxCloseEvent& event) {				
	wxLogStatus("WINDOW CLOSED");
	Destroy();
}

