#ifndef GET_GOOGLE_SIGN_IN_STATUS_USE_CASE_H
#define GET_GOOGLE_SIGN_IN_STATUS_USE_CASE_H

#include "get_google_sign_in_status_response.h"
#include "../../../Services/calendar_auth_service.h"

class GetGoogleSignInStatusUseCase {
	public:
		explicit GetGoogleSignInStatusUseCase(ICalendarAuthService& authService)
			: authService(authService) {}

		GetGoogleSignInStatusResponse Execute();

	private:
		ICalendarAuthService& authService;
};

#endif