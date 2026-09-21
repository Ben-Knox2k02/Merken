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
#include "theme.h"
#include "icon_button.h"
#include <wx/aboutdlg.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE), selectedDeckId(0) {
	//=========== INITIALIZE WIDGETS ===================================== INITIALIZE WIDGETS ================================
	
	this->menuBar = new wxMenuBar;						// MENU BAR
	 
	this->fileMenu = new wxMenu;
	this->fileMenu->Append(wxID_NEW, "Add Deck");
	this->fileMenu->Append(ID_EDIT_DECK, "Edit Deck");
	this->fileMenu->Append(ID_DELETE_DECK, "Delete Deck");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(ID_SETTINGS, "Settings");
	this->fileMenu->AppendSeparator();
	this->fileMenu->Append(wxID_EXIT, "Exit");
	 
	this->editMenu = new wxMenu;
	this->editMenu->Append(wxID_CUT, "Cut");
	this->editMenu->Append(wxID_COPY, "Copy");
	this->editMenu->Append(wxID_PASTE, "Paste");
	 
	this->studyMenu = new wxMenu;
	this->studyMenu->Append(ID_STUDY_DECK, "Study Deck");
	this->studyMenu->Append(ID_AI_STUDY, "AI Study Deck");
	this->studyMenu->Append(ID_TODAYS_PROGRESS, "Today's Progress");
	 
	this->calendarMenu = new wxMenu;
	this->calendarMenu->Append(ID_CALENDAR, "View Calendar");
	 
	this->helpMenu = new wxMenu;
	this->helpMenu->Append(wxID_ABOUT, "About Merken");
	 
	this->menuBar->Append(fileMenu, "File");
	this->menuBar->Append(editMenu, "Edit");
	this->menuBar->Append(studyMenu, "Study");
	this->menuBar->Append(calendarMenu, "Calendar");
	this->menuBar->Append(helpMenu, "Help");
	 
	this->SetMenuBar(this->menuBar);
	this->ApplyMenuIcons();
	this->SetBackgroundColour(Theme::Get().color.window);
	
	this->rootSizer = new wxBoxSizer(wxHORIZONTAL);
	this->currentPanel = nullptr;
	this->flashCardList = nullptr;
	
	this->deckPanelList = new DeckPanelList(this);

	this->activePanel = new wxPanel(this, wxID_ANY);
	this->activePanel->SetBackgroundColour(Theme::Get().color.window);
	this->flashCardList = new FlashCardList(this->activePanel, 0);
	this->SwapCurrentPanel(this->flashCardList);
	this->OnDeckSelected(this->deckPanelList->GetSelectedDeckId());
	this->UpdateDeckMenus();

	const int pad = Theme::Get().size.panelPad;
	this->rootSizer->Add(this->deckPanelList, 0, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT, pad);
	this->rootSizer->AddSpacer(Theme::Get().space.md);
	this->rootSizer->Add(this->activePanel, 1, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, pad);
	
	this->SetSizer(this->rootSizer);
	this->Layout();
	
	//========= BIND EVENT HANDLERS ==================================== BIND EVENT HANDLERS =================================
	
	this->Bind(wxEVT_MENU, &MainFrame::OnAddDeck, this, wxID_NEW);
	this->Bind(wxEVT_MENU, &MainFrame::OnEditDeck, this, ID_EDIT_DECK);
	this->Bind(wxEVT_MENU, &MainFrame::OnDeleteDeck, this, ID_DELETE_DECK);
	this->Bind(wxEVT_MENU, &MainFrame::OnSettings, this, ID_SETTINGS);
	this->Bind(wxEVT_MENU, &MainFrame::OnStudyDeck, this, ID_STUDY_DECK);
	this->Bind(wxEVT_MENU, &MainFrame::OnTodaysProgress, this, ID_TODAYS_PROGRESS);
	this->Bind(wxEVT_MENU, &MainFrame::OnCalendar, this, ID_CALENDAR);
	this->Bind(wxEVT_MENU, &MainFrame::OnAiStudy, this, ID_AI_STUDY);
	this->Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
	this->Bind(wxEVT_SYS_COLOUR_CHANGED, &MainFrame::OnSysColourChanged, this);
}

void MainFrame::OnDeckSelected(int deckId) {
	this->selectedDeckId = deckId;
	this->UpdateDeckMenus();
	if (this->flashCardList != nullptr && this->currentPanel == this->flashCardList) {
		this->flashCardList->SetDeck(deckId);
	}
}

void MainFrame::UpdateDeckMenus() {
	const bool hasDeck = this->selectedDeckId != 0;
	this->fileMenu->Enable(ID_EDIT_DECK, hasDeck);
	this->fileMenu->Enable(ID_DELETE_DECK, hasDeck);
}

void MainFrame::ApplyMenuIcons() {
	const bool invert = Theme::IsDarkAppearance();
	auto setIcon = [this, invert](int id, const wxString& path) {
		wxMenuItem* item = this->menuBar->FindItem(id);
		if (item != nullptr) {
			item->SetBitmap(IconButton::LoadIconBundle(path, 16, invert));
		}
	};
	setIcon(wxID_NEW, "UI/assets/add_icon.png");
	setIcon(ID_EDIT_DECK, "UI/assets/edit_icon.png");
	setIcon(ID_DELETE_DECK, "UI/assets/delete_icon.png");
	setIcon(ID_STUDY_DECK, "UI/assets/study_icon.png");
	setIcon(ID_AI_STUDY, "UI/assets/ai_study_icon.png");
}

void MainFrame::ReloadDecks() {
	if (this->deckPanelList != nullptr) {
		this->deckPanelList->LoadDecks();
		this->selectedDeckId = this->deckPanelList->GetSelectedDeckId();
		this->UpdateDeckMenus();
	}
}

void MainFrame::ApplyTheme() {
	const Theme& theme = Theme::Get();
	this->SetBackgroundColour(theme.color.window);
	if (this->activePanel != nullptr) {
		this->activePanel->SetBackgroundColour(theme.color.window);
	}
	if (this->deckPanelList != nullptr) {
		this->deckPanelList->ApplyTheme();
	}
	if (this->flashCardList != nullptr) {
		this->flashCardList->ApplyTheme();
	}
	this->ApplyMenuIcons();
	if (this->currentPanel != nullptr && this->currentPanel != this->flashCardList) {
		this->currentPanel->SetBackgroundColour(theme.color.window);
		this->currentPanel->Refresh();
	}
	this->Refresh();
}

void MainFrame::OnSysColourChanged(wxSysColourChangedEvent& event) {
	this->ApplyTheme();
	event.Skip();
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

void MainFrame::OnAddDeck(wxCommandEvent& event) {
	if (this->deckPanelList != nullptr) {
		this->deckPanelList->OnAddDeck(event);
	}
}

void MainFrame::OnEditDeck(wxCommandEvent& event) {
	if (this->deckPanelList != nullptr) {
		this->deckPanelList->OnEditDeck(event);
	}
}

void MainFrame::OnDeleteDeck(wxCommandEvent& event) {
	if (this->deckPanelList != nullptr) {
		this->deckPanelList->OnDeleteDeck(event);
	}
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

void MainFrame::OnAbout(wxCommandEvent&) {
	wxAboutDialogInfo info;
	info.SetName("Merken");
	info.SetDescription(
		"Merken is a flash-card study app. Create decks, add cards, and review them "
		"with spaced practice or AI-guided study sessions."
	);
	wxAboutBox(info, this);
}

void MainFrame::OnExit(wxCommandEvent& event) {
	event.Skip();
}
