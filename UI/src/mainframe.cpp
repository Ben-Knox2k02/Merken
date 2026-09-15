#include <wx/wx.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "mainframe.h"
#include "flash_card_list.h"
#include "deck_panel_list.h"
#include "study_panel.h"
#include "progress_panel.h"
#include "calendar_panel.h"
#include "ai_study_panel.h"
#include "settings_dialog.h"
#include "centered_message.h"

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE), selectedDeckId(0) {
	//=========== INITIALIZE WIDGETS ===================================== INITIALIZE WIDGETS ================================
	
	this->menuBar = new wxMenuBar;						// MENU BAR
	 
	this->fileMenu = new wxMenu;						// FILE MENU
	this->fileMenu->Append(wxID_NEW, "New");		
	this->fileMenu->Append(wxID_OPEN, "Open");
	this->fileMenu->Append(wxID_SAVE, "Save");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_SETTINGS, "Settings");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "Exit");
	 
	this->editMenu = new wxMenu;						// EDIT MENU
	this->editMenu->Append(wxID_CUT, "Cut");
	this->editMenu->Append(wxID_COPY, "Copy");
	this->editMenu->Append(wxID_PASTE, "Paste");
	 
	this->studyMenu = new wxMenu;						// STUDY MENU
	this->studyMenu->Append(ID_STUDY_DECK, "Study Deck");
	this->studyMenu->Append(ID_TODAYS_PROGRESS, "Today's Progress");
	 
	this->calendarMenu = new wxMenu;					// CALENDAR MENU
	this->calendarMenu->Append(ID_CALENDAR, "View Calendar");
	 
	this->AIMenu = new wxMenu;							// AI MENU
	this->AIMenu->Append(ID_AI_STUDY, "Start test");
	 
	this->helpMenu = new wxMenu;						// HELP MENU
	this->helpMenu->Append(wxID_ABOUT, "About");
	 
	this->menuBar->Append(fileMenu, "File");			// ATTACH MENUS
	this->menuBar->Append(editMenu, "Edit");
	this->menuBar->Append(studyMenu, "Study");
	this->menuBar->Append(calendarMenu, "Calendar");
	this->menuBar->Append(AIMenu, "AI");
	this->menuBar->Append(helpMenu, "Help");
	 
	this->SetMenuBar(this->menuBar);					// ATTACH MENU BAR / STATUS BAR
	this->CreateStatusBar();
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	this->currentPanel = nullptr;
	this->flashCardList = nullptr;
	
	this->deckPanelList = new DeckPanelList(this);

	this->activePanel = new wxPanel(this, wxID_ANY);
	this->flashCardList = new FlashCardList(this->activePanel, 0);
	this->SwapCurrentPanel(this->flashCardList);
	this->OnDeckSelected(this->deckPanelList->GetSelectedDeckId());
	
	this->rootSizer->Add(this->deckPanelList, 0, wxEXPAND | wxALL, 0);
	this->rootSizer->Add(activePanel, 1, wxEXPAND | wxALL, 0);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS =================================
	
	this->Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);					// MENU EVENTS
	this->Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
	this->Bind(wxEVT_MENU, &MainFrame::OnSettings, this, ID_SETTINGS);
	this->Bind(wxEVT_MENU, &MainFrame::OnStudyDeck, this, ID_STUDY_DECK);
	this->Bind(wxEVT_MENU, &MainFrame::OnTodaysProgress, this, ID_TODAYS_PROGRESS);
	this->Bind(wxEVT_MENU, &MainFrame::OnCalendar, this, ID_CALENDAR);
	this->Bind(wxEVT_MENU, &MainFrame::OnAiStudy, this, ID_AI_STUDY);
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);					// WINDOW EVENTS
	this->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
}

void MainFrame::OnDeckSelected(int deckId) {
	this->selectedDeckId = deckId;
	if (this->flashCardList != nullptr && this->currentPanel == this->flashCardList) {
		this->flashCardList->SetDeck(deckId);
	}
}

void MainFrame::SwapCurrentPanel(wxPanel* newPanel) {
	if (this->currentPanel != nullptr && this->currentPanel != newPanel) {
		if (this->currentPanel == this->flashCardList) {
			this->flashCardList = nullptr;
		}
		this->currentPanel->Destroy();
		this->currentPanel = nullptr;
	}

	this->activePanel->SetSizer(nullptr);
	this->currentPanel = newPanel;

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->currentPanel, 1, wxEXPAND);

	this->activePanel->SetSizer(sizer);
	this->activePanel->Layout();

	this->currentPanel->SetFocus();
}

void MainFrame::ShowCardList() {
	this->flashCardList = new FlashCardList(this->activePanel, this->selectedDeckId);
	this->SwapCurrentPanel(this->flashCardList);
}

void MainFrame::ShowStudyDeck() {
	if (this->selectedDeckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Study Deck", wxOK | wxICON_WARNING);
		return;
	}
	this->SwapCurrentPanel(new StudyPanel(this->activePanel, this->selectedDeckId));
}

void MainFrame::ShowTodaysProgress() {
	this->SwapCurrentPanel(new ProgressPanel(this->activePanel));
}

void MainFrame::ShowCalendar() {
	this->SwapCurrentPanel(new CalendarPanel(this->activePanel));
}

void MainFrame::ShowAiStudy() {
	if (this->selectedDeckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "AI Study", wxOK | wxICON_WARNING);
		return;
	}
	this->SwapCurrentPanel(new AiStudyPanel(this->activePanel, this->selectedDeckId));
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

void MainFrame::OnSettings(wxCommandEvent&) {
	SettingsDialog dialog(this);
	dialog.ShowModal();
}

void MainFrame::OnStudyDeck(wxCommandEvent&) {
	this->ShowStudyDeck();
}

void MainFrame::OnTodaysProgress(wxCommandEvent&) {
	this->ShowTodaysProgress();
}

void MainFrame::OnCalendar(wxCommandEvent&) {
	this->ShowCalendar();
}

void MainFrame::OnAiStudy(wxCommandEvent&) {
	this->ShowAiStudy();
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
