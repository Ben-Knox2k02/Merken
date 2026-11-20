#ifndef FAKE_AUTH_HELPERS_H
#define FAKE_AUTH_HELPERS_H

#include <functional>
#include <string>
#include "../../Application/Services/browser_launcher.h"
#include "../../Application/Services/loopback_receiver.h"
#include "../../Application/Services/clock_service.h"

class FakeBrowserLauncher : public IBrowserLauncher {
    public:
        bool openSucceeds = true;
        int openCount = 0;
        std::string lastUrl;

        bool Open(const std::string& url) override {
            this->openCount += 1;
            this->lastUrl = url;
            return this->openSucceeds;
        }
};

class FakeLoopbackReceiver : public ILoopbackReceiver {
    public:
        int port = 54321;
        int startCount = 0;
        int stopCount = 0;
        int lastTimeout = 0;

        // Called when SignIn waits for the redirect. The test builds the redirect target here,
        // usually from the state value that SignIn put in the browser URL.
        std::function<std::string()> onWait;

        int Start() override {
            this->startCount += 1;
            return this->port;
        }

        std::string WaitForRedirect(int timeoutSeconds) override {
            this->lastTimeout = timeoutSeconds;
            return this->onWait ? this->onWait() : std::string();
        }

        void Stop() override {
            this->stopCount += 1;
        }
};

class FakeClock : public IClockService {
    public:
        long long now = 1000000;

        long long NowSeconds() override {
            return this->now;
        }
};

#endif