#include "windows_browser_launcher.h"
#include <windows.h>
#include <shellapi.h>

bool WindowsBrowserLauncher::Open(const std::string& url) {
    if (url.rfind("https://", 0) != 0) {
        return false;
    }
    HINSTANCE result = ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(result) > 32;
}