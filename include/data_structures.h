#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <wx/wx.h>

enum {									// CUSTOM IDs
	ID_CARDS = wxID_HIGHEST + 1,
	ID_STUDY,
	ID_CALENDAR,
	ID_AI,
	ID_SETTINGS,
	ID_SHOW_ANSWER,
	ID_PREVIOUS_CARD,
	ID_NEXT_CARD,
	ID_F1, ID_F2, ID_F3, ID_F4, ID_F5, ID_F6, ID_F7, ID_F8, ID_F9, ID_F10, ID_F11, ID_F12
};

struct Card {
	int ID;
	wxString front;
	wxString back;
	std::vector<wxString> tags;
};

struct Deck {
	int ID;
	wxString name;
	wxString description;
};

#endif
