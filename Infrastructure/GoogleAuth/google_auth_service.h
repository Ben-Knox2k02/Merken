#ifndef GOOGLE_AUTH_SERVICE_H
#define GOOGLE_AUTH_SERVICE_H

#include "../../Application/Services/calendar_auth_service.h"
#include "../../Application/Services/http_client.h"
#include "../../Application/Services/app_settings_service.h"
#include "../../Application/Services/browser_launcher.h"
#include "../../Application/Services/loopback_receiver.h"
#include "../../Application/Services/clock_service.h"
#include "google_client_config.h"

class GoogleAuthService : public ICalendarAuthService {
    public:
        GoogleAuthService(
            IHttpClient& httpClient,
            IAppSettingsService& appSettingsService,
            IBrowserLauncher& browserLauncher,
            ILoopbackReceiver& loopbackReceiver,
            IClockService& clock,
            GoogleClientConfig config
        ) : httpClient(httpClient),
            appSettingsService(appSettingsService),
            browserLauncher(browserLauncher),
            loopbackReceiver(loopbackReceiver),
            clock(clock),
            config(config) {}

        bool SignIn() override;
        void SignOut() override;
        bool IsSignedIn() override;
        std::string GetAccessToken() override;

    private:
        bool ExchangeCode(const std::string& code, const std::string& codeVerifier, const std::string& redirectUri);
        std::string RefreshAccessToken(AppSettings settings);

        IHttpClient& httpClient;
        IAppSettingsService& appSettingsService;
        IBrowserLauncher& browserLauncher;
        ILoopbackReceiver& loopbackReceiver;
        IClockService& clock;
        GoogleClientConfig config;
};

#endif