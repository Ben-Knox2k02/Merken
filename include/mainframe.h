#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "app.h"
#include "data_structures.h"
#include <wx/wx.h>

#define NUM_SHORTCUTS 16

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);
		App& app;										// GLOBAL REFERENCE TO APP
		
		wxPanel* deckPanel;								// LAYOUT PANELS
		wxPanel* activePanel;
		wxPanel* currentPanel;
		
		wxBoxSizer* rootSizer;
		
		wxAcceleratorEntry shortcuts[NUM_SHORTCUTS];	// KEYBOARD SHORTCUTS
		
		wxMenuBar* menuBar;								// MENU
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* searchMenu;
		wxMenu* viewMenu;
		wxMenu* toolsMenu;
		wxMenu* helpMenu;
		
		void SetCurrentPanel(wxPanel* newPanel);
		
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
