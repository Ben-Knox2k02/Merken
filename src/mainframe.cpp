#include <wx/wx.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "mainframe.h"
#include "card_list_panel.h"
#include "deck_panel.h"

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {
	//=========== INITIALIZE WIDGETS ===================================== INITIALIZE WIDGETS ================================
	
	this->menuBar = new wxMenuBar;						// MENU BAR
	 
	this->fileMenu = new wxMenu;						// FILE MENU
	this->fileMenu->Append(wxID_NEW, "New");		
	this->fileMenu->Append(wxID_OPEN, "Open");
	this->fileMenu->Append(wxID_SAVE, "Save");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "Exit");
	 
	this->editMenu = new wxMenu;						// EDIT MENU
	this->editMenu->Append(wxID_CUT, "Cut");
	this->editMenu->Append(wxID_COPY, "Copy");
	this->editMenu->Append(wxID_PASTE, "Paste");
	 
	this->studyMenu = new wxMenu;						// STUDY MENU
	 
	this->calendarMenu = new wxMenu;					// CALENDAR MENU
	 
	this->AIMenu = new wxMenu;							// AI MENU
	 
	this->helpMenu = new wxMenu;						// HELP MENU
	this->helpMenu->Append(wxID_ABOUT, "About");
	 
	this->menuBar->Append(fileMenu, "File");			// ATTACH MENUS
	this->menuBar->Append(editMenu, "Edit");
	this->menuBar->Append(studyMenu, "Study");
	this->menuBar->Append(AIMenu, "AI");
	this->menuBar->Append(helpMenu, "Help");
	 
	this->SetMenuBar(this->menuBar);					// ATTACH MENU BAR / STATUS BAR
	this->CreateStatusBar();
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	
	this->deckPanel = new DeckPanel(this);

	this->activePanel = new wxPanel(this, wxID_ANY);
	this->activePanel->SetBackgroundColour(*wxWHITE);
	this->SwapCurrentPanel(new CardListPanel(this->activePanel, 0));
	
	this->rootSizer->Add(deckPanel, 0, wxEXPAND | wxALL, 0);
	this->rootSizer->Add(activePanel, 1, wxEXPAND | wxALL, 0);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS =================================
	
	this->Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);					// MENU EVENTS
	this->Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);					// WINDOW EVENTS
	this->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);
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
	
	this->currentPanel->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyEvent, this);
	
	for(wxWindow* child : this->currentPanel->GetChildren()) {
		child->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_LEFT_UP, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_RIGHT_DOWN, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_RIGHT_UP, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_MOTION, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_MOUSEWHEEL, &MainFrame::OnMouseEvent, this);
        child->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyEvent, this);
	}
	
	this->currentPanel->SetFocus();
}

void MainFrame::OnNew(wxCommandEvent& event) {
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

void MainFrame::OnAbout(wxCommandEvent& event) {
	wxLogStatus("ABOUT");
	event.Skip();
}

void MainFrame::OnExit(wxCommandEvent& event) {
	wxLogStatus("EXIT");
	event.Skip();
}

void MainFrame::OnInputEnter(wxCommandEvent& event) {
	wxLogStatus("ENTER");
	event.Skip();
}

void MainFrame::OnWindowResized(wxSizeEvent& event) {
	wxLogStatus("WINDOW RESIZED");
	event.Skip();
}

void MainFrame::OnWindowClosed(wxCloseEvent& event) {
	wxLogStatus("WINDOW CLOSED");
	event.Skip();
}

void MainFrame::OnMouseEvent(wxMouseEvent& event) {
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
