#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <algorithm>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include "theme.h"

// Sidebar folder-style deck row: icon, title, count, blue selection.
class DeckCard : public wxPanel {
	public:
		DeckCard(
			wxWindow* parent,
			const wxString& title,
			int cardCount
		) : wxPanel(),
			title(title),
			cardCount(cardCount),
			selected(false),
			hover(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			this->SetMinSize(wxSize(-1, this->FromDIP(36)));
			this->SetCursor(wxCURSOR_HAND);
			this->Bind(wxEVT_PAINT, &DeckCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &DeckCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
			this->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent&) {
				this->hover = true;
				this->Refresh();
			});
			this->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent&) {
				this->hover = false;
				this->Refresh();
			});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->ApplyTheme();
		}

		void ApplyTheme() {
			this->Refresh();
		}

		wxString GetTitle() const { return this->title; }

		void SetTitle(const wxString& value) {
			this->title = value;
			this->Refresh();
		}

		void SetCardCount(int count) {
			this->cardCount = count;
			this->Refresh();
		}

	private:
		wxString title;
		int cardCount;
		bool selected;
		bool hover;

		void OnSize(wxSizeEvent& event) {
			this->Refresh();
			event.Skip();
		}

		void OnPaint(wxPaintEvent&) {
			wxAutoBufferedPaintDC dc(this);
			const wxSize size = this->GetClientSize();
			const Theme& theme = Theme::Get();

			wxColour canvas = theme.color.grouped;
			dc.SetBrush(wxBrush(canvas));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			wxColour bg = canvas;
			if (this->selected) {
				bg = theme.color.selectedFill;
			} else if (this->hover) {
				bg = theme.color.fill;
			}

			const int insetX = this->FromDIP(8);
			const int insetY = this->FromDIP(2);
			dc.SetBrush(wxBrush(bg));
			dc.DrawRoundedRectangle(
				insetX,
				insetY,
				size.GetWidth() - insetX * 2,
				size.GetHeight() - insetY * 2,
				this->FromDIP(theme.radius.row)
			);

			const wxColour iconCol = this->selected ? theme.color.onSelected : theme.color.tertiaryLabel;
			const wxColour textCol = this->selected ? theme.color.onSelected : theme.color.label;
			const wxColour countCol = this->selected
				? wxColour(
					static_cast<unsigned char>((theme.color.onSelected.Red() + theme.color.selectedFill.Red()) / 2),
					static_cast<unsigned char>((theme.color.onSelected.Green() + theme.color.selectedFill.Green()) / 2),
					static_cast<unsigned char>((theme.color.onSelected.Blue() + theme.color.selectedFill.Blue()) / 2)
				)
				: theme.color.tertiaryLabel;

			const int ix = this->FromDIP(20);
			const int iy = (size.GetHeight() - this->FromDIP(14)) / 2;
			dc.SetBrush(wxBrush(iconCol));
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.DrawRoundedRectangle(ix, iy + this->FromDIP(3), this->FromDIP(14), this->FromDIP(11), this->FromDIP(2));
			dc.DrawRoundedRectangle(ix, iy, this->FromDIP(8), this->FromDIP(5), this->FromDIP(1));

			dc.SetTextForeground(textCol);
			wxFont font = this->GetFont();
			font.SetPointSize(std::max(10, font.GetPointSize()));
			dc.SetFont(font);
			const int textY = (size.GetHeight() - dc.GetCharHeight()) / 2;
			dc.DrawText(this->title, ix + this->FromDIP(22), textY);

			const wxString countStr = wxString::Format("%d", this->cardCount);
			dc.SetTextForeground(countCol);
			wxFont countFont = font;
			countFont.SetPointSize(std::max(9, font.GetPointSize() - 1));
			dc.SetFont(countFont);
			const wxSize ts = dc.GetTextExtent(countStr);
			dc.DrawText(countStr, size.GetWidth() - this->FromDIP(20) - ts.x, (size.GetHeight() - ts.y) / 2);
		}
};

#endif
