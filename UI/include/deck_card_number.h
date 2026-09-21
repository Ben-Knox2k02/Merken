#ifndef DECK_CARD_NUMBER_H
#define DECK_CARD_NUMBER_H

#include <algorithm>
#include <wx/wx.h>
#include <wx/dcgraph.h>
#include "theme.h"

class DeckCardNumber : public wxPanel {
	public:
		DeckCardNumber(wxWindow* parent, int number)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  label(wxString::Format("%d", number)) {
			this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->FitToLabel();
			this->Bind(wxEVT_PAINT, &DeckCardNumber::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetNumber(int number) {
			this->label = wxString::Format("%d", number);
			this->FitToLabel();
			this->Refresh();
		}

	private:
		wxString label;

		void FitToLabel() {
			const int height = this->FromDIP(Theme::Get().size.chipHeight);
			const int pad = this->FromDIP(Theme::Get().size.chipPad);
			const wxSize extent = this->GetTextExtent(this->label);
			const int width = std::max(height, extent.GetWidth() + pad * 2);
			const wxSize size(width, height);
			this->SetMinSize(size);
			this->SetMaxSize(size);
			this->SetSize(size);
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}

			const Theme& theme = Theme::Get();
			theme.DrawRounded(
				gc,
				wxRect(0, 0, size.GetWidth() - 1, size.GetHeight() - 1),
				this->FromDIP(theme.radius.chip),
				theme.color.fill,
				wxColour(0, 0, 0, 0)
			);

			gc.SetFont(this->GetFont());
			gc.SetTextForeground(theme.color.label);
			const wxSize extent = gc.GetTextExtent(this->label);
			gc.DrawText(
				this->label,
				(size.GetWidth() - extent.GetWidth()) / 2,
				(size.GetHeight() - extent.GetHeight()) / 2
			);
		}
};

#endif
