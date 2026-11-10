#ifndef WINDOWS_BROWSER_LAUNCHER_H
#define WINDOWS_BROWSER_LAUNCHER_H

#include <string>
#include "../../Application/Services/browser_launcher.h"

class WindowsBrowserLauncher : public IBrowserLauncher {
public:
    bool Open(const std::string& url) override;
};

#endif