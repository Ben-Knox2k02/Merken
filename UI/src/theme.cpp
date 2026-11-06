#include "theme.h"

ThemeChoiceState& ThemeChoice() {
	static ThemeChoiceState state;
	return state;
}

void Theme::ApplyTitleBar(wxWindow* window) {
	if (window == nullptr) {
		return;
	}
#ifdef __WXMSW__
	HWND hwnd = reinterpret_cast<HWND>(window->GetHWND());
	if (hwnd == nullptr) {
		return;
	}
	HMODULE library = ::LoadLibraryW(L"dwmapi.dll");
	if (library == nullptr) {
		return;
	}
	using SetAttribute = HRESULT (WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
	auto setAttribute = reinterpret_cast<SetAttribute>(
		::GetProcAddress(library, "DwmSetWindowAttribute")
	);
	if (setAttribute != nullptr) {
		BOOL dark = IsDarkAppearance() ? TRUE : FALSE;
		// 20 is DWMWA_USE_IMMERSIVE_DARK_MODE. 19 is the pre-20H1 value.
		const DWORD modes[] = { 20, 19 };
		for (DWORD mode : modes) {
			setAttribute(hwnd, mode, &dark, sizeof(dark));
		}
	}
	::FreeLibrary(library);
#else
	(void)window;
#endif
}
