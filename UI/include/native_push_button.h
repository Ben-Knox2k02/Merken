#ifndef NATIVE_PUSH_BUTTON_H
#define NATIVE_PUSH_BUTTON_H

#include <wx/wx.h>

// Preferred size of a platform push button for this label.
// A non-positive width or height means the caller should choose that side.
wxSize PlatformPushButtonSize(wxWindow* window, const wxString& label);

// Draws the platform push-button face and its label.
// Returns false when the caller should draw wxRendererNative's push button.
bool DrawPlatformPushButton(
	wxWindow* window,
	wxDC& dc,
	const wxRect& rect,
	const wxString& label,
	bool enabled,
	bool pressed,
	bool focused = false
);

#endif
