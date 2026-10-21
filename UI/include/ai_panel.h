#ifndef AI_PANEL_H
#define AI_PANEL_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

class AIPanel : public wxPanel {
	public:
		AIPanel(wxWindow* parent);
	
		wxPanel* topPanel;
		wxPanel* bottomPanel;
	
		wxBoxSizer* rootSizer;
		wxBoxSizer* topSizer;
		wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		
		wxTextCtrl* convoCtrl;
		wxTextCtrl* promptCtrl;
		
		wxButton* submitButton;
		
		void OnPaint(wxPaintEvent& event);
		
		void OnSubmit(wxCommandEvent& event);
};

#endif
