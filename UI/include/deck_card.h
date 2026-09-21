#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <algorithm>
#include <cmath>
#include <random>
#include <wx/wx.h>
#include <wx/dcgraph.h>

class DeckCardInitials : public wxPanel {
	public:
		DeckCardInitials(wxWindow* parent, const wxString& title)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  selected(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->fill = RandomFill();
			this->labelColour = ContrastingText(this->fill);
			this->SetInitialsFromTitle(title);
			this->FitToInitials();
			this->Bind(wxEVT_PAINT, &DeckCardInitials::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->Refresh();
		}

		void SetTitle(const wxString& title) {
			this->SetInitialsFromTitle(title);
			this->FitToInitials();
			this->Refresh();
		}

	private:
		static constexpr int kCornerRadius = 5;
		static constexpr int kMinSide = 32 * 2 / 3;

		wxString initials;
		bool selected;
		wxColour fill;
		wxColour labelColour;

		static wxString InitialsFromTitle(const wxString& title) {
			wxString trimmed = title;
			trimmed.Trim(true).Trim(false);
			wxString letters;
			for (size_t i = 0; i < trimmed.length() && letters.length() < 3; ++i) {
				const wxUniChar ch = trimmed[i];
				if (ch != ' ' && ch != '\t') {
					letters += ch;
				}
			}
			return letters.Upper();
		}

		void SetInitialsFromTitle(const wxString& title) {
			this->initials = InitialsFromTitle(title);
		}

		static wxColour ColourFromHsv(int hue, double saturation, double value) {
			const double c = value * saturation;
			const double x = c * (1.0 - std::fabs(std::fmod(hue / 60.0, 2.0) - 1.0));
			const double m = value - c;
			double r = 0;
			double g = 0;
			double b = 0;
			if (hue < 60) {
				r = c; g = x;
			} else if (hue < 120) {
				r = x; g = c;
			} else if (hue < 180) {
				g = c; b = x;
			} else if (hue < 240) {
				g = x; b = c;
			} else if (hue < 300) {
				r = x; b = c;
			} else {
				r = c; b = x;
			}
			return wxColour(
				static_cast<unsigned char>((r + m) * 255.0 + 0.5),
				static_cast<unsigned char>((g + m) * 255.0 + 0.5),
				static_cast<unsigned char>((b + m) * 255.0 + 0.5)
			);
		}

		static wxColour RandomFill() {
			static std::mt19937 rng{std::random_device{}()};
			std::uniform_int_distribution<int> hue(0, 359);
			std::uniform_int_distribution<int> sat(50, 75);
			std::uniform_int_distribution<int> val(55, 85);
			return ColourFromHsv(hue(rng), sat(rng) / 100.0, val(rng) / 100.0);
		}

		static wxColour ContrastingText(const wxColour& fill) {
			const double luminance =
				(0.299 * fill.Red() + 0.587 * fill.Green() + 0.114 * fill.Blue()) / 255.0;
			return luminance > 0.55 ? wxColour(24, 24, 24) : wxColour(255, 255, 255);
		}

		static wxColour BorderFor(const wxColour& fill) {
			return wxColour(
				static_cast<unsigned char>(fill.Red() * 3 / 4),
				static_cast<unsigned char>(fill.Green() * 3 / 4),
				static_cast<unsigned char>(fill.Blue() * 3 / 4)
			);
		}

		void FitToInitials() {
			const int pad = this->FromDIP(4);
			const wxSize extent = this->GetTextExtent("WWW");
			const int side = std::max({
				extent.GetWidth() + pad * 2,
				extent.GetHeight() + pad * 2,
				this->FromDIP(kMinSide)
			});
			const wxSize size(side, side);
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

			wxColour background = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
			if (this->GetParent() != nullptr) {
				background = this->GetParent()->GetBackgroundColour();
			}
			gc.SetPen(*wxTRANSPARENT_PEN);
			gc.SetBrush(wxBrush(background));
			gc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			gc.SetPen(wxPen(BorderFor(this->fill), 1));
			gc.SetBrush(wxBrush(this->fill));
			gc.DrawRoundedRectangle(
				0,
				0,
				size.GetWidth() - 1,
				size.GetHeight() - 1,
				this->FromDIP(kCornerRadius)
			);

			gc.SetFont(this->GetFont());
			gc.SetTextForeground(this->labelColour);
			const wxSize extent = gc.GetTextExtent(this->initials);
			gc.DrawText(
				this->initials,
				(size.GetWidth() - extent.GetWidth()) / 2,
				(size.GetHeight() - extent.GetHeight()) / 2
			);
		}
};

class DeckCard : public wxPanel {
	public:
		DeckCard(
			wxWindow* parent,
			const wxString& title
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			selected(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);

			this->initialsBox = new DeckCardInitials(this, title);

			this->titleCtrl = new wxStaticText(
				this,
				wxID_ANY,
				title,
				wxDefaultPosition,
				wxDefaultSize,
				wxST_ELLIPSIZE_END
			);
			this->titleCtrl->SetFont(this->titleCtrl->GetFont().Bold());
			this->titleCtrl->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->titleCtrl->SetMinSize(wxSize(0, -1));
			this->ApplyTextColours();

			wxBoxSizer* titleCol = new wxBoxSizer(wxVERTICAL);
			titleCol->AddStretchSpacer(1);
			titleCol->Add(this->titleCtrl, 0, wxEXPAND);
			titleCol->AddStretchSpacer(1);

			wxBoxSizer* titleRow = new wxBoxSizer(wxHORIZONTAL);
			titleRow->Add(this->initialsBox, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);
			titleRow->Add(titleCol, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

			wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
			mainSizer->Add(titleRow, 0, wxEXPAND | wxTOP | wxBOTTOM, 6);
			this->SetSizer(mainSizer);

			this->Bind(wxEVT_PAINT, &DeckCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &DeckCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->initialsBox->SetSelected(selected);
			this->ApplyTextColours();
			this->Refresh();
		}

		wxString GetTitle() const { return this->titleCtrl->GetLabel(); }

		void SetTitle(const wxString& title) {
			this->titleCtrl->SetLabel(title);
			this->initialsBox->SetTitle(title);
			this->Layout();
		}

	private:
		static constexpr int kCornerRadius = 5;

		DeckCardInitials* initialsBox;
		wxStaticText* titleCtrl;
		bool selected;

		void OnSize(wxSizeEvent& event) {
			this->Refresh();
			event.Skip();
		}

		void ApplyTextColours() {
			const wxColour text = wxSystemSettings::GetColour(
				this->selected ? wxSYS_COLOUR_HIGHLIGHTTEXT : wxSYS_COLOUR_WINDOWTEXT
			);
			this->titleCtrl->SetForegroundColour(text);
		}

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			wxGCDC gc(dc);
			const wxSize size = this->GetClientSize();
			wxColour background = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
			if (this->GetParent() != nullptr) {
				background = this->GetParent()->GetBackgroundColour();
			}

			gc.SetPen(*wxTRANSPARENT_PEN);
			gc.SetBrush(wxBrush(background));
			gc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

			if (!this->selected) {
				return;
			}

			gc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
			gc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), kCornerRadius);
		}
};

#endif
