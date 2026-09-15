#ifndef DECK_CARD_ICON_H
#define DECK_CARD_ICON_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/statbmp.h>
#include <wx/image.h>
#include <wx/bmpbndl.h>
#include <wx/dcgraph.h>
#include <wx/vector.h>

class DeckCardIcon : public wxPanel {
	public:
		DeckCardIcon(
			wxWindow* parent,
			const wxString& title,
			const wxString& description,
			const wxString& imagePath = wxEmptyString
		) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
			selected(false) {
			this->SetBackgroundStyle(wxBG_STYLE_PAINT);
			this->SetMinSize(wxSize(-1, kCardHeight));

			wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

			wxStaticBitmap* cardImage = new wxStaticBitmap(
				this,
				wxID_ANY,
				LoadIconBundle(imagePath.IsEmpty() ? DefaultIconPath() : imagePath, kIconSize)
			);
			cardImage->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			mainSizer->Add(cardImage, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

			wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);

			this->titleCtrl = new wxStaticText(this, wxID_ANY, title);
			this->titleCtrl->SetFont(this->titleCtrl->GetFont().Bold());
			this->titleCtrl->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

			this->descCtrl = new wxStaticText(this, wxID_ANY, description);
			this->descCtrl->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
			this->descCtrl->Wrap(200);
			this->ApplyTextColours();

			textSizer->AddStretchSpacer(1);
			textSizer->Add(this->titleCtrl, 0, wxBOTTOM, 2);
			textSizer->Add(this->descCtrl, 0, wxEXPAND);
			textSizer->AddStretchSpacer(1);

			mainSizer->Add(textSizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 12);
			this->SetSizer(mainSizer);

			this->Bind(wxEVT_PAINT, &DeckCardIcon::OnPaint, this);
			this->Bind(wxEVT_SIZE, &DeckCardIcon::OnSize, this);
			this->Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
		}

		void SetSelected(bool selected) {
			this->selected = selected;
			this->ApplyTextColours();
			this->Refresh();
		}

		wxString GetTitle() const { return this->titleCtrl->GetLabel(); }
		wxString GetDescription() const { return this->descCtrl->GetLabel(); }

		void SetTitle(const wxString& title) {
			this->titleCtrl->SetLabel(title);
			this->Layout();
		}

		void SetDescription(const wxString& description) {
			this->descCtrl->SetLabel(description);
			this->descCtrl->Wrap(200);
			this->Layout();
		}

	private:
		static constexpr int kCardHeight = 64;
		static constexpr int kIconSize = 28;
		static constexpr int kCornerRadius = 5;

		wxStaticText* titleCtrl;
		wxStaticText* descCtrl;
		bool selected;

		void OnSize(wxSizeEvent& event) {
			this->Refresh();
			event.Skip();
		}

		void ApplyTextColours() {
			const wxColour text = wxSystemSettings::GetColour(
				this->selected ? wxSYS_COLOUR_HIGHLIGHTTEXT : wxSYS_COLOUR_WINDOWTEXT
			);
			const wxColour secondary = this->selected
				? text
				: wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
			this->titleCtrl->SetForegroundColour(text);
			this->descCtrl->SetForegroundColour(secondary);
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

		static wxString DefaultIconPath() {
			const wxString relative = "UI/assets/deck_card_icon.png";
			wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
			const wxString candidates[] = {
				wxGetCwd() + wxFileName::GetPathSeparator() + relative,
				exe.GetPath() + wxFileName::GetPathSeparator() + relative,
				exe.GetPath() + "/../../../" + relative
			};
			for (const wxString& candidate : candidates) {
				wxFileName file(candidate);
				file.MakeAbsolute();
				if (file.FileExists()) {
					return file.GetFullPath();
				}
			}
			return relative;
		}

		static bool IsDarkTheme() {
			const wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
			return (bg.Red() + bg.Green() + bg.Blue()) < 3 * 128;
		}

		static wxImage ScaledIcon(const wxImage& source, int pixels) {
			wxImage scaled = source.Copy();
			scaled.Rescale(pixels, pixels, wxIMAGE_QUALITY_HIGH);
			if (IsDarkTheme() && scaled.HasAlpha()) {
				const int width = scaled.GetWidth();
				const int height = scaled.GetHeight();
				for (int y = 0; y < height; ++y) {
					for (int x = 0; x < width; ++x) {
						scaled.SetRGB(x, y, 255 - scaled.GetRed(x, y), 255 - scaled.GetGreen(x, y), 255 - scaled.GetBlue(x, y));
					}
				}
			}
			return scaled;
		}

		static wxBitmapBundle LoadIconBundle(const wxString& path, int dipSize) {
			wxImage image;
			if (!image.LoadFile(path, wxBITMAP_TYPE_PNG)) {
				return wxBitmapBundle::FromBitmap(wxBitmap(dipSize, dipSize));
			}
			wxVector<wxBitmap> bitmaps;
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize)));
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize * 2)));
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize * 3)));
			return wxBitmapBundle::FromBitmaps(bitmaps);
		}
};

#endif
