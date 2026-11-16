#include "native_push_button.h"

#ifdef __WXOSX__

#include <wx/dcgraph.h>

#include <cmath>

#import <AppKit/AppKit.h>

namespace {

NSFont* ButtonFont(wxWindow* window) {
	if (window != nullptr) {
		NSFont* font = window->GetFont().OSXGetNSFont();
		if (font != nil) {
			return font;
		}
	}
	return [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSControlSizeRegular]];
}

NSString* ButtonTitle(const wxString& label) {
	const wxScopedCharBuffer utf8 = label.utf8_str();
	const char* bytes = utf8.data();
	if (bytes == nullptr) {
		return @"";
	}
	NSString* title = [NSString stringWithUTF8String:bytes];
	return title != nil ? title : @"";
}

// The same bezel a borderless-free wxButton uses (NSBezelStyleRounded).
// wxRendererNative draws NSBezelStyleShadowlessSquare instead.
NSButtonCell* PushButtonCell() {
	static NSButtonCell* cell = nil;
	if (cell == nil) {
		cell = [[NSButtonCell alloc] initTextCell:@""];
		cell.buttonType = NSButtonTypeMomentaryPushIn;
		cell.highlightsBy = NSPushInCellMask;
		cell.bezelStyle = NSBezelStyleRounded;
		cell.bordered = YES;
		cell.alignment = NSTextAlignmentCenter;
	}
	return cell;
}

void ConfigureCell(NSButtonCell* cell, wxWindow* window, const wxString& label, bool enabled, bool pressed) {
	cell.font = ButtonFont(window);
	cell.title = ButtonTitle(label);
	cell.enabled = enabled ? YES : NO;
	cell.highlighted = (enabled && pressed) ? YES : NO;
	cell.state = (enabled && pressed) ? NSControlStateValueOn : NSControlStateValueOff;
}

}

wxSize PlatformPushButtonSize(wxWindow* window, const wxString& label) {
	NSButtonCell* cell = PushButtonCell();
	ConfigureCell(cell, window, label, true, false);
	const NSSize size = cell.cellSize;
	if (size.width < 1 || size.height < 1) {
		return wxSize(0, 0);
	}
	return wxSize(
		static_cast<int>(std::ceil(size.width)),
		static_cast<int>(std::ceil(size.height))
	);
}

bool DrawPlatformPushButton(
	wxWindow* window,
	wxDC& dc,
	const wxRect& rect,
	const wxString& label,
	bool enabled,
	bool pressed
) {
	if (window == nullptr || rect.width < 2 || rect.height < 2) {
		return false;
	}

	wxGCDCImpl* gcdc = wxDynamicCast(dc.GetImpl(), wxGCDCImpl);
	if (gcdc == nullptr || gcdc->GetGraphicsContext() == nullptr) {
		return false;
	}
	void* native = gcdc->GetGraphicsContext()->GetNativeContext();
	if (native == nullptr) {
		return false;
	}

	NSButtonCell* cell = PushButtonCell();
	ConfigureCell(cell, window, label, enabled, pressed);

	const CGContextRef cgContext = static_cast<CGContextRef>(native);
	const NSRect controlRect = NSMakeRect(rect.x, rect.y, rect.width, rect.height);
	NSView* view = nil;
	if (window->GetHandle() != nullptr) {
		view = (NSView*)window->GetHandle();
	}

	CGContextSaveGState(cgContext);
	NSGraphicsContext* former = [NSGraphicsContext currentContext];
	[NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithCGContext:cgContext flipped:YES]];
	[cell drawWithFrame:controlRect inView:view];
	[NSGraphicsContext setCurrentContext:former];
	CGContextRestoreGState(cgContext);
	return true;
}

#endif
