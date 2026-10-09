#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "app.h"
#include <wx/wx.h>

#define NUM_SHORTCUTS 16

enum {									// CUSTOM IDs
	ID_CARDS = wxID_HIGHEST + 1,
	ID_STUDY,
	ID_CALENDAR,
	ID_AI,
	ID_SETTINGS,
	ID_SHOW_ANSWER,
	ID_PREVIOUS_CARD,
	ID_NEXT_CARD
};

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);
		App& app;

		wxBoxSizer* rootSizer;

		wxPanel* sidebarPanel;		
		wxPanel* contentPanel;		// HUSK TO HOLD currentPanel
		wxPanel* currentPanel;

		wxAcceleratorEntry shortcuts[NUM_SHORTCUTS];

		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* searchMenu;
		wxMenu* viewMenu;
		wxMenu* toolsMenu;
		wxMenu* helpMenu;
		
		void OnDeckSelected(int deckId);

		void SetCurrentPanel(wxPanel* newPanel);		// LAYOUT METHODS
		void SetSidebarPanel(wxPanel* newPanel);
		
		void OnNew(wxCommandEvent& event);				// FILE
		void OnOpen(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		
		void OnUndo(wxCommandEvent& event);				// EDIT
		void OnRedo(wxCommandEvent& event);
		void OnCut(wxCommandEvent& event);				
		void OnCopy(wxCommandEvent& event);
		void OnPaste(wxCommandEvent& event);
		void OnSelectAll(wxCommandEvent& event);
		
		void OnFind(wxCommandEvent& event);				// SEARCH
		void OnReplace(wxCommandEvent& event);
		
		void OnCards(wxCommandEvent& event);			// VIEW
		void OnStudy(wxCommandEvent& event);
		void OnCalendar(wxCommandEvent& event);
		void OnAI(wxCommandEvent& event);
		
		void OnSettings(wxCommandEvent& event);			// TOOLS
		
		void OnAbout(wxCommandEvent& event);			// HELP
		
		void OnMouseEvent(wxMouseEvent& event);			// I/O
		void OnKeyEvent(wxKeyEvent& event);
		
		void OnWindowResized(wxSizeEvent& event);		// WINDOW
		void OnWindowClosed(wxCloseEvent& event);
};

#endif
