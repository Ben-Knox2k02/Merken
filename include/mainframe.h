#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "app.h"
#include <wx/wx.h>

enum {									// CUSTOM EVENT IDs
	ID_CARDS = wxID_HIGHEST + 1,
	ID_STUDY,
	ID_CALENDAR,
	ID_AI,
	ID_SETTINGS,
};

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);
		App& app;								// GLOBAL REFERENCE TO APP
		
		wxPanel* deckPanel;
		wxPanel* activePanel;
		wxPanel* currentPanel;
		
		wxBoxSizer* rootSizer;
		
		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* searchMenu;
		wxMenu* viewMenu;
		wxMenu* toolsMenu;
		wxMenu* helpMenu;
		
		void SwapCurrentPanel(wxPanel* newPanel);
		
		void OnNew(wxCommandEvent& event);				// FILE
		void OnOpen(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		
		void OnCut(wxCommandEvent& event);				// EDIT
		void OnCopy(wxCommandEvent& event);
		void OnPaste(wxCommandEvent& event);
		
		void OnFind(wxCommandEvent& event);			// SEARCH
		void OnReplace(wxCommandEvent& event);
		
		void OnCards(wxCommandEvent& event);			// VIEW
		void OnStudy(wxCommandEvent& event);
		void OnCalendar(wxCommandEvent& event);
		void OnAI(wxCommandEvent& event);
		
		void OnSettings(wxCommandEvent& event);		// TOOLS
		
		void OnAbout(wxCommandEvent& event);			// HELP
		
		void OnMouseEvent(wxMouseEvent& event);			// I/O
		void OnKeyEvent(wxKeyEvent& event);
		
		void OnWindowResized(wxSizeEvent& event);		// WINDOW
		void OnWindowClosed(wxCloseEvent& event);
		
};

#endif
