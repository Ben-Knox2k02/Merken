#include "native_push_button.h"

// Windows and Linux draw the themed control through wxRendererNative.
// macOS is implemented in native_push_button_mac.mm. The shared renderer
// there uses a square bezel, which is not the push button wxButton uses.
#ifndef __WXOSX__

wxSize PlatformPushButtonSize(wxWindow*, const wxString&) {
	return wxSize(0, 0);
}

bool DrawPlatformPushButton(wxWindow*, wxDC&, const wxRect&, const wxString&, bool, bool, bool) {
	return false;
}

#endif
