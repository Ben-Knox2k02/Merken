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

		bool SignIn() override {
			if (!this->signInSucceeds) {
				return false;
			}
			if (this->token.empty()) {
				this->token = "token";
			}
			return true;
		}

		void SignOut() override {
			this->token.clear();
		}

		bool IsSignedIn() override {
			return !this->token.empty();
		}

		std::string GetAccessToken() override {
			if (this->throwOnGetAccessToken) {
				throw std::runtime_error("google unavailable");
			}
			return this->token;
		}
};

#endif
