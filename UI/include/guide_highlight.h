#ifndef GUIDE_HIGHLIGHT_H
#define GUIDE_HIGHLIGHT_H

#include <wx/wx.h>
#include "centered_message.h"

#ifdef __WXOSX__
#include <objc/message.h>
#include <objc/runtime.h>
#endif

enum class GuidePrompt {
	None,
	CreateDeck,
	AddCard,
	StudyDeck,
	ShowAnswer,
	Grade,
	Back
};

// One OK dialog per guide step, shown together with a red border on that step's button.
// The dialog is not shown again until the step changes.
namespace GuideHighlight {

struct Session {
	GuidePrompt announced = GuidePrompt::None;
	GuidePrompt highlighted = GuidePrompt::None;
	int ticket = 0;
	int pendingTicket = 0;
	bool showing = false;
};

inline Session& State() {
	static Session session;
	return session;
}

inline void SetBorder(wxWindow* window, bool on) {
	if (window == nullptr) {
		return;
	}
#ifdef __WXOSX__
	id view = reinterpret_cast<id>(window->GetHandle());
	if (view == nullptr) {
		return;
	}

	using VoidBool = void (*)(id, SEL, bool);
	using VoidDouble = void (*)(id, SEL, double);
	using VoidPtr = void (*)(id, SEL, void*);
	using IdFn = id (*)(id, SEL);
	using ColorFn = id (*)(id, SEL, double, double, double, double);

	id layer = reinterpret_cast<IdFn>(objc_msgSend)(view, sel_registerName("layer"));
	if (!on) {
		if (layer == nullptr) {
			return;
		}
		reinterpret_cast<VoidDouble>(objc_msgSend)(layer, sel_registerName("setBorderWidth:"), 0.0);
		reinterpret_cast<VoidBool>(objc_msgSend)(view, sel_registerName("setWantsLayer:"), false);
		return;
	}

	reinterpret_cast<VoidBool>(objc_msgSend)(view, sel_registerName("setWantsLayer:"), true);
	layer = reinterpret_cast<IdFn>(objc_msgSend)(view, sel_registerName("layer"));
	if (layer == nullptr) {
		return;
	}

	reinterpret_cast<VoidBool>(objc_msgSend)(layer, sel_registerName("setMasksToBounds:"), false);
	reinterpret_cast<VoidDouble>(objc_msgSend)(layer, sel_registerName("setBorderWidth:"), 2.0);

	id red = reinterpret_cast<ColorFn>(objc_msgSend)(
		reinterpret_cast<id>(objc_getClass("NSColor")),
		sel_registerName("colorWithCalibratedRed:green:blue:alpha:"),
		0.86,
		0.15,
		0.15,
		1.0
	);
	void* cgColor = reinterpret_cast<IdFn>(objc_msgSend)(red, sel_registerName("CGColor"));
	reinterpret_cast<VoidPtr>(objc_msgSend)(layer, sel_registerName("setBorderColor:"), cgColor);
	reinterpret_cast<VoidDouble>(objc_msgSend)(layer, sel_registerName("setCornerRadius:"), 6.0);
#else
	window->SetBackgroundColour(on ? wxColour(220, 38, 38) : wxNullColour);
	window->Refresh();
#endif
}

inline void Announce(
	wxWindow* button,
	GuidePrompt step,
	const wxString& message,
	wxWindow* also = nullptr
) {
	if (button == nullptr || step == GuidePrompt::None) {
		return;
	}

	Session& session = State();
	if (session.showing) {
		return;
	}
	if (session.announced == step && session.highlighted == step) {
		SetBorder(button, true);
		if (also != nullptr) {
			SetBorder(also, true);
		}
		return;
	}

	const int ticket = ++session.ticket;
	session.announced = step;
	session.pendingTicket = ticket;
	button->CallAfter([button, also, step, message, ticket]() {
		Session& current = State();
		if (current.pendingTicket != ticket || current.showing) {
			return;
		}
		current.showing = true;
		current.highlighted = step;
		current.announced = step;
		SetBorder(button, true);
		if (also != nullptr) {
			SetBorder(also, true);
		}
		if (wxWindow* top = wxGetTopLevelParent(button)) {
			top->Update();
		}
		ShowCenteredMessage(button, message, "Next step", wxOK | wxICON_INFORMATION, button, also);
		current.showing = false;
		current.pendingTicket = 0;
	});
}

}

#endif
