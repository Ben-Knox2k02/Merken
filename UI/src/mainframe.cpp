#include "mainframe.h"
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "flash_card_list.h"
#include "deck_panel_list.h"
#include "study_panel.h"
#include "calendar_panel.h"
#include "ai_panel.h"
#include "ai_study_panel.h"
#include "progress_panel.h"
#include "settings_dialog.h"
#include "centered_message.h"

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
      app(wxGetApp()), currentPanel(nullptr), sidebarPanel(nullptr), selectedDeckId(0), findDialog(nullptr)
{
	std::cout << "[TRACE] Entering MainFrame constructor..." << std::endl;
	//=========== INITIALIZE WIDGETS ===================================== INITIALIZE WIDGETS ================================
	this->SetIcon(wxIcon("UI/resources/app.ico", wxBITMAP_TYPE_ICO));
	
	this->menuBar = new wxMenuBar;						// MENU BAR
	 
	this->fileMenu = new wxMenu;										// FILE
	this->fileMenu->Append(wxID_NEW, "&New\tCtrl+N");		
	this->fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O");
	this->fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "&Exit\tAlt+F4");
	 
	this->editMenu = new wxMenu;										// EDIT		
	this->editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z");
	this->editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y");
	this->editMenu->AppendSeparator();
	this->editMenu->Append(wxID_CUT, "&Cut\tCtrl+X");
	this->editMenu->Append(wxID_COPY, "&Copy\tCtrl+C");
	this->editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V");
	this->editMenu->Append(wxID_SELECTALL, "&Select All\tCtrl+A");
	
	this->searchMenu = new wxMenu;										// SEARCH
	this->searchMenu->Append(wxID_FIND, "&Find\tCtrl+F");
	this->searchMenu->Append(wxID_REPLACE, "&Replace\tCtrl+H");
	 
	this->viewMenu = new wxMenu;										// VIEW
	this->viewMenu->Append(ID_CARDS, "&Cards\tF1");
	this->viewMenu->Append(ID_STUDY, "&Study\tF2");
	this->viewMenu->Append(ID_TODAYS_PROGRESS, "Today's &Progress");
	this->viewMenu->Append(ID_CALENDAR, "&Calendar\tF3");
	this->viewMenu->Append(ID_AI, "&AI\tF4");
	
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
	
	//======== KEYBOARD SHORTCUTS ================================== KEYBOARD SHORTCUTS ======================
	this->shortcuts[0].Set(wxACCEL_CTRL, (int)'N', wxID_NEW);			
	this->shortcuts[1].Set(wxACCEL_CTRL, (int)'O', wxID_OPEN);
	this->shortcuts[2].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
	this->shortcuts[3].Set(wxACCEL_ALT,  WXK_F4,   wxID_EXIT);
	
	this->shortcuts[4].Set(wxACCEL_CTRL, (int)'Z', wxID_UNDO);
	this->shortcuts[5].Set(wxACCEL_CTRL, (int)'Y', wxID_REDO);
	this->shortcuts[6].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
	this->shortcuts[7].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
	this->shortcuts[8].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
	this->shortcuts[9].Set(wxACCEL_CTRL, (int)'A', wxID_SELECTALL);
	
	this->shortcuts[10].Set(wxACCEL_CTRL, (int)'F', wxID_FIND);
	this->shortcuts[11].Set(wxACCEL_CTRL, (int)'H', wxID_REPLACE);
	
	this->shortcuts[12].Set(wxACCEL_NORMAL, WXK_F1, ID_CARDS);
	this->shortcuts[13].Set(wxACCEL_NORMAL, WXK_F2, ID_STUDY);
	this->shortcuts[14].Set(wxACCEL_NORMAL, WXK_F3, ID_CALENDAR);
	this->shortcuts[15].Set(wxACCEL_NORMAL, WXK_F4, ID_AI);
	
	wxAcceleratorTable accel(15, this->shortcuts);
	this->SetAcceleratorTable(accel);
	
	//============ ATTACH WIDGETS ==================================== ATTACH WIDGETS ========================= 
	this->SetMenuBar(this->menuBar);									
	this->CreateStatusBar();
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	
	this->contentPanel = new wxPanel(this, wxID_ANY);
	this->SetCurrentPanel(new FlashCardList(this->contentPanel, 0));
	this->sidebarPanel = new DeckPanelList(this);
	
	this->rootSizer->Add(this->sidebarPanel, 30, wxEXPAND | wxALL, this->FromDIP(10));
	this->rootSizer->Add(this->contentPanel, 70, wxEXPAND | wxALL, 0);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS ===================
	this->Bind(wxEVT_MENU, &MainFrame::OnNew, this, wxID_NEW);					// FILE
	this->Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnUndo, this, wxID_UNDO);					// EDIT
	this->Bind(wxEVT_MENU, &MainFrame::OnRedo, this, wxID_REDO);
	this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateUndo, this, wxID_UNDO);
	this->Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdateRedo, this, wxID_REDO);
	this->Bind(wxEVT_MENU, &MainFrame::OnCut, this, wxID_CUT);					
	this->Bind(wxEVT_MENU, &MainFrame::OnCopy, this, wxID_COPY);
	this->Bind(wxEVT_MENU, &MainFrame::OnPaste, this, wxID_PASTE);
	this->Bind(wxEVT_MENU, &MainFrame::OnSelectAll, this, wxID_SELECTALL);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnFind, this, wxID_FIND);					// SEARCH
	this->Bind(wxEVT_MENU, &MainFrame::OnReplace, this, wxID_REPLACE);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnCards, this, ID_CARDS);					// VIEW
	this->Bind(wxEVT_MENU, &MainFrame::OnStudy, this, ID_STUDY);
	this->Bind(wxEVT_MENU, &MainFrame::OnTodaysProgress, this, ID_TODAYS_PROGRESS);
	this->Bind(wxEVT_MENU, &MainFrame::OnCalendar, this, ID_CALENDAR);
	this->Bind(wxEVT_MENU, &MainFrame::OnAI, this, ID_AI);
	
	this->Bind(wxEVT_MENU, &MainFrame::OnSettings, this, ID_SETTINGS);			// TOOLS
	
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);				// HELP
		
	this->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyEvent, this);						// KEY EVENTS
					
	this->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);				// MOUSE EVENTS
	this->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);
	
	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);					// WINDOW EVENTS
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
	
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

void MainFrame::OnDeckSelected(int deckId) {
	this->selectedDeckId = deckId;
	if(auto cardPanel = dynamic_cast<FlashCardList*>(this->currentPanel)) {
		cardPanel->SetDeck(deckId);
	}
}

void MainFrame::ShowCardList() {
	this->SetCurrentPanel(new FlashCardList(this->contentPanel, this->selectedDeckId));
}

void MainFrame::ShowStudyDeck() {
	if(this->selectedDeckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Study Deck", wxOK | wxICON_WARNING);
		return;
	}
	this->SetCurrentPanel(new StudyPanel(this->contentPanel, this->selectedDeckId));
}

void MainFrame::ShowTodaysProgress() {
	this->SetCurrentPanel(new ProgressPanel(this->contentPanel));
}

void MainFrame::ShowCalendar() {
	this->SetCurrentPanel(new CalendarPanel(this->contentPanel));
}

void MainFrame::ShowAiStudy() {
	if(this->selectedDeckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "AI Study", wxOK | wxICON_WARNING);
		return;
	}
	this->SetCurrentPanel(new AiStudyPanel(this->contentPanel, this->selectedDeckId));
}

void MainFrame::SetCurrentPanel(wxPanel* newPanel) {
	if(this->currentPanel != NULL) { 
		this->currentPanel->Destroy();
		this->currentPanel = NULL;
	}
	
	this->contentPanel->SetSizer(NULL);
	
	this->currentPanel = newPanel;
	this->currentPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->currentPanel->SetBackgroundColour(*wxWHITE);
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->currentPanel, 1, wxEXPAND);
	
	this->contentPanel->SetSizer(sizer);
	this->contentPanel->Layout();
	this->ApplyPanelWidths();
	
	this->currentPanel->SetFocus();
}

void MainFrame::SetSidebarPanel(wxPanel* newPanel) {
	wxLogStatus("CHANGE SIDEBAR");
}

//=============== EVENT HANDLERS ================================== EVENT HANDLERS =============

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

void MainFrame::OnUndo(wxCommandEvent& event) {						// EDIT
	wxLogStatus("UNDO");
	wxWindow* focused = wxWindow::FindFocus();
    wxTextCtrl* textCtrl = wxDynamicCast(focused, wxTextCtrl);
    if (textCtrl && textCtrl->CanUndo()) {
        textCtrl->Undo();
        return;
    }
	event.Skip();
}

void MainFrame::OnRedo(wxCommandEvent& event) {
	wxLogStatus("REDO");
	wxWindow* focused = wxWindow::FindFocus();
    wxTextCtrl* textCtrl = wxDynamicCast(focused, wxTextCtrl);
    if (textCtrl && textCtrl->CanRedo()) {
        textCtrl->Redo();
        return;
    }
	event.Skip();
}

void MainFrame::OnUpdateUndo(wxUpdateUIEvent& event) {
    wxWindow* focused = wxWindow::FindFocus();
    wxTextCtrl* textCtrl = wxDynamicCast(focused, wxTextCtrl);
    
    if (textCtrl) {
        event.Enable(textCtrl->CanUndo());
    } else {
        // Fallback: enable if active panel has undo history, or false by default
        event.Enable(false); 
    }
}

void MainFrame::OnUpdateRedo(wxUpdateUIEvent& event) {
    wxWindow* focused = wxWindow::FindFocus();
    wxTextCtrl* textCtrl = wxDynamicCast(focused, wxTextCtrl);
    
    if (textCtrl) {
        bool canRedo = textCtrl->CanRedo();
        event.Enable(canRedo);
        // wxLogStatus("CanRedo: %s", canRedo ? "TRUE" : "FALSE");
    } else {
        event.Enable(false); // FIXED: Removed the invalid textCtrl->CanRedo() call here
    }
}

void MainFrame::OnCut(wxCommandEvent& event) {						
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

void MainFrame::OnSelectAll(wxCommandEvent& event) {
	wxLogStatus("SELECT ALL");
	
	event.Skip();
}

void MainFrame::OnFind(wxCommandEvent& event) {                        // SEARCH
    wxLogStatus("FIND");

    this->findDialog = new wxFindReplaceDialog(this, &findData, "Find");
	//this->findDialog->CentreOnParent();
	this->findDialog->Show(true);
	
	if(this->findDialog && this->findDialog->IsShown()) {
        this->findDialog->SetFocus();
        return;
    }
    
    this->findDialog->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& e) {
        this->findDialog->Show(false);
		this->findDialog = NULL;
		e.Skip();
    });
    
    event.Skip();
}

void MainFrame::OnReplace(wxCommandEvent& event) {    
    wxLogStatus("REPLACE");
	
	this->findDialog = new wxFindReplaceDialog(this, &this->findData, "Replace");
    this->findData.SetFlags(wxFR_DOWN);
	//this->findDialog->CentreOnParent();
	this->findDialog->Show(true);
	
	if(this->findDialog && this->findDialog->IsShown()) {
        this->findDialog->SetFocus();
        return;
    }
    
    this->findDialog->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& e) {
        this->findDialog->Show(false);
		this->findDialog = NULL;
		e.Skip();
    });
   
    event.Skip();
}

void MainFrame::OnCards(wxCommandEvent& event) {					// VIEW
	wxLogStatus("CARDS");
	this->ShowCardList();
	event.Skip();
}

void MainFrame::OnStudy(wxCommandEvent& event) {
	wxLogStatus("STUDY");
	this->ShowStudyDeck();
	event.Skip();
}

void MainFrame::OnTodaysProgress(wxCommandEvent& event) {
	wxLogStatus("TODAY'S PROGRESS");
	this->ShowTodaysProgress();
	event.Skip();
}

void MainFrame::OnCalendar(wxCommandEvent& event) {
	wxLogStatus("CALENDAR");
	this->ShowCalendar();
	event.Skip();
}

void MainFrame::OnAI(wxCommandEvent& event) {
	wxLogStatus("AI");
	this->ShowAiStudy();
	event.Skip();
}

void MainFrame::OnSettings(wxCommandEvent& event) {					// TOOLS
	wxLogStatus("SETTINGS");
	SettingsDialog dialog(this);
	dialog.ShowModal();
	event.Skip();
}

void MainFrame::OnAbout(wxCommandEvent& event) {					// HELP
	wxLogStatus("ABOUT");
	event.Skip();
}

void MainFrame::OnMouseEvent(wxMouseEvent& event) {					// MOUSE
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

void MainFrame::OnKeyEvent(wxKeyEvent& event) {						// KEY						
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

void MainFrame::ApplyPanelWidths() {
	const int clientWidth = this->GetClientSize().GetWidth();
	if(clientWidth <= 0 || this->sidebarPanel == nullptr || this->contentPanel == nullptr) {
		return;
	}

	const int outerPad = this->FromDIP(10);
	const int deckSlot = clientWidth * 30 / 100;
	const int deckWidth = deckSlot - outerPad * 2;
	const int cardWidth = clientWidth - deckSlot;
	if(deckWidth <= 0 || cardWidth <= 0) {
		return;
	}
	this->sidebarPanel->SetMinSize(wxSize(deckWidth, -1));
	this->sidebarPanel->SetMaxSize(wxSize(deckWidth, -1));
	this->contentPanel->SetMinSize(wxSize(cardWidth, -1));
	this->contentPanel->SetMaxSize(wxSize(cardWidth, -1));
}

void MainFrame::OnWindowResized(wxSizeEvent& event) {				// WINDOW
	wxSize size = event.GetSize();
	
	this->app.SetWindowWidth(size.GetWidth());
	this->app.SetWindowHeight(size.GetHeight());
	this->ApplyPanelWidths();
	
	event.Skip();
}

void MainFrame::OnWindowClosed(wxCloseEvent& event) {				
	wxLogStatus("WINDOW CLOSED");
	Destroy();
}

