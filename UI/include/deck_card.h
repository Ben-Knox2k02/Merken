#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <wx/wx.h>
#include <wx/dcgraph.h>
#include <wx/statbmp.h>
#include "deck_card_number.h"
#include "icon_button.h"
#include "theme.h"

class DeckCard : public wxPanel {
	public:
		DeckCard(
			wxWindow* parent,
			const wxString& title,
			int cardCount
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			selected(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);

			const int side = this->FromDIP(18);
			this->icon = new wxStaticBitmap(this, wxID_ANY, this->IconBundle());
			this->icon->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->icon->SetMinSize(wxSize(side, side));
			this->icon->SetMaxSize(wxSize(side, side));

			this->titleCtrl = new wxStaticText(
				this,
				wxID_ANY,
				title,
				wxDefaultPosition,
				wxDefaultSize,
				wxST_ELLIPSIZE_END
			);
			this->titleCtrl->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->titleCtrl->SetMinSize(wxSize(0, -1));
			this->countBadge = new DeckCardNumber(this, cardCount);
			this->ApplyTextColours();

			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(this->icon, 0, wxALIGN_CENTER_VERTICAL);
			row->Add(this->titleCtrl, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, Theme::Get().space.sm);
			row->Add(this->countBadge, 0, wxALIGN_CENTER_VERTICAL);

			wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
			mainSizer->Add(row, 0, wxEXPAND | wxALL, Theme::Get().space.lg);
			this->SetSizer(mainSizer);

			this->Bind(wxEVT_PAINT, &DeckCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &DeckCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->icon->SetBitmap(this->IconBundle());
			this->ApplyTextColours();
			this->Refresh();
		}

		wxString GetTitle() const { return this->titleCtrl->GetLabel(); }

		void SetTitle(const wxString& title) {
			this->titleCtrl->SetLabel(title);
			this->Layout();
		}

	private:
		wxStaticBitmap* icon;
		wxStaticText* titleCtrl;
		DeckCardNumber* countBadge;
		bool selected;

		wxColour TitleColour() const {
			return this->selected
				? Theme::Get().color.onSelected
				: Theme::Get().color.onPrimary;
		}

		wxBitmapBundle IconBundle() const {
			const wxColour label = this->TitleColour();
			const bool invert = (label.Red() + label.Green() + label.Blue()) >= 3 * 128;
			return IconButton::LoadIconBundle("UI/assets/deck_card_icon.png", 18, invert);
		}

		void OnSize(wxSizeEvent& event) {
			this->Refresh();
			event.Skip();
		}

		void ApplyTextColours() {
			this->titleCtrl->SetForegroundColour(this->TitleColour());
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			const Theme& theme = Theme::Get();
			theme.FillCanvas(gc, this, size);
			if (!this->selected) {
				return;
			}
			theme.DrawRounded(
				gc,
				wxRect(0, 0, size.GetWidth(), size.GetHeight()),
				this->FromDIP(theme.radius.row),
				theme.color.selectedFill,
				theme.color.selectedFill
			);
		}
};

#endif
