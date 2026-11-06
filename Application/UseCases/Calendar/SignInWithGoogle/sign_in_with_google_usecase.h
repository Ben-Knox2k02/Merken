#ifndef SIGN_IN_WITH_GOOGLE_USE_CASE_H
#define SIGN_IN_WITH_GOOGLE_USE_CASE_H

#include "sign_in_with_google_response.h"
#include "../../../Services/calendar_auth_service.h"

class SignInWithGoogleUseCase {
	public:
		explicit SignInWithGoogleUseCase(ICalendarAuthService& authService)
			: authService(authService) {}

		SignInWithGoogleResponse Execute();

	private:
		ICalendarAuthService& authService;
};

#endif