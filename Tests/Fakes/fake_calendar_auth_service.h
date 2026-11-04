#ifndef FAKE_CALENDAR_AUTH_SERVICE_H
#define FAKE_CALENDAR_AUTH_SERVICE_H

#include <string>
#include "../../Application/Services/calendar_auth_service.h"

class FakeCalendarAuthService : public ICalendarAuthService {
public:
    std::string accessToken;
    bool signInSucceeds = true;
    bool signedIn = false;
    int signInCount = 0;
    int signOutCount = 0;

    bool SignIn() override { ++signInCount; signedIn = signInSucceeds; return signInSucceeds; }
    void SignOut() override { ++signOutCount; signedIn = false; accessToken.clear(); }
    bool IsSignedIn() override { return signedIn; }
    std::string GetAccessToken() override { return accessToken; }
};

#endif