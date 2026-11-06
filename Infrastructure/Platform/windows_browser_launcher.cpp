#include "windows_browser_launcher.h"
#include <windows.h>
#include <shellapi.h>
#include <objbase.h>

bool WindowsBrowserLauncher::Open(const std::string& url) {
    if (url.rfind("https://", 0) != 0) {
        return false;
    }
    // sign-in runs on a background thread, and ShellExecute expects COM to be set up on its thread
    const HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    HINSTANCE result = ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return reinterpret_cast<INT_PTR>(result) > 32;
}