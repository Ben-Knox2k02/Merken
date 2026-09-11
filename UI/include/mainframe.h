#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>

class DeckPanel;
class CardListPanel;

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);

		wxBoxSizer* rootSizer;

		DeckPanel* deckPanel;
		CardListPanel* cardListPanel;
		wxPanel* currentPanel;
		wxPanel* activePanel;

		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* studyMenu;
		wxMenu* calendarMenu;
		wxMenu* AIMenu;
		wxMenu* helpMenu;

		void SwapCurrentPanel(wxPanel* newPanel);
		void OnDeckSelected(int deckId);

		void OnNew(wxCommandEvent& event);
		void OnOpen(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnInputEnter(wxCommandEvent& event);

		void OnWindowResized(wxSizeEvent& event);
		void OnWindowClosed(wxCloseEvent& event);
		void OnMouseEvent(wxMouseEvent& event);
		void OnKeyEvent(wxKeyEvent& event);
};

#endif
