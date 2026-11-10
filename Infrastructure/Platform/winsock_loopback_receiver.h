#ifndef WINSOCK_LOOPBACK_RECEIVER_H
#define WINSOCK_LOOPBACK_RECEIVER_H

#include <cstdint>
#include <string>
#include "../../Application/Services/loopback_receiver.h"

class WinsockLoopbackReceiver : public ILoopbackReceiver {
public:
    WinsockLoopbackReceiver() = default;
    WinsockLoopbackReceiver(const WinsockLoopbackReceiver&) = delete;
    WinsockLoopbackReceiver& operator=(const WinsockLoopbackReceiver&) = delete;
    ~WinsockLoopbackReceiver() override;

    int Start() override;
    std::string WaitForRedirect(int timeoutSeconds) override;
    void Stop() override;

private:
    std::uintptr_t listener = static_cast<std::uintptr_t>(-1);
    bool winsockStarted = false;
};

#endif