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
#include "theme.h"
#include "../../Application/UseCases/Profile/GetUserProfile/get_user_profile_usecase.h"
#include <wx/statline.h>

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
      app(wxGetApp()), currentPanel(nullptr), sidebarPanel(nullptr), selectedDeckId(0), selectedCardCount(0), studying(false), findDialog(nullptr)
{
	std::cout << "[TRACE] Entering MainFrame constructor..." << std::endl;
	//=========== INITIALIZE WIDGETS ===================================== INITIALIZE WIDGETS ================================
	this->SetIcon(wxIcon("UI/resources/app.ico", wxBITMAP_TYPE_ICO));
	
	this->menuBar = new wxMenuBar;						// MENU BAR
	 
	this->fileMenu = new wxMenu;										// FILE
	this->fileMenu->Append(wxID_NEW, "Add Deck");
	this->fileMenu->Append(ID_EDIT_DECK, "Edit Deck");
	this->fileMenu->Append(ID_DELETE_DECK, "Delete Deck");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_SETTINGS, "Settings");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "Exit");
	 
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
	 
	this->viewMenu = nullptr;
	this->studyMenu = new wxMenu;										// STUDY
	this->studyMenu->Append(ID_CARDS, "View Cards\tF1");
	this->studyMenu->Append(ID_STUDY, "Study Deck\tF2");
	this->studyMenu->Append(ID_AI, "AI Study Deck\tF3");
	this->studyMenu->Append(ID_TODAYS_PROGRESS, "View Progress\tF4");

	this->calendarMenu = new wxMenu;									// CALENDAR
	this->calendarMenu->Append(ID_CALENDAR, "View Calendar\tF5");

	this->toolsMenu = nullptr;
	
	this->helpMenu = new wxMenu;										// HELP		
	this->helpMenu->Append(wxID_ABOUT, "About Merken");
	 
	this->menuBar->Append(fileMenu, "File");							// ATTACH MENUS
	this->menuBar->Append(editMenu, "Edit");
	this->menuBar->Append(searchMenu, "Search");
	this->menuBar->Append(studyMenu, "Study");
	this->menuBar->Append(calendarMenu, "Calendar");
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
	this->shortcuts[14].Set(wxACCEL_NORMAL, WXK_F3, ID_AI);
	this->shortcuts[15].Set(wxACCEL_NORMAL, WXK_F4, ID_TODAYS_PROGRESS);
	this->shortcuts[16].Set(wxACCEL_NORMAL, WXK_F5, ID_CALENDAR);
	
	wxAcceleratorTable accel(NUM_SHORTCUTS, this->shortcuts);
	this->SetAcceleratorTable(accel);
	
	//============ ATTACH WIDGETS ==================================== ATTACH WIDGETS ========================= 
	this->SetMenuBar(this->menuBar);
	this->CreateStatusBar();
	this->SetBackgroundColour(Theme::Get().color.window);
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	
	this->contentPanel = new wxPanel(this, wxID_ANY);
	this->contentPanel->SetBackgroundColour(Theme::Get().color.window);
	this->SetCurrentPanel(new FlashCardList(this->contentPanel, 0));
	this->sidebarPanel = new DeckPanelList(this);

	const int edge = this->FromDIP(Theme::Get().size.panelPad);
	wxStaticLine* paneSplit = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	this->rootSizer->Add(this->sidebarPanel, 20, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT, edge);
	this->rootSizer->Add(paneSplit, 0, wxEXPAND | wxTOP | wxBOTTOM, edge);
	this->rootSizer->Add(this->contentPanel, 80, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, edge);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS ===================
	this->Bind(wxEVT_MENU, &MainFrame::OnAddDeck, this, wxID_NEW);				// FILE
	this->Bind(wxEVT_MENU, &MainFrame::OnEditDeck, this, ID_EDIT_DECK);
	this->Bind(wxEVT_MENU, &MainFrame::OnDeleteDeck, this, ID_DELETE_DECK);
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
	
	this->Bind(wxEVT_SIZE, &MainFrame::OnWindowResized, this);					// WINDOW EVENTS
	this->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseEvent, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
	this->Bind(wxEVT_SYS_COLOUR_CHANGED, &MainFrame::OnSysColourChanged, this);
	Theme::ApplyTitleBar(this);
}

void MainFrame::OnDeckSelected(int deckId) {
	this->selectedDeckId = deckId;
	this->UpdateDeckMenus();
	if(auto cardPanel = dynamic_cast<FlashCardList*>(this->currentPanel)) {
		cardPanel->SetDeck(deckId);
	}
}

void MainFrame::SetStudying(bool active) {
	this->studying = active;
	if (this->sidebarPanel != nullptr) {
		this->sidebarPanel->Enable(!active);
	}
	this->UpdateDeckMenus();
}

void MainFrame::UpdateDeckMenus() {
	if (this->fileMenu == nullptr) {
		return;
	}
	bool guiding = false;
	if (!this->studying) {
		auto useCase = wxGetApp().GetInjector().create<GetUserProfileUseCase>();
		GetUserProfileResponse profile = useCase.Execute();
		guiding = profile.ok && !profile.guideFinished;
	}
	const bool unlocked = !this->studying && !guiding;
	bool hasDecks = false;
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		hasDecks = decks->HasDecks();
	}
	const bool canEditDeck = hasDecks && this->selectedDeckId != 0 && unlocked;
	const bool canStudy = this->selectedCardCount > 0 && unlocked;
	this->fileMenu->Enable(wxID_NEW, unlocked);
	this->fileMenu->Enable(ID_EDIT_DECK, canEditDeck);
	this->fileMenu->Enable(ID_DELETE_DECK, canEditDeck);
	this->fileMenu->Enable(ID_SETTINGS, unlocked);
	if (this->studyMenu != nullptr) {
		this->studyMenu->Enable(ID_CARDS, unlocked);
		this->studyMenu->Enable(ID_STUDY, canStudy);
		this->studyMenu->Enable(ID_AI, canStudy);
		this->studyMenu->Enable(ID_TODAYS_PROGRESS, unlocked);
	}
	if (this->calendarMenu != nullptr) {
		this->calendarMenu->Enable(ID_CALENDAR, unlocked);
	}
}

void MainFrame::SetSelectedCardCount(int count) {
	this->selectedCardCount = count;
	this->UpdateDeckMenus();
}

void MainFrame::ReloadDecks() {
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->LoadDecks();
		this->selectedDeckId = decks->GetSelectedDeckId();
		this->UpdateDeckMenus();
	}
}

void MainFrame::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	if (this->contentPanel != nullptr) {
		this->contentPanel->SetBackgroundColour(theme.color.window);
	}
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->ApplyTheme();
	}
	if (auto* cards = dynamic_cast<FlashCardList*>(this->currentPanel)) {
		cards->ApplyTheme();
	} else if (this->currentPanel != nullptr) {
		this->currentPanel->SetBackgroundColour(theme.color.window);
		this->currentPanel->Refresh();
	}
	Theme::ApplyTitleBar(this);
	this->Refresh();
	this->Update();
}

void MainFrame::OnSysColourChanged(wxSysColourChangedEvent& event) {
	this->ApplyTheme();
	event.Skip();
}

void MainFrame::ShowCardList() {
	this->SetStudying(false);
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->RefreshGuide();
	}
	this->SetCurrentPanel(new FlashCardList(this->contentPanel, this->selectedDeckId));
	this->UpdateDeckMenus();
}

void MainFrame::ShowStudyDeck() {
	if(this->selectedDeckId == 0) {
		ShowCenteredMessage(this, "Select a deck first.", "Study Deck", wxOK | wxICON_WARNING);
		return;
	}
	this->SetCurrentPanel(new StudyPanel(this->contentPanel, this->selectedDeckId));
	this->SetStudying(true);
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
	this->SetStudying(true);
}

void MainFrame::SetCurrentPanel(wxPanel* newPanel) {
	if(this->currentPanel != NULL) { 
		this->currentPanel->Destroy();
		this->currentPanel = NULL;
	}
	
	this->contentPanel->SetSizer(NULL);
	
	this->currentPanel = newPanel;
	this->currentPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->currentPanel->SetBackgroundColour(Theme::Get().color.window);
	
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

void MainFrame::OnAddDeck(wxCommandEvent& event) {
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->OnAddDeck(event);
	}
}

void MainFrame::OnEditDeck(wxCommandEvent& event) {
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->OnEditDeck(event);
	}
}

void MainFrame::OnDeleteDeck(wxCommandEvent& event) {
	if (auto* decks = dynamic_cast<DeckPanelList*>(this->sidebarPanel)) {
		decks->OnDeleteDeck(event);
	}
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
	wxWindow* focused = wxWindow::FindFocus();
	wxTextCtrl* textCtrl = wxDynamicCast(focused, wxTextCtrl);
	if (textCtrl) {
		textCtrl->SelectAll();
		return;
	}
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

void MainFrame::OnAbout(wxCommandEvent&) {
	wxDialog dialog(
		this,
		wxID_ANY,
		"About Merken",
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE
	);

	wxStaticText* name = new wxStaticText(&dialog, wxID_ANY, "Merken");
	wxFont nameFont = name->GetFont();
	nameFont.SetPointSize(nameFont.GetPointSize() + 3);
	name->SetFont(nameFont);

	wxStaticText* description = new wxStaticText(
		&dialog,
		wxID_ANY,
		"Merken is a flash-card study app. Create decks, add cards, and review them "
		"with spaced practice or AI-guided study sessions."
	);
	description->Wrap(dialog.FromDIP(360));

	wxButton* okButton = new wxButton(&dialog, wxID_OK);
	ApplyButtonPadding(okButton);
	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	buttons->AddButton(okButton);
	buttons->Realize();

	dialog.Bind(wxEVT_BUTTON, [&dialog](wxCommandEvent& event) {
		const int id = event.GetId();
		if (id == wxID_OK || id == wxID_CANCEL) {
			dialog.EndModal(id);
			return;
		}
		event.Skip();
	});

	const Theme& theme = Theme::Get();
	const int pad = dialog.FromDIP(16);
	wxBoxSizer* root = new wxBoxSizer(wxVERTICAL);
	root->Add(name, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
	root->Add(description, 0, wxEXPAND | wxALL, pad);
	root->Add(buttons, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

	dialog.SetSizer(root);
	dialog.SetMinSize(dialog.FromDIP(wxSize(420, 160)));
	theme.StyleDialog(&dialog);
	description->SetForegroundColour(theme.color.secondaryLabel);
	dialog.Fit();
	dialog.Layout();
	dialog.CentreOnParent();
	dialog.ShowModal();
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

	const int edge = this->FromDIP(Theme::Get().size.panelPad);
	const int gap = this->FromDIP(Theme::Get().space.md);
	const int inner = clientWidth - edge - gap - edge;
	if(inner <= 0) {
		return;
	}
	const int deckWidth = inner * 20 / 100;
	const int cardWidth = inner - deckWidth;
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

