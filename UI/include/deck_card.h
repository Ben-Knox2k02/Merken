#ifndef DECK_CARD_H
#define DECK_CARD_H

#include <wx/wx.h>
#include <wx/bmpbndl.h>
#include <wx/dcgraph.h>
#include <wx/statbmp.h>
#include <wx/vector.h>
#include "theme.h"

class DeckCard : public wxPanel {
	public:
		DeckCard(
			wxWindow* parent,
			const wxString& title,
			int cardCount
		) : wxPanel(),
			selected(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
			this->SetWindowStyleFlag(this->GetWindowStyleFlag() & ~wxCLIP_CHILDREN);
			this->SetBackgroundColour(Theme::Get().color.window);

			const int iconDip = Theme::Get().size.icon;
			const int rowHeight = this->FromDIP(36);
			const int side = this->FromDIP(iconDip);
			this->icon = new wxStaticBitmap();
			this->icon->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->icon->Create(this, wxID_ANY, this->IconBundle());
			this->icon->SetMinSize(wxSize(side, side));
			this->icon->SetMaxSize(wxSize(side, side));

			this->titleCtrl = new wxStaticText();
			this->titleCtrl->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->titleCtrl->Create(this, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
			this->titleCtrl->SetMinSize(wxSize(0, -1));
			wxFont titleFont = this->titleCtrl->GetFont();
			titleFont.SetPointSize(10);
			titleFont.SetWeight(wxFONTWEIGHT_NORMAL);
			this->titleCtrl->SetFont(titleFont);
			this->countLabel = new wxStaticText();
			this->countLabel->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->countLabel->Create(this, wxID_ANY, wxString::Format("%d", cardCount));
			this->countLabel->SetFont(titleFont);
			this->ApplyTextColours();

			const int gap = this->FromDIP(Theme::Get().space.sm);
			const int pad = this->FromDIP(Theme::Get().space.lg);

			wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
			row->Add(this->icon, 0, wxALIGN_CENTER_VERTICAL);
			row->Add(this->titleCtrl, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, gap);
			row->Add(this->countLabel, 0, wxALIGN_CENTER_VERTICAL);

			wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
			mainSizer->AddStretchSpacer(1);
			mainSizer->Add(row, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);
			mainSizer->AddStretchSpacer(1);
			this->SetSizer(mainSizer);
			this->SetMinSize(wxSize(-1, rowHeight));
			this->SetMaxSize(wxSize(-1, rowHeight));

			this->Bind(wxEVT_PAINT, &DeckCard::OnPaint, this);
			this->Bind(wxEVT_SIZE, &DeckCard::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->ApplyTheme();
		}

		void ApplyTheme() {
			this->SetBackgroundColour(
				this->selected ? SelectedFill() : Theme::Get().color.window
			);
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
		wxStaticText* countLabel;
		bool selected;

		static wxColour SelectedFill() {
			return wxColour(70, 130, 240);
		}

		wxColour TitleColour() const {
			return this->selected ? wxColour(255, 255, 255) : Theme::Get().color.label;
		}

		wxColour CountColour() const {
			return this->selected ? wxColour(255, 255, 255) : Theme::Get().color.secondaryLabel;
		}

		wxColour FolderColour() const {
			return this->selected ? wxColour(255, 255, 255) : Theme::Get().color.secondaryLabel;
		}

		static wxBitmap DrawFolder(int pixels, const wxColour& colour) {
			wxBitmap bitmap(pixels, pixels);
			bitmap.UseAlpha();
			wxMemoryDC dc(bitmap);
			dc.SetBackground(wxBrush(wxColour(0, 0, 0, 0)));
			dc.Clear();
			wxGCDC gc(dc);
			wxGraphicsContext* g = gc.GetGraphicsContext();
			if (g == nullptr) {
				return bitmap;
			}
			g->SetPen(*wxTRANSPARENT_PEN);
			g->SetBrush(wxBrush(colour));
			const double width = pixels;
			const double height = pixels;
			const double radius = height * 0.14;
			const double tabWidth = width * 0.46;
			const double tabTop = height * 0.14;
			const double bodyTop = height * 0.32;
			g->DrawRoundedRectangle(width * 0.08, tabTop, tabWidth, bodyTop - tabTop + radius, radius);
			g->DrawRoundedRectangle(width * 0.06, bodyTop, width * 0.88, height * 0.56, radius);
			dc.SelectObject(wxNullBitmap);
			return bitmap;
		}

		wxBitmapBundle IconBundle() const {
			const int dip = Theme::Get().size.icon;
			const wxColour colour = this->FolderColour();
			wxVector<wxBitmap> bitmaps;
			bitmaps.push_back(DrawFolder(dip, colour));
			bitmaps.push_back(DrawFolder(dip * 2, colour));
			bitmaps.push_back(DrawFolder(dip * 3, colour));
			return wxBitmapBundle::FromBitmaps(bitmaps);
		}

		void OnSize(wxSizeEvent& event) {
			this->Refresh();
			event.Skip();
		}

		void ApplyTextColours() {
			this->titleCtrl->SetForegroundColour(this->TitleColour());
			this->countLabel->SetForegroundColour(this->CountColour());
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
			const int inset = this->FromDIP(2);
			theme.DrawRounded(
				gc,
				wxRect(inset, inset, size.GetWidth() - inset * 2, size.GetHeight() - inset * 2),
				this->FromDIP(theme.radius.md),
				SelectedFill(),
				wxColour(0, 0, 0, 0)
			);
		}
};

#endif
