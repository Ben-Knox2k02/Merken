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
	ID_NEXT_CARD
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
