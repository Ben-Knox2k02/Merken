#ifndef THEME_H
#define THEME_H

#include <wx/wx.h>
#include <wx/dcgraph.h>

// Visual tokens for Merken. Brand primary is #0B2937; surfaces, type, and
// elevation follow iOS grouped surfaces and macOS hairline cards.
class Theme {
	public:
		struct Color {
			wxColour primary;
			wxColour primaryHover;
			wxColour primarySoft;
			wxColour onPrimary;

			wxColour accent;
			wxColour onAccent;

			wxColour window;
			wxColour grouped;
			wxColour card;
			wxColour well;
			wxColour fill;

			wxColour label;
			wxColour secondaryLabel;
			wxColour tertiaryLabel;
			wxColour onSelected;

			wxColour separator;
			wxColour border;
			wxColour borderStrong;

			wxColour selected;
			wxColour selectedFill;

			wxColour destructive;
			wxColour onDestructive;
			wxColour success;

			wxColour tagFill;
			wxColour tagLabel;
			wxColour tagBorder;

			wxColour studyFront;
			wxColour studyBack;

			wxColour shadow;
		};

		struct Radius {
			int xs = 6;
			int sm = 8;
			int md = 16;
			int lg = 14;
			int well = 8;
			int chip = 11;
			int row = 8;
		};

		struct Space {
			int xxs = 2;
			int xs = 4;
			int sm = 6;
			int md = 8;
			int lg = 10;
			int xl = 12;
			int xxl = 16;
			int xxxl = 24;
		};

		struct Elevation {
			int offsetY = 0;
			int blur = 0;
			unsigned char alpha = 0;
		};

		struct Elevations {
			Elevation flat{0, 0, 0};
			Elevation card{1, 3, 28};
			Elevation raised{3, 8, 36};
			Elevation overlay{8, 20, 50};
		};

		struct Stroke {
			int hairline = 1;
			int regular = 1;
		};

		struct Size {
			int icon = 16;
			int badge = 32;
			int chipHeight = 22;
			int chipPad = 10;
			int cardPad = 16;
			int wellPadX = 15;
			int wellPadY = 10;
			int panelPad = 10;
			int listGap = 12;
			int wrapDeadband = 8;
			int minWrap = 40;
		};

		Color color;
		Radius radius;
		Space space;
		Elevations elevation;
		Stroke stroke;
		Size size;
		bool dark = false;

		static const Theme& Get();
		static bool IsDarkAppearance();
		static int Dip(const wxWindow* window, int value);
		static wxColour Hex(unsigned int rgb, unsigned char alpha = 255);

		wxColour CanvasBehind(const wxWindow* window) const;
		wxColour LabelOn(bool selected) const;
		wxPen HairlinePen() const;
		wxPen BorderPen() const;

		void FillCanvas(wxGCDC& gc, const wxWindow* window, const wxSize& size) const;
		void DrawShadow(wxGCDC& gc, const wxRect& rect, int radiusDip, const Elevation& elev) const;
		void DrawRounded(wxGCDC& gc, const wxRect& rect, int radiusDip, const wxColour& fill, const wxColour& border) const;
		void DrawCard(wxGCDC& gc, wxWindow* window, const wxRect& rect, bool selected = false) const;
		void DrawPanel(wxGCDC& gc, wxWindow* window, const wxSize& client) const;
		void DrawWell(wxGCDC& gc, wxWindow* window, const wxRect& rect) const;
		void DrawBadge(wxGCDC& gc, wxWindow* window, const wxRect& rect) const;
		void StyleDialog(wxWindow* window) const;

	private:
		Theme();
};

inline wxColour Theme::Hex(unsigned int rgb, unsigned char alpha) {
	return wxColour(
		static_cast<unsigned char>((rgb >> 16) & 0xFF),
		static_cast<unsigned char>((rgb >> 8) & 0xFF),
		static_cast<unsigned char>(rgb & 0xFF),
		alpha
	);
}

inline bool Theme::IsDarkAppearance() {
	return wxSystemSettings::GetAppearance().IsDark();
}

inline int Theme::Dip(const wxWindow* window, int value) {
	return window != nullptr ? window->FromDIP(value) : value;
}

inline Theme::Theme() {
	this->dark = IsDarkAppearance();

	this->color.primary = Hex(0x0B2937);
	this->color.primaryHover = Hex(0x0E3547);
	this->color.onPrimary = Hex(0xFFFFFF);
	this->color.accent = Hex(0x1F86B0);
	this->color.onAccent = Hex(0xFFFFFF);
	this->color.destructive = this->dark ? Hex(0xFF453A) : Hex(0xFF3B30);
	this->color.onDestructive = Hex(0xFFFFFF);
	this->color.success = this->dark ? Hex(0x30D158) : Hex(0x34C759);
	this->color.shadow = Hex(0x000000, this->dark ? 70 : 28);

	if (this->dark) {
		this->color.primarySoft = Hex(0x163442);
		this->color.window = Hex(0x071C26);
		this->color.grouped = Hex(0x071C26);
		this->color.card = Hex(0x102A36);
		this->color.well = Hex(0x0C222C);
		this->color.fill = Hex(0x1A3A48);
		this->color.label = Hex(0xF3F7F9);
		this->color.secondaryLabel = Hex(0xA9BCC4);
		this->color.tertiaryLabel = Hex(0x7E949C);
		this->color.onSelected = Hex(0xFFFFFF);
		this->color.separator = Hex(0x274554);
		this->color.border = Hex(0x2E5160);
		this->color.borderStrong = Hex(0x3A6576);
		this->color.selected = this->color.accent;
		this->color.selectedFill = this->color.accent;
		this->color.tagFill = Hex(0x1A3A48);
		this->color.tagLabel = Hex(0xE4F3F8);
		this->color.tagBorder = Hex(0x2E5160);
	} else {
		this->color.primarySoft = Hex(0xE4EEF2);
		this->color.window = Hex(0xF2F4F6);
		this->color.grouped = Hex(0xF2F4F6);
		this->color.card = Hex(0xFFFFFF);
		this->color.well = Hex(0xF7F9FA);
		this->color.fill = Hex(0xE8EEF1);
		this->color.label = Hex(0x0B2937);
		this->color.secondaryLabel = Hex(0x5E717A);
		this->color.tertiaryLabel = Hex(0x8A9AA1);
		this->color.onSelected = Hex(0xFFFFFF);
		this->color.separator = Hex(0xD5DEE2);
		this->color.border = Hex(0xD0D9DD);
		this->color.borderStrong = Hex(0xB7C4CA);
		this->color.selected = this->color.accent;
		this->color.selectedFill = this->color.accent;
		this->color.tagFill = Hex(0xE8EEF1);
		this->color.tagLabel = Hex(0x0B2937);
		this->color.tagBorder = Hex(0xD0D9DD);
	}

	this->color.studyFront = this->color.card;
	this->color.studyBack = this->dark ? Hex(0x1A5A72) : Hex(0xD7F0F8);
}

inline const Theme& Theme::Get() {
	static Theme theme;
	if (theme.dark != IsDarkAppearance()) {
		theme = Theme();
	}
	return theme;
}

inline wxColour Theme::CanvasBehind(const wxWindow* window) const {
	if (window != nullptr && window->GetParent() != nullptr) {
		const wxColour parent = window->GetParent()->GetBackgroundColour();
		if (parent.IsOk()) {
			return parent;
		}
	}
	return this->color.window;
}

inline wxColour Theme::LabelOn(bool selected) const {
	return selected ? this->color.onSelected : this->color.label;
}

inline wxPen Theme::HairlinePen() const {
	return wxPen(this->color.separator, this->stroke.hairline);
}

inline wxPen Theme::BorderPen() const {
	return wxPen(this->color.border, this->stroke.hairline);
}

inline void Theme::FillCanvas(wxGCDC& gc, const wxWindow* window, const wxSize& size) const {
	gc.SetPen(*wxTRANSPARENT_PEN);
	gc.SetBrush(wxBrush(this->CanvasBehind(window)));
	gc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
}

inline void Theme::DrawShadow(wxGCDC& gc, const wxRect& rect, int radiusDip, const Elevation& elev) const {
	if (elev.alpha == 0 || rect.GetWidth() < 4 || rect.GetHeight() < 4) {
		return;
	}

	gc.SetPen(*wxTRANSPARENT_PEN);
	const int layers = elev.blur > 0 ? (elev.blur > 4 ? 3 : 2) : 1;
	for (int i = layers; i >= 1; --i) {
		const int spread = i - 1;
		const unsigned char alpha = static_cast<unsigned char>(
			elev.alpha / (i + 1)
		);
		gc.SetBrush(wxBrush(wxColour(0, 0, 0, alpha)));
		gc.DrawRoundedRectangle(
			rect.x - spread,
			rect.y - spread + elev.offsetY,
			rect.width + spread * 2,
			rect.height + spread * 2,
			radiusDip + spread
		);
	}
}

inline void Theme::DrawRounded(wxGCDC& gc, const wxRect& rect, int radiusDip, const wxColour& fill, const wxColour& border) const {
	if (rect.GetWidth() < 2 || rect.GetHeight() < 2) {
		return;
	}
	if (border.IsOk() && border.Alpha() > 0) {
		gc.SetPen(wxPen(border, this->stroke.hairline));
	} else {
		gc.SetPen(*wxTRANSPARENT_PEN);
	}
	if (fill.IsOk() && fill.Alpha() > 0) {
		gc.SetBrush(wxBrush(fill));
	} else {
		gc.SetBrush(*wxTRANSPARENT_BRUSH);
	}
	gc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, radiusDip);
}

inline void Theme::DrawCard(wxGCDC& gc, wxWindow* window, const wxRect& rect, bool selected) const {
	const int radius = Dip(window, this->radius.md);
	if (selected) {
		this->DrawRounded(gc, rect, radius, this->color.selectedFill, this->color.selectedFill);
		return;
	}
	this->DrawRounded(gc, rect, radius, this->color.card, this->color.card);
}

inline void Theme::DrawPanel(wxGCDC& gc, wxWindow* window, const wxSize& client) const {
	this->FillCanvas(gc, window, client);
	const int radius = Dip(window, this->radius.md);
	this->DrawRounded(
		gc,
		wxRect(0, 0, client.GetWidth() - 1, client.GetHeight() - 1),
		radius,
		this->color.primary,
		this->color.primary
	);
}

inline void Theme::DrawWell(wxGCDC& gc, wxWindow* window, const wxRect& rect) const {
	this->DrawRounded(gc, rect, Dip(window, this->radius.well), wxColour(0, 0, 0, 0), this->color.separator);
}

inline void Theme::DrawBadge(wxGCDC& gc, wxWindow* window, const wxRect& rect) const {
	this->DrawRounded(gc, rect, Dip(window, this->radius.xs), this->color.fill, wxColour(0, 0, 0, 0));
}

inline void Theme::StyleDialog(wxWindow* root) const {
	if (root == nullptr) {
		return;
	}

	root->CallForEachChild([this](wxWindow* window) {
		if (dynamic_cast<wxButton*>(window) != nullptr) {
			return;
		}

		if (wxTextCtrl* field = dynamic_cast<wxTextCtrl*>(window)) {
			field->SetBackgroundColour(this->color.card);
			field->SetForegroundColour(this->color.label);
			wxTextAttr attr;
			attr.SetTextColour(this->color.label);
			attr.SetBackgroundColour(this->color.card);
			field->SetDefaultStyle(attr);
			return;
		}

		window->SetBackgroundColour(this->color.window);
		window->SetForegroundColour(this->color.label);
	});
	root->Refresh();
}

#endif
