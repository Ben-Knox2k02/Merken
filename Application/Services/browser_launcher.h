#ifndef BROWSER_LAUNCHER_H
#define BROWSER_LAUNCHER_H

#include <string>

class IBrowserLauncher{
    public:
        virtual ~IBrowserLauncher() = default;

        virtual bool Open(const std::string& url) = 0;
};

#endif