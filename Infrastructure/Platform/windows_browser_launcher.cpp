#include "windows_browser_launcher.h"

#ifndef _WIN32

bool WindowsBrowserLauncher::Open(const std::string&) {
	return false;
}

#else

#include <windows.h>
#include <shellapi.h>
#include <objbase.h>

bool WindowsBrowserLauncher::Open(const std::string& url) {
	if (url.rfind("https://", 0) != 0) {
		return false;
	}
	// Sign-in runs on a background thread. ShellExecute expects COM on that thread.
	const HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	HINSTANCE result = ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	if (SUCCEEDED(comResult)) {
		CoUninitialize();
	}
	return reinterpret_cast<INT_PTR>(result) > 32;
}

#endif
