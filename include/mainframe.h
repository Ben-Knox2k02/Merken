#include <wx/wx.h>

class MainFrame : public wxFrame {
	public:
		MainFrame(const wxString& title);
		
		wxPanel* panel;
		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenu* helpMenu;
		
		void InitWidgets();
		void BindEventHandlers();
		
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
