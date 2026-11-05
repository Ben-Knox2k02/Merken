#ifndef ICON_BUTTON_H
#define ICON_BUTTON_H

#include <wx/wx.h>
#include <wx/bmpbuttn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/image.h>
#include <wx/bmpbndl.h>
#include <wx/vector.h>
#include "theme.h"

class IconButton : public wxBitmapButton {
	public:
		IconButton(
			wxWindow* parent,
			const wxString& imagePath,
			const wxString& tooltip = wxEmptyString,
			bool invert = false
		) : wxBitmapButton(
				parent,
				wxID_ANY,
				LoadIconBundle(imagePath, kIconSize, invert || IsDarkTheme()),
				wxDefaultPosition,
				wxDefaultSize,
				wxBU_AUTODRAW
			),
			imagePath(imagePath),
			forceInvert(invert) {
			if (!tooltip.IsEmpty()) {
				this->SetToolTip(tooltip);
			}
			const int height = wxButton::GetDefaultSize(parent).GetHeight();
			const int padX = this->FromDIP(24);
			const int width = this->FromDIP(kIconSize) + padX * 2;
			this->SetMinSize(wxSize(width, height));
			this->CenterIcon();
			this->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
				event.Skip();
				this->CenterIcon();
			});
		}

		void ApplyTheme() {
			if (this->imagePath.IsEmpty()) {
				this->SetBitmapLabel(BackBundle());
				this->CenterIcon();
				return;
			}
			this->SetBitmapLabel(LoadIconBundle(this->imagePath, kIconSize, this->forceInvert || IsDarkTheme()));
			this->CenterIcon();
		}

		static IconButton* Back(wxWindow* parent) {
			return new IconButton(parent, BackBundle(), "Back");
		}

		static wxBitmapBundle LoadIconBundle(const wxString& path, int dipSize, bool invert) {
			wxImage image;
			if (!image.LoadFile(ResolveAssetPath(path), wxBITMAP_TYPE_PNG)) {
				return wxBitmapBundle::FromBitmap(wxBitmap(dipSize, dipSize));
			}
			wxVector<wxBitmap> bitmaps;
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize, invert)));
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize * 2, invert)));
			bitmaps.push_back(wxBitmap(ScaledIcon(image, dipSize * 3, invert)));
			return wxBitmapBundle::FromBitmaps(bitmaps);
		}

	private:
		wxString imagePath;
		bool forceInvert;
		static constexpr int kIconSize = 16;

		IconButton(wxWindow* parent, const wxBitmapBundle& bundle, const wxString& tooltip)
			: wxBitmapButton(
				parent,
				wxID_ANY,
				bundle,
				wxDefaultPosition,
				wxDefaultSize,
				wxBU_AUTODRAW
			),
			imagePath(),
			forceInvert(false) {
			if (!tooltip.IsEmpty()) {
				this->SetToolTip(tooltip);
			}
			const int side = wxButton::GetDefaultSize(parent).GetHeight();
			this->SetMinSize(wxSize(side, side));
			this->CenterIcon();
			this->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
				event.Skip();
				this->CenterIcon();
			});
		}

		static wxBitmapBundle BackBundle() {
			const wxColour colour = Theme::Get().color.label;
			wxVector<wxBitmap> bitmaps;
			bitmaps.push_back(DrawChevron(kIconSize, colour));
			bitmaps.push_back(DrawChevron(kIconSize * 2, colour));
			bitmaps.push_back(DrawChevron(kIconSize * 3, colour));
			return wxBitmapBundle::FromBitmaps(bitmaps);
		}

		static wxBitmap DrawChevron(int pixels, const wxColour& colour) {
			wxBitmap bitmap(pixels, pixels);
			bitmap.UseAlpha();
			wxMemoryDC dc(bitmap);
			dc.SetBackground(wxBrush(wxColour(0, 0, 0, 0)));
			dc.Clear();
			wxGCDC gc(dc);
			wxGraphicsContext* g = gc.GetGraphicsContext();
			if (g == nullptr) {
				dc.SelectObject(wxNullBitmap);
				return bitmap;
			}
			int stroke = (pixels * 12) / 100;
			if (stroke < 2) {
				stroke = 2;
			}
			wxPen pen(colour, stroke);
			pen.SetCap(wxCAP_ROUND);
			pen.SetJoin(wxJOIN_ROUND);
			g->SetPen(pen);
			wxGraphicsPath path = g->CreatePath();
			const double left = pixels * 0.32;
			const double right = pixels * 0.70;
			const double top = pixels * 0.22;
			const double bottom = pixels * 0.78;
			const double mid = pixels * 0.50;
			path.MoveToPoint(right, top);
			path.AddLineToPoint(left, mid);
			path.AddLineToPoint(right, bottom);
			g->StrokePath(path);
			dc.SelectObject(wxNullBitmap);
			return bitmap;
		}

		static wxString ResolveAssetPath(const wxString& relative) {
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
			return Theme::IsDarkAppearance();
		}

		void CenterIcon() {
			// macOS draws NSImageOnly in the middle of the button.
			// wxBitmapButton on Windows already centers its bitmap.
			(void)this;
		}

		static wxImage ScaledIcon(const wxImage& source, int pixels, bool invert) {
			wxImage scaled = source.Copy();
			scaled.Rescale(pixels, pixels, wxIMAGE_QUALITY_HIGH);
			if (invert && scaled.HasAlpha()) {
				const int width = scaled.GetWidth();
				const int height = scaled.GetHeight();
				for (int y = 0; y < height; ++y) {
					for (int x = 0; x < width; ++x) {
						scaled.SetRGB(
							x,
							y,
							255 - scaled.GetRed(x, y),
							255 - scaled.GetGreen(x, y),
							255 - scaled.GetBlue(x, y)
						);
					}
				}
			}
			return scaled;
		}
};

#endif
