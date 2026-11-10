#ifndef CALENDAR_AUTH_SERVICE_H
#define CALENDAR_AUTH_SERVICE_H

#include <string>

class ICalendarAuthService{
    public:
        virtual ~ICalendarAuthService() = default;

        virtual bool SignIn() = 0;
        virtual void SignOut() = 0;
        virtual bool IsSignedIn() = 0;
        virtual std::string GetAccessToken() = 0;
};

#endif