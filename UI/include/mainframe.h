#include <wx/wx.h>

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);
		
		wxBoxSizer* rootSizer;
		
		wxPanel* deckPanel;
		wxPanel* currentPanel; 						// ATTACHES ON TOP OF activePanel
		wxPanel* activePanel;						// A HUSK TO HOLD currentPanel
		
		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* studyMenu;
		wxMenu* calendarMenu;
		wxMenu* AIMenu;
		wxMenu* helpMenu;
		
		void SwapCurrentPanel(wxPanel* newPanel);
		
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
