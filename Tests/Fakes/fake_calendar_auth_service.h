#ifndef FAKE_CALENDAR_AUTH_SERVICE_H
#define FAKE_CALENDAR_AUTH_SERVICE_H

#include <stdexcept>
#include <string>
#include "../../Application/Services/calendar_auth_service.h"

class FakeCalendarAuthService : public ICalendarAuthService {
	public:
		std::string token;
		bool signInSucceeds = true;
		bool throwOnGetAccessToken = false;
		bool signedIn = false;
		int signInCount = 0;
		int signOutCount = 0;

		bool SignIn() override {
			this->signInCount += 1;
			if (!this->signInSucceeds) {
				this->signedIn = false;
				return false;
			}
			if (this->token.empty()) {
				this->token = "token";
			}
			this->signedIn = true;
			return true;
		}

		void SignOut() override {
			this->signOutCount += 1;
			this->token.clear();
			this->signedIn = false;
		}

		bool IsSignedIn() override {
			return this->signedIn || !this->token.empty();
		}

		std::string GetAccessToken() override {
			if (this->throwOnGetAccessToken) {
				throw std::runtime_error("google unavailable");
			}
			return this->token;
		}
};

#endif
