#ifndef FLASH_CARD_NUMBER_H
#define FLASH_CARD_NUMBER_H

#include <wx/wx.h>
#include "theme.h"

class FlashCardNumber : public wxPanel {
	public:
		FlashCardNumber(wxWindow* parent, int number)
			: wxPanel(),
			  label(wxString::Format("%d", number)) {
			this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			const int side = this->FromDIP(Theme::Get().size.badge);
			const wxSize size(side, side);
			this->SetMinSize(size);
			this->SetMaxSize(size);
			this->SetSize(size);

			wxFont font = this->GetFont();
			for (int point = font.GetPointSize(); point >= 6; --point) {
				font.SetPointSize(point);
				this->SetFont(font);
				const wxSize extent = this->GetTextExtent(this->label);
				if (extent.GetWidth() <= side - 2 && extent.GetHeight() <= side - 2) {
					break;
				}
			}

			this->Bind(wxEVT_PAINT, &FlashCardNumber::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetNumber(int number) {
			this->label = wxString::Format("%d", number);
			this->Refresh();
		}

	private:
		wxString label;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			const wxSize size = this->GetClientSize();
			if (size.GetWidth() < 2 || size.GetHeight() < 2) {
				return;
			}

			const Theme& theme = Theme::Get();
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(wxBrush(theme.color.fill));
			dc.DrawRoundedRectangle(
				0,
				0,
				size.GetWidth() - 1,
				size.GetHeight() - 1,
				this->FromDIP(theme.radius.xs)
			);

			dc.SetFont(this->GetFont());
			dc.SetTextForeground(theme.color.label);
			const wxSize extent = dc.GetTextExtent(this->label);
			dc.DrawText(
				this->label,
				(size.GetWidth() - extent.GetWidth()) / 2,
				(size.GetHeight() - extent.GetHeight()) / 2
			);
		}
};

#endif
