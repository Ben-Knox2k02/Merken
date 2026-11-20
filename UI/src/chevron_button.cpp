#include "chevron_button.h"
#include "native_push_button.h"
#include <wx/dcbuffer.h>
#include <wx/renderer.h>

namespace {

wxPoint GlyphDrawOrigin(wxDC& dc, const wxString& glyph, const wxSize& buttonSize) {
	wxCoord textWidth = 0;
	wxCoord textHeight = 0;
	dc.GetTextExtent(glyph, &textWidth, &textHeight);
	if (textWidth < 1 || textHeight < 1) {
		return wxPoint(0, 0);
	}
	const int pad = 2;
	const int tempWidth = textWidth + pad * 2;
	const int tempHeight = textHeight + pad * 2;
	wxBitmap temp;
#ifdef __WXMSW__
	temp.Create(tempWidth, tempHeight, dc);
#else
	temp.Create(tempWidth, tempHeight);
#endif
	if (!temp.IsOk()) {
		return wxPoint((buttonSize.GetWidth() - textWidth) / 2, (buttonSize.GetHeight() - textHeight) / 2);
	}
	{
		wxMemoryDC memory(temp);
		memory.SetBackground(*wxWHITE_BRUSH);
		memory.Clear();
		memory.SetFont(dc.GetFont());
		memory.SetTextForeground(*wxBLACK);
		memory.DrawText(glyph, pad, pad);
	}
	const wxImage image = temp.ConvertToImage();
	int minX = tempWidth;
	int minY = tempHeight;
	int maxX = -1;
	int maxY = -1;
	for (int y = 0; y < tempHeight; ++y) {
		for (int x = 0; x < tempWidth; ++x) {
			const int lum = (image.GetRed(x, y) * 30 + image.GetGreen(x, y) * 59 + image.GetBlue(x, y) * 11) / 100;
			if (lum >= 240) {
				continue;
			}
			if (x < minX) {
				minX = x;
			}
			if (y < minY) {
				minY = y;
			}
			if (x > maxX) {
				maxX = x;
			}
			if (y > maxY) {
				maxY = y;
			}
		}
	}
	if (maxX < minX || maxY < minY) {
		return wxPoint((buttonSize.GetWidth() - textWidth) / 2, (buttonSize.GetHeight() - textHeight) / 2);
	}
	const int inkWidth = maxX - minX + 1;
	const int inkHeight = maxY - minY + 1;
	return wxPoint(
		(buttonSize.GetWidth() - inkWidth) / 2 - (minX - pad),
		(buttonSize.GetHeight() - inkHeight) / 2 - (minY - pad)
	);
}

}

ChevronButton::ChevronButton(wxWindow* parent, const wxString& glyph, const wxSize& size)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxBORDER_NONE | wxWANTS_CHARS),
	  glyph(glyph),
	  font(parent->GetFont()),
	  pressed(false),
	  inside(false) {
	this->font.SetPointSize(16);
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetMinSize(size);
	this->SetMaxSize(size);
	this->Bind(wxEVT_PAINT, &ChevronButton::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &ChevronButton::OnLeftDown, this);
	this->Bind(wxEVT_LEFT_UP, &ChevronButton::OnLeftUp, this);
	this->Bind(wxEVT_ENTER_WINDOW, &ChevronButton::OnEnter, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &ChevronButton::OnLeave, this);
}

void ChevronButton::OnPaint(wxPaintEvent&) {
	wxAutoBufferedPaintDC dc(this);
	const wxRect rect(this->GetClientSize());
	wxColour backdrop = this->GetBackgroundColour();
	if (this->GetParent() != nullptr && this->GetParent()->GetBackgroundColour().IsOk()) {
		backdrop = this->GetParent()->GetBackgroundColour();
	}
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(backdrop));
	dc.DrawRectangle(rect);

	const bool enabled = this->IsEnabled();
	const bool down = enabled && this->pressed && this->inside;
	// An empty label draws only the face. The glyph is centered on its ink
	// afterwards, which the button title rect does not do.
	if (!DrawPlatformPushButton(this, dc, rect, wxString(), enabled, down, this->HasFocus())) {
		int flags = 0;
		if (!enabled) {
			flags |= wxCONTROL_DISABLED;
		} else if (down) {
			flags |= wxCONTROL_PRESSED;
		} else if (this->inside) {
			flags |= wxCONTROL_CURRENT;
		}
		if (this->HasFocus()) {
			flags |= wxCONTROL_FOCUSED;
		}
		wxRendererNative::Get().DrawPushButton(this, dc, rect, flags);
	}

	dc.SetFont(this->font);
	dc.SetTextForeground(
		enabled
			? wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT)
			: wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT)
	);
	const wxPoint origin = GlyphDrawOrigin(dc, this->glyph, rect.GetSize());
	dc.DrawText(this->glyph, origin.x, origin.y);
}

void ChevronButton::OnLeftDown(wxMouseEvent&) {
	this->pressed = true;
	this->inside = true;
	this->CaptureMouse();
	this->Refresh();
}

void ChevronButton::OnLeftUp(wxMouseEvent& event) {
	const bool fire = this->pressed && this->GetClientRect().Contains(event.GetPosition());
	this->pressed = false;
	if (this->HasCapture()) {
		this->ReleaseMouse();
	}
	this->Refresh();
	if (!fire) {
		return;
	}
	wxCommandEvent click(wxEVT_BUTTON, this->GetId());
	click.SetEventObject(this);
	this->ProcessWindowEvent(click);
}

void ChevronButton::OnEnter(wxMouseEvent&) {
	this->inside = true;
	this->Refresh();
}

void ChevronButton::OnLeave(wxMouseEvent&) {
	this->inside = false;
	this->Refresh();
}

wxSize ChevronButtonSize(wxWindow* window, const wxString& glyph) {
	wxFont font = window->GetFont();
	font.SetPointSize(16);
	wxMemoryDC measure;
	wxBitmap measureBitmap(8, 8);
	measure.SelectObject(measureBitmap);
	measure.SetFont(font);
	const int textWidth = measure.GetTextExtent(glyph).GetWidth();
	measure.SelectObject(wxNullBitmap);
	const wxSize nativeFace = PlatformPushButtonSize(window, wxString());
	int height = nativeFace.GetHeight();
	if (height < 1) {
		height = wxButton::GetDefaultSize().GetHeight();
	}
	int width = textWidth + window->FromDIP(30);
	if (nativeFace.GetWidth() > width) {
		width = nativeFace.GetWidth();
	}
	return wxSize(width, height);
}
