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
#ifdef __WXOSX__
#include <objc/message.h>
#include <objc/runtime.h>
#endif

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
			this->SetBitmapLabel(LoadIconBundle(this->imagePath, kIconSize, this->forceInvert || IsDarkTheme()));
			this->CenterIcon();
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
#ifdef __WXOSX__
			id button = reinterpret_cast<id>(this->GetHandle());
			if (button == nullptr) {
				return;
			}
			using VoidLong = void (*)(id, SEL, long);
			// NSImageOnly draws the icon in the middle of the button.
			reinterpret_cast<VoidLong>(objc_msgSend)(button, sel_registerName("setImagePosition:"), 1L);
#else
			(void)this;
#endif
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
