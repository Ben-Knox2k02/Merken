#ifndef PASTEL_DECK_CARD_H
#define PASTEL_DECK_CARD_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <cmath>
#include <functional>
#include "theme.h"

// Pastel gradient deck tile for the deck-home grid (bookmark-manager look).
class PastelDeckCard : public wxPanel {
	public:
		static constexpr int kPreferredWidth = 260;
		static constexpr int kPreferredHeight = 168;

		using OpenCallback = std::function<void(int deckId)>;

		PastelDeckCard(
			wxWindow* parent,
			int deckId,
			const wxString& title,
			const wxString& description,
			const wxString& createdAt,
			int cardCount,
			int colorIndex,
			OpenCallback onOpen
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition,
				wxSize(kPreferredWidth, kPreferredHeight), wxBORDER_NONE),
			deckId(deckId),
			title(title),
			description(description),
			createdAt(createdAt.IsEmpty() ? wxString("Deck") : createdAt),
			cardCount(cardCount),
			colorIndex(colorIndex),
			onOpen(std::move(onOpen)) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->SetMinSize(wxSize(kPreferredWidth, kPreferredHeight));
			this->SetMaxSize(wxSize(kPreferredWidth, kPreferredHeight));
			this->SetCursor(wxCURSOR_HAND);
			this->Bind(wxEVT_PAINT, &PastelDeckCard::OnPaint, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
			this->Bind(wxEVT_SIZE, [this](wxSizeEvent& e) { this->Refresh(); e.Skip(); });
			this->Bind(wxEVT_LEFT_DOWN, &PastelDeckCard::OnClick, this);
			this->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent&) { this->hover = true; this->Refresh(); });
			this->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent&) { this->hover = false; this->Refresh(); });
		}

		int GetDeckId() const { return this->deckId; }

	private:
		int deckId;
		wxString title;
		wxString description;
		wxString createdAt;
		int cardCount;
		int colorIndex;
		OpenCallback onOpen;
		bool hover = false;

		struct PastelPair {
			wxColour top;
			wxColour bot;
		};

		static PastelPair Palette(int index) {
			static const PastelPair kColors[] = {
				{ Theme::Hex(0xE6DCFF), Theme::Hex(0xD2C8F5) },
				{ Theme::Hex(0xD2E6FF), Theme::Hex(0xBED7FA) },
				{ Theme::Hex(0xFFDCEB), Theme::Hex(0xFAC8DC) },
				{ Theme::Hex(0xFFE6D2), Theme::Hex(0xFFD7BE) },
				{ Theme::Hex(0xD2F0FF), Theme::Hex(0xBEE6FA) },
				{ Theme::Hex(0xDCF5E1), Theme::Hex(0xC8EBD2) },
				{ Theme::Hex(0xEBE1FF), Theme::Hex(0xDCD2FA) },
				{ Theme::Hex(0xFFE1F0), Theme::Hex(0xFAD2E6) },
				{ Theme::Hex(0xE1F5F0), Theme::Hex(0xCDEBE6) },
			};
			const int n = static_cast<int>(sizeof(kColors) / sizeof(kColors[0]));
			return kColors[((index % n) + n) % n];
		}

		void OnClick(wxMouseEvent&) {
			if (this->onOpen) {
				this->onOpen(this->deckId);
			}
		}

		static wxString Ellipsize(wxGraphicsContext* gc, wxString text, double maxW) {
			wxDouble tw = 0, th = 0, td = 0, te = 0;
			gc->GetTextExtent(text, &tw, &th, &td, &te);
			if (tw <= maxW) {
				return text;
			}
			while (text.length() > 3) {
				text.RemoveLast();
				gc->GetTextExtent(text + "...", &tw, &th, &td, &te);
				if (tw <= maxW) {
					break;
				}
			}
			return text + "...";
		}

		void DrawFavicon(wxGraphicsContext* gc, double x, double y, double size) const {
			wxGraphicsPath circle = gc->CreatePath();
			circle.AddEllipse(x, y, size, size);
			gc->SetBrush(wxBrush(wxColour(255, 255, 255, 220)));
			gc->FillPath(circle);
			gc->SetPen(wxPen(wxColour(200, 200, 220, 160), 1));
			gc->StrokePath(circle);

			wxString initial = this->title.Left(1).Upper();
			if (initial.empty()) {
				initial = "D";
			}
			gc->SetFont(wxFontInfo(11).Bold().Family(wxFONTFAMILY_DEFAULT), wxColour(70, 70, 100));
			wxDouble tw = 0, th = 0, td = 0, te = 0;
			gc->GetTextExtent(initial, &tw, &th, &td, &te);
			gc->DrawText(initial, x + (size - tw) / 2.0, y + (size - th) / 2.0);
		}

		void DrawBookmarkGlyph(wxGraphicsContext* gc, double x, double y, double size) const {
			wxGraphicsPath path = gc->CreatePath();
			path.MoveToPoint(x, y);
			path.AddLineToPoint(x + size, y);
			path.AddLineToPoint(x + size, y + size * 1.15);
			path.AddLineToPoint(x + size / 2.0, y + size * 0.85);
			path.AddLineToPoint(x, y + size * 1.15);
			path.CloseSubpath();
			gc->SetBrush(wxBrush(wxColour(120, 100, 180)));
			gc->SetPen(*wxTRANSPARENT_PEN);
			gc->FillPath(path);
		}

		void DrawExternalLinkGlyph(wxGraphicsContext* gc, double x, double y, double size) const {
			gc->SetPen(wxPen(wxColour(120, 120, 140), 1.5));
			wxGraphicsPath box = gc->CreatePath();
			box.AddRectangle(x, y + size * 0.25, size * 0.7, size * 0.7);
			gc->StrokePath(box);
			wxGraphicsPath arrow = gc->CreatePath();
			arrow.MoveToPoint(x + size * 0.4, y + size * 0.15);
			arrow.AddLineToPoint(x + size, y);
			arrow.AddLineToPoint(x + size, y + size * 0.55);
			gc->StrokePath(arrow);
		}

		void OnPaint(wxPaintEvent&) {
			wxAutoBufferedPaintDC dc(this);
			dc.Clear();
			wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
			if (gc == nullptr) {
				return;
			}

			const wxSize sz = this->GetClientSize();
			const double radius = 16.0;
			const double pad = 14.0;
			const PastelPair pastel = Palette(this->colorIndex);

			{
				wxGraphicsPath shadow = gc->CreatePath();
				shadow.AddRoundedRectangle(3, 4, sz.x - 6, sz.y - 6, radius);
				gc->SetBrush(wxBrush(wxColour(0, 0, 0, this->hover ? 40 : 28)));
				gc->FillPath(shadow);
			}

			wxGraphicsPath path = gc->CreatePath();
			path.AddRoundedRectangle(0, 0, sz.x - 2, sz.y - 2, radius);
			wxGraphicsBrush brush = gc->CreateLinearGradientBrush(
				0, 0, 0, sz.y - 2, pastel.top, pastel.bot
			);
			gc->SetBrush(brush);
			gc->FillPath(path);
			gc->SetPen(wxPen(wxColour(255, 255, 255, 180), 1));
			gc->StrokePath(path);

			this->DrawFavicon(gc, pad, pad, 28.0);

			gc->SetFont(wxFontInfo(10).Family(wxFONTFAMILY_DEFAULT), wxColour(90, 90, 110));
			wxDouble tw = 0, th = 0, td = 0, te = 0;
			gc->GetTextExtent(this->createdAt, &tw, &th, &td, &te);
			gc->DrawText(this->createdAt, sz.x - pad - tw - 4, pad + 6);

			this->DrawBookmarkGlyph(gc, sz.x - pad - 18, pad + 2, 12.0);

			double y = pad + 36;
			gc->SetFont(wxFontInfo(10).Family(wxFONTFAMILY_DEFAULT), wxColour(110, 110, 130));
			const wxString subtitle = wxString::Format("%d cards", this->cardCount);
			gc->DrawText(subtitle, pad, y);

			y += 20;
			gc->SetFont(wxFontInfo(13).Bold().Family(wxFONTFAMILY_DEFAULT), wxColour(30, 30, 45));
			gc->DrawText(Ellipsize(gc, this->title, sz.x - pad * 2), pad, y);
			gc->GetTextExtent("Ag", &tw, &th, &td, &te);
			y += th + 6;

			gc->SetFont(wxFontInfo(10).Family(wxFONTFAMILY_DEFAULT), wxColour(80, 80, 100));
			wxString desc = this->description.IsEmpty()
				? wxString("Flashcard deck — open to study or edit cards.")
				: this->description;
			wxDouble maxW = sz.x - pad * 2;
			wxDouble lineH = 0;
			gc->GetTextExtent("Ag", &tw, &lineH, &td, &te);
			for (int line = 0; line < 2 && !desc.empty(); ++line) {
				wxString chunk;
				for (size_t i = 0; i < desc.length(); ++i) {
					wxString trial = chunk + desc[i];
					gc->GetTextExtent(trial, &tw, &th, &td, &te);
					if (tw > maxW) {
						if (line == 1) {
							chunk += "...";
						}
						break;
					}
					chunk = trial;
				}
				gc->DrawText(chunk, pad, y);
				y += lineH + 2;
				if (chunk.EndsWith("...")) {
					break;
				}
				desc = desc.Mid(chunk.length());
				desc.Trim(false);
			}

			const double bottomY = sz.y - pad - 18;
			gc->SetFont(wxFontInfo(10).Family(wxFONTFAMILY_DEFAULT), wxColour(100, 100, 120));
			gc->DrawText(wxString::Format("%d", this->cardCount), pad, bottomY);
			this->DrawExternalLinkGlyph(gc, sz.x - pad - 18, bottomY, 14.0);

			delete gc;
		}
};

#endif
