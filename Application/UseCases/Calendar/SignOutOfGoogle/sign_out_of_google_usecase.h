#ifndef SIGN_OUT_OF_GOOGLE_USE_CASE_H
#define SIGN_OUT_OF_GOOGLE_USE_CASE_H

#include "../../../Services/calendar_auth_service.h"

class SignOutOfGoogleUseCase {
	public:
		explicit SignOutOfGoogleUseCase(ICalendarAuthService& authService)
			: authService(authService) {}

		void Execute();

	private:
		ICalendarAuthService& authService;
};

#endif