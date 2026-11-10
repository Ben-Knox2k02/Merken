#ifndef SYSTEM_CLOCK_SERVICE_H
#define SYSTEM_CLOCK_SERVICE_H

#include "../../Application/Services/clock_service.h"

class SystemClockService : public IClockService {
public:
    long long NowSeconds() override;
};

#endif