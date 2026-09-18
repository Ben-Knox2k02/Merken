#ifndef FLASH_CARD_TAG_H
#define FLASH_CARD_TAG_H

#include <wx/wx.h>
#include <wx/tokenzr.h>

class FlashCardTag : public wxPanel {
	public:
		enum class Style {
			Default,
			Secondary,
			Destructive,
			Outline
		};

		FlashCardTag(wxWindow* parent, const wxString& label, Style style = Style::Default)
			: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			  label(label),
			  style(style) {
			this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->FitToLabel();
			this->Bind(wxEVT_PAINT, &FlashCardTag::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void FitToLabel() {
			int textWidth = 0;
			int textHeight = 0;
			this->GetTextExtent(this->label, &textWidth, &textHeight);
			const int pad = this->FromDIP(10);
			this->fitted = wxSize(textWidth + pad * 2, this->FromDIP(22));
			if (this->fitted.GetHeight() < textHeight + pad) {
				this->fitted.SetHeight(textHeight + pad);
			}
			this->SetMinSize(this->fitted);
			this->SetMaxSize(this->fitted);
			this->SetSize(this->fitted);
		}

		static wxArrayString Split(const wxString& tags) {
			wxArrayString result;
			wxStringTokenizer tokenizer(tags, ",");
			while (tokenizer.HasMoreTokens()) {
				wxString token = tokenizer.GetNextToken();
				token.Trim(true).Trim(false);
				if (!token.empty()) {
					result.Add(token);
				}
			}
			return result;
		}

		static wxSize Measure(wxDC& dc, const wxString& label, wxWindow* scaleWindow) {
			const int padX = scaleWindow != nullptr ? scaleWindow->FromDIP(10) : 10;
			const int height = scaleWindow != nullptr ? scaleWindow->FromDIP(22) : 22;
			const wxSize text = dc.GetTextExtent(label);
			return wxSize(text.GetWidth() + padX * 2, height);
		}

		static void Draw(wxDC& dc, const wxRect& rect, const wxString& label, Style style) {
			const double radius = rect.GetHeight() / 2.0;
			if (style == Style::Outline) {
				dc.SetBrush(*wxTRANSPARENT_BRUSH);
				dc.SetPen(wxPen(BorderColour(style)));
			} else {
				dc.SetBrush(wxBrush(FillColour(style)));
				dc.SetPen(*wxTRANSPARENT_PEN);
			}
			dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, radius);

			dc.SetTextForeground(TextColour(style));
			const wxSize extent = dc.GetTextExtent(label);
			dc.DrawText(
				label,
				rect.x + (rect.width - extent.GetWidth()) / 2,
				rect.y + (rect.height - extent.GetHeight()) / 2
			);
		}

	private:
		wxString label;
		Style style;
		wxSize fitted;

		void OnPaint(wxPaintEvent&) {
			wxPaintDC dc(this);
			dc.SetFont(this->GetFont());
			wxSize size = this->fitted;
			if (size.GetWidth() < 1 || size.GetHeight() < 1) {
				this->FitToLabel();
				size = this->fitted;
			}
			Draw(dc, wxRect(0, 0, size.GetWidth(), size.GetHeight()), this->label, this->style);
		}

		static bool IsDarkTheme() {
			const wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
			return (bg.Red() + bg.Green() + bg.Blue()) < 3 * 128;
		}

		static wxColour FillColour(Style style) {
			const bool dark = IsDarkTheme();
			switch (style) {
				case Style::Secondary:
					return dark ? wxColour(63, 63, 70) : wxColour(39, 39, 42);
				case Style::Destructive:
					return dark ? wxColour(69, 26, 26) : wxColour(254, 226, 226);
				case Style::Outline:
					return wxColour(0, 0, 0, 0);
				case Style::Default:
				default:
					return dark ? wxColour(228, 228, 231) : wxColour(244, 244, 245);
			}
		}

		static wxColour TextColour(Style style) {
			const bool dark = IsDarkTheme();
			switch (style) {
				case Style::Secondary:
					return dark ? wxColour(250, 250, 250) : wxColour(250, 250, 250);
				case Style::Destructive:
					return dark ? wxColour(252, 165, 165) : wxColour(185, 28, 28);
				case Style::Outline:
					return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
				case Style::Default:
				default:
					return dark ? wxColour(24, 24, 27) : wxColour(24, 24, 27);
			}
		}

		static wxColour BorderColour(Style style) {
			return IsDarkTheme() ? wxColour(82, 82, 91) : wxColour(24, 24, 27);
		}
};

#endif
