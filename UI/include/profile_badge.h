#ifndef PROFILE_BADGE_H
#define PROFILE_BADGE_H

#include <wx/wx.h>
#include <wx/dcgraph.h>
#include <algorithm>
#include "theme.h"

class ProfileBadge : public wxPanel {
	public:
		explicit ProfileBadge(wxWindow* parent)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			const int side = this->FromDIP(36);
			this->SetMinSize(wxSize(side, side));
			this->SetMaxSize(wxSize(side, side));
			this->backdrop = Theme::Get().color.primary;
			this->circle = Theme::Get().color.fill;
			this->ink = Theme::Get().color.label;
			this->Bind(wxEVT_PAINT, &ProfileBadge::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetColours(const wxColour& backdrop, const wxColour& circle, const wxColour& ink) {
			this->backdrop = backdrop;
			this->circle = circle;
			this->ink = ink;
			this->Refresh();
		}

		void SetInitials(const wxString& initials) {
			this->initials = initials;
			this->Refresh();
		}

	private:
		wxString initials;
		wxColour backdrop;
		wxColour circle;
		wxColour ink;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			wxGraphicsContext* g = gc.GetGraphicsContext();
			const wxSize size = this->GetClientSize();
			if (g == nullptr || size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}
			g->SetPen(*wxTRANSPARENT_PEN);
			g->SetBrush(wxBrush(this->backdrop));
			g->DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			const double side = std::min(size.GetWidth(), size.GetHeight());
			const double x = (size.GetWidth() - side) / 2.0;
			const double y = (size.GetHeight() - side) / 2.0;
			g->SetBrush(wxBrush(this->circle));
			g->DrawEllipse(x, y, side, side);
			wxFont font = this->GetFont();
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			g->SetFont(font, this->ink);
			const wxString label = this->initials.empty() ? wxString("P") : this->initials;
			double textWidth = 0;
			double textHeight = 0;
			g->GetTextExtent(label, &textWidth, &textHeight);
			g->DrawText(label, x + (side - textWidth) / 2.0, y + (side - textHeight) / 2.0);
		}
};

#endif
