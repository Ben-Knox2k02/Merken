#ifndef CHEVRON_BUTTON_H
#define CHEVRON_BUTTON_H

#include <wx/wx.h>

// Painted push button with one glyph, the same face as the calendar controls.
class ChevronButton : public wxPanel {
	public:
		ChevronButton(wxWindow* parent, const wxString& glyph, const wxSize& size);

	private:
		void OnPaint(wxPaintEvent&);
		void OnLeftDown(wxMouseEvent&);
		void OnLeftUp(wxMouseEvent& event);
		void OnEnter(wxMouseEvent&);
		void OnLeave(wxMouseEvent&);

		wxString glyph;
		wxFont font;
		bool pressed;
		bool inside;
};

// Size that fits the glyph on the platform push-button face.
wxSize ChevronButtonSize(wxWindow* window, const wxString& glyph);

#endif
