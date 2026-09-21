#ifndef AI_PANEL_H
#define AI_PANEL_H

#include "data_structures.h"
#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

class AIPanel : public wxPanel {
	public:
		AIPanel(wxWindow* parent);
	
		wxPanel* topPanel;
		wxPanel* midPanel;
		wxPanel* bottomPanel;
	
		wxBoxSizer* rootSizer;
		wxBoxSizer* topSizer;
		wxBoxSizer* midSizer;
		wxBoxSizer* bottomSizer;
		
		wxStaticText* header;
		
		void OnPaint(wxPaintEvent& event);
};

#endif
