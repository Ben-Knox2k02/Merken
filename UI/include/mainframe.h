#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>

class DeckPanelList;
class FlashCardList;

enum {
	ID_SETTINGS = wxID_HIGHEST + 1,
	ID_STUDY_DECK,
	ID_TODAYS_PROGRESS,
	ID_CALENDAR,
	ID_AI_STUDY
};

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);

		wxBoxSizer* rootSizer;

		DeckPanelList* deckPanelList;
		FlashCardList* flashCardList;
		wxPanel* currentPanel;
		wxPanel* activePanel;
		int selectedDeckId;

		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* studyMenu;
		wxMenu* calendarMenu;
		wxMenu* AIMenu;
		wxMenu* helpMenu;

		void SwapCurrentPanel(wxPanel* newPanel);
		void ReloadDecks();
		void OnDeckSelected(int deckId);
		void ShowCardList();
		void ShowStudyDeck();
		void ShowTodaysProgress();
		void ShowCalendar();
		void ShowAiStudy();

		void OnNew(wxCommandEvent& event);
		void OnOpen(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnSettings(wxCommandEvent& event);
		void OnStudyDeck(wxCommandEvent& event);
		void OnTodaysProgress(wxCommandEvent& event);
		void OnCalendar(wxCommandEvent& event);
		void OnAiStudy(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
};

#endif
