#include "system_clock_service.h"
#include <chrono>

long long SystemClockService::NowSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}