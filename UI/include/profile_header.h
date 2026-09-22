#ifndef PROFILE_HEADER_H
#define PROFILE_HEADER_H

#include <wx/wx.h>
#include "profile_badge.h"

class ProfileHeader : public wxPanel {
	public:
		explicit ProfileHeader(wxWindow* parent);

		void Load();
		void ApplyTheme();

	private:
		ProfileBadge* badge;
		wxStaticText* nameLabel;

		void BindClick(wxWindow* window);
		void Open();
};

#endif
