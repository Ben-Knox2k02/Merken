#include "mainframe.h"
#include <wx/wx.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {
	InitWidgets();
	BindEventHandlers();
}

void MainFrame::InitWidgets() {
	 this->panel = new wxPanel(this);
	 
	 this->menuBar = new wxMenuBar;					// MENU BAR
	 
	 this->fileMenu = new wxMenu;						// FILE MENU
	 this->fileMenu->Append(wxID_NEW, "New");		
	 this->fileMenu->Append(wxID_OPEN, "Open");
	 this->fileMenu->Append(wxID_SAVE, "Save");
	 this->fileMenu->AppendSeparator();
	 this->fileMenu->Append(wxID_EXIT, "Exit");
	 
	 this->helpMenu = new wxMenu;						// HELP MENU
	 this->helpMenu->Append(wxID_ABOUT, "About");
	 
	 this->menuBar->Append(fileMenu, "File");			// ATTACH MENUS
	 this->menuBar->Append(helpMenu, "Help");
	 
	 this->SetMenuBar(this->menuBar);					// ATTACH MENU BAR / STATUS BAR
	 this->CreateStatusBar();
}

void MainFrame::BindEventHandlers() {
	this->Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);			// MENU EVENTS
	this->Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);			// WINDOW EVENTS
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
	
	this->panel->Bind(wxEVT_LEFT_DOWN,    &MainFrame::OnMouseEvent, this);		// MOUSE EVENTS
	this->panel->Bind(wxEVT_LEFT_UP,      &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_RIGHT_DOWN,   &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_RIGHT_UP,     &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_MOTION,       &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_MOUSEWHEEL,   &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);
	this->panel->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);

	this->panel->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyEvent, this);				// KEYBOARD EVENTS
	this->panel->SetFocus();
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
